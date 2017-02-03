<?php
/**
 * @file
 * Contains the panelizer node view row style plugin.
 */

/**
 * Plugin which renders a panelizer node.
 */
class panelizer_plugin_row_panelizer_node_view extends views_plugin_row_node_view {
  // Basic properties that let the row style follow relationships.
  var $base_table = 'node';
  var $base_field = 'nid';

  function init(&$view, &$display, $options = NULL) {
    parent::init($view, $display, $options);
    // We need to define this here, not in hook_views_plugins() due to a bug
    // in Views itself.  See http://drupal.org/node/1205376 for more info.
    $this->definition['theme'] = 'views_view_row_node';
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['render_anything'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['render_anything'] = array(
      '#type' => 'checkbox',
      '#title' => t('Also display content that is not handled by panelizer'),
      '#description' => t('If any content is not panelized, it will be hidden unless this is checked.'),
      '#default_value' => $this->options['render_anything'],
      '#weight' => -1,
    );

    // Force the row options from the parent plugin to depend on this checkbox.
    foreach (array('view_mode', 'links', 'comments') as $element_name) {
      $form[$element_name]['#dependency'] = array(
        'edit-row-options-render-anything' => array(1),
      );
    }
  }

  function render($row) {
    $node = node_load($row->nid);
    if (empty($node)) {
      return;
    }

    $handler = panelizer_entity_plugin_get_handler('node');
    $info = $handler->render_entity($node, 'page_manager');

    if (empty($info)) {
      if (!empty($this->options['render_anything'])) {
        return parent::render($row);
      }
    }
    else {
      return $info['content'];
    }
  }
}
