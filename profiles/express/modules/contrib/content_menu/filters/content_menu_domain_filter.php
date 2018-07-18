<?php
/**
 * Created by dreizwo.de.
 * User: jakobs
 * Date: 16.08.2012
 * Time: 13:59:48
 * @author markus jakobs <jakobs@dreizwo.de>
 */

require_once 'content_menu_filter.php';

class content_menu_domain_filter implements content_menu_filter {

  private $active;
  private $domain;

  function __construct() {
    $this->domain = $this->_content_menu_domainacccess();
    $this->active = $this->domain ? TRUE : FALSE;
  }


  public function addFilterWidget(&$form, &$form_state, $form_id) {
    // add a selection filter based only if multilang{
    if ($this->active) {
      foreach ($this->_content_menu_domain_list() as $did => $dom) {
        $options[$did] = $dom['sitename'];
      }
      $form['domainselect'] = array(
        '#type' => 'select',
        '#title' => t('Filter menu by domain'),
        '#options' => $options,
        '#default_value' => $this->domain['domain_id'],
        '#ajax' => array(
          'callback' => '_content_menu_filter_elements_by_domain',
        )
      );
      $form['#content_menu_filter_widget'][] = 'domainselect';
    }
  }

  public function hideElement($el) {
    if ($this->active) {
      //hide on different lang
      if (isset($el['#item']['options']['domain_menu_access'])) {
        //unset if explicit hidden
        if (isset($el['#item']['options']['domain_menu_access']['hide']) &&
          in_array(
            'd' . $this->domain['domain_id'], $el['#item']['options']['domain_menu_access']['hide'])
        ) {
          return TRUE;
        }
        //if shown empty && not in shown => unset too
        if (!empty($el['#item']['options']['domain_menu_access']['show']) &&
          !in_array(
            'd' . $this->domain['domain_id'], $el['#item']['options']['domain_menu_access']['show'])
        ) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  private function _content_menu_domainacccess() {
    if (module_exists('domain')) {
      $domains = $this->_content_menu_domain_list();

      if (isset($_SESSION['content_menu_domain_filter'])) {
        return $domains[$_SESSION['content_menu_domain_filter']];
      }
      if (count($domains) > 1) {
        return domain_default(FALSE, FALSE);
      }
    }
    return FALSE;
  }

  private function _content_menu_domain_list() {
    $cache = &drupal_static(__FUNCTION__, array());
    if (empty($cache['domains'])) {
      $domains = array();
      $query = db_select('domain', 'd')
        ->fields('d',
        array(
          'domain_id',
          'sitename',
          'subdomain',
          'scheme',
          'valid',
          'weight',
          'is_default'
        ))
        ->orderBy('weight');
      // Get the domains.
      $result = $query->execute();
      while ($domain = $result->fetchAssoc()) {
        $domains[$domain['domain_id']] = domain_api($domain);
      }
      $cache['domains'] = $domains;
      return $domains;
    }
    return $cache['domains'];
  }
}

function _content_menu_filter_elements_by_domain($form, &$form_state) {
  $domain = $form_state['values']['domainselect'];
  ctools_include('ajax');
  $_SESSION['content_menu_domain_filter'] = $domain;
  $commands[] = ctools_ajax_command_reload();
  print ajax_render($commands);
  exit;
}
