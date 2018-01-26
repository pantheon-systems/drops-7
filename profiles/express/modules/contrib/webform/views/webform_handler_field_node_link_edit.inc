<?php

/**
 * @file
 * Views handler to display an edit link for Webform configuration.
 */

/**
 * Field handler to present a link node edit.
 */
class webform_handler_field_node_link_edit extends views_handler_field_node_link {

  /**
   *
   */
  public function option_definition() {
    $options = parent::option_definition();
    $options['subpath'] = array('default' => '');
    return $options;
  }

  /**
   *
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['subpath'] = array(
      '#type' => 'textfield',
      '#title' => t('Subpath'),
      '#default_value' => $this->options['subpath'],
      '#field_prefix' => 'node/NID/webform/',
    );
  }

  /**
   * Renders the link.
   */
  public function render_link($node, $values) {
    // Ensure node is webform-enabled and user has access to edit this node.
    if (!in_array($node->type, webform_node_types()) || !node_access('update', $node)) {
      return;
    }

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "node/$node->nid/webform" .
                                      (strlen($this->options['subpath']) ? '/' . $this->options['subpath'] : '');

    $text = !empty($this->options['text']) ? $this->options['text'] : t('edit webform');
    return $text;
  }

}
