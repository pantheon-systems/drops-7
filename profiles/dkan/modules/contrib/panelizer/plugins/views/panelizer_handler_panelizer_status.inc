<?php

/**
 * Views field handler for rendering node links that point to panelizer tabs.
 */
class panelizer_handler_panelizer_status extends views_handler_field_entity {
  function option_definition() {
    $options = parent::option_definition();
    $options['view_mode'] = array('default' => 'page_manager');
    $options['not_panelized'] = array('default' => 'Not panelized', 'translatable' => TRUE);
    $options['custom'] = array('default' => 'Custom', 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
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

    $form['custom'] = array(
      '#type' => 'textfield',
      '#title' => t('Customized text'),
      '#default_value' => $this->options['custom'],
      '#description' => t('Text to display if the entity has a custom display.'),
    );
    $form['not_panelized'] = array(
      '#type' => 'textfield',
      '#title' => t('Not panelized text'),
      '#default_value' => $this->options['not_panelized'],
      '#description' => t('Text to display if the entity is not panelized.'),
    );
  }

  function render($values) {
    if ($entity = $this->get_value($values)) {
      $view_mode = $this->options['view_mode'];
      if (!empty($entity->panelizer[$view_mode]->name)) {
        $panelizer = ctools_export_crud_load('panelizer_defaults', $entity->panelizer[$view_mode]->name);
        $status = !empty($panelizer->title) ? check_plain($panelizer->title) : t('Default');
      }
      else if (!empty($entity->panelizer[$view_mode]->did)) {
        $status = $this->options['custom'];
      }
      else {
        $status = $this->options['not_panelized'];
      }

      return $status;
    }
  }

}
