<?php
/**
 * @file
 * Provide views handlers for fieldable panel panes
 */

/**
 * Field handler to present a link to the node.
 */
class fieldable_panels_panes_handler_field_view_entity extends views_handler_field_entity {

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => '', 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    parent::options_form($form, $form_state);

    // The path is set by render_link function so don't allow to set it.
    $form['alter']['path'] = array('#access' => FALSE);
    $form['alter']['external'] = array('#access' => FALSE);
  }

  function render($values) {
    if ($entity = $this->get_value($values)) {
      return $this->render_link($entity, $values);
    }
  }

  function render_link($entity, $values) {
    if (fieldable_panels_panes_access('view', $entity)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "admin/structure/fieldable-panels-panes/view/$entity->fpid";
      $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
      return $text;
    }
    else {
      $this->options['alter']['path'] = '';
    }
  }
}
