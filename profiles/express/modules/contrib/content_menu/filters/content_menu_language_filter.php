<?php
/**
 * Created by dreizwo.de.
 * User: jakobs
 * Date: 16.08.2012
 * Time: 14:00:17
 * @author markus jakobs <jakobs@dreizwo.de>
 */

require_once 'content_menu_filter.php';

class content_menu_language_filter implements content_menu_filter {

  private $active;
  private $language;

  function __construct($menu_name) {
    $this->active = $this->_content_menu_multilanguage_support($menu_name);
    $this->language = $this->_content_menu_language();
  }

  public function addFilterWidget(&$form, &$form_state, $form_id) {
    if ($this->active) {
      $options = array(
        '' => t('-- Current (@lang) --', array('@lang' => t($GLOBALS['language']->name))),
        'all' => t('-- All --'),
      );
      foreach (language_list() as $key => $lang) {
        $options[$key] = t($lang->name);
      }
      $form['langselect'] = array(
        '#type' => 'select',
        '#title' => t('Filter menu by language'),
        '#options' => $options,
        '#default_value' => $this->language,
        // @todo Remove ctools dependency as ajax is actually not used.
        //       Refactor to use own form and submit handler instead.
        '#ajax' => array(
          'callback' => '_content_menu_filter_elements_by_language',
        )
      );
      $form['#content_menu_filter_widget'][] = 'langselect';
    }
  }

  public function hideElement($el) {
    // If this filter is active...
    $lang = $this->language;
    if ($this->active && ($lang != 'all')) {
      if ($lang == '') {
        $lang = $GLOBALS['language']->language;
      }
      // Check every menu item with a specific language...
      if (isset($el['#item']['language']) && ($el['#item']['language'] != LANGUAGE_NONE)) {
        if ($el['#item']['language'] != $lang) {
          return TRUE;
        }
      }
    }
    return FALSE;
  }

  private function _content_menu_multilanguage_support($menuname) {
    $i18n_mode = 0;
    $menu = menu_load($menuname);
    $i18n_mode = $menu && isset($menu['i18n_mode']) ? $menu['i18n_mode'] : 0;
    return (drupal_multilingual() && ($i18n_mode != 0));
  }

  private function _content_menu_language() {
    $lang = '';
    if ($this->active) {
      if (isset($_SESSION['content_menu_lang_filter'])) {
        $lang = $_SESSION['content_menu_lang_filter'];
      }
    }
    return $lang;
  }

}

function _content_menu_filter_elements_by_language($form, &$form_state) {
  $lang = $form_state['values']['langselect'];
  ctools_include('ajax');
  $_SESSION['content_menu_lang_filter'] = $lang;
  if ($lang = '') {
    unset($_SESSION['content_menu_lang_filter']);
  }
  $commands[] = ctools_ajax_command_reload();
  print ajax_render($commands);
  exit;
}



