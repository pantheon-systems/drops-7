<?php

/**
 * @file
 * A handler to link to the Lingotek Workbench.
 *
 * Definition terms:
 *   - output formats: An array where the first entry is displayed on boolean true
 *      and the second is displayed on boolean false. An example for sticky is:
 *      @code
 *      'output formats' => array(
 *        'sticky' => array(t('Sticky'), ''),
 *      ),
 *      @endcode
 *
 * @ingroup views_field_handlers
 */
class lingotek_views_handler_workbench_link extends views_handler_field {
  /**
   * Constructor; calls to base object constructor.
   */
  function construct() {
    parent::construct();
  }

  
  /**
   * Implements views_handler#options_form().
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['custom_label']['#default_value'] = FALSE;
    $form['label']['#default_value'] = '';
  }

  function render($values) {
    global $language;
    
    $entity_type = $this->view->base_table;
    
    if ($entity_type == 'node') {
      $entity = node_load($values->nid);
    }
    elseif ($entity_type == 'comment') {
      $entity = comment_load($values->cid);
    }

    if (!user_access('manage projects') && !user_access('translation')) {
      return; //do not show if the user does not have correct permissions
    }
        
    if ($language->language == $entity->language) {
      return;
    }
    
    drupal_add_css(drupal_get_path('module', 'lingotek') . '/style/workbench-link.css', array('group' => CSS_DEFAULT));

    $text = '<img src="' . base_path() . drupal_get_path('module', 'lingotek') . '/images/ico_chevs_dd.png" >'; 
    $url = 'node/' . $values->nid . '/lingotekworkbench/' . Lingotek::convertDrupal2Lingotek($language->language);
    
    return l($text, $url, array('attributes' => array('target' => '_blank', 'title' => 'Edit this translation', 'class' => 'lingotek-translation-link'), 'html' => TRUE));
  }

  function element_type($none_supported = FALSE, $default_empty = FALSE, $inline = FALSE) {
    if ($inline) {
      return 'span';
    }

    if (isset($this->definition['element type'])) {
      return $this->definition['element type'];
    }

    return 'div';
  }
}
