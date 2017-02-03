<?php

/**
 * Views field handler for rendering node links that point to panelizer tabs.
 */
class panelizer_handler_field_link extends views_handler_field_node_link {
  /**
   * Mapping of node paths (the part after node/N) to human-readable labels.
   *
   * This array is used both for the options_form in the select dropdown, and
   * also when rendering the link to define the link text if not overridden.
   */
  protected $tab_map = array();

  function construct() {
    parent::construct();
    $this->tab_map = array(
      'settings' => t('Settings'),
      'context' => t('Context'),
      'layout' => t('Layout'),
      'content' => t('Content'),
    );
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['panelizer_tab'] = array('default' => 'panelizer');
    $options['view_mode'] = array('default' => 'page_manager');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['panelizer_tab'] = array(
      '#type' => 'select',
      '#title' => 'Panelizer tab to link to',
      '#options' => $this->tab_map,
      '#default_value' => $this->options['panelizer_tab'],
    );

    $entity_type = $this->definition['entity_type'];
    $handler = panelizer_entity_plugin_get_handler($entity_type);

    $view_modes = array();
    foreach ($handler->plugin['view modes'] as $view_mode => $view_mode_info) {
      $view_modes[$view_mode] = $view_mode_info['label'];
    }

    $form['view_mode'] = array(
      '#type' => 'select',
      '#title' => t('View mode'),
      '#options' => $view_modes,
    );
  }

  function render_link($entity, $values) {
    if (!$entity) {
      return;
    }

    $entity_type = $this->definition['entity_type'];
    $handler = panelizer_entity_plugin_get_handler($entity_type);

    list($entity_id, $revision_id, $bundle) = entity_extract_ids($entity_type, $entity);

    if ($handler && $handler->is_panelized($bundle) && $handler->panelizer_access($this->options['panelizer_tab'], $entity, $this->options['view_mode'])) {
      $this->options['alter']['make_link'] = TRUE;

      $path = str_replace("%$entity_type", $entity_id, $handler->plugin['entity path']) . '/panelizer/' . $this->options['view_mode'] . '/' . $this->options['panelizer_tab'];
      $this->options['alter']['path'] = $path;
      $this->options['alter']['query'] = drupal_get_destination();
      $text = !empty($this->options['text']) ? $this->options['text'] : $this->tab_map[$this->options['panelizer_tab']];
      return $text;
    }
  }
}
