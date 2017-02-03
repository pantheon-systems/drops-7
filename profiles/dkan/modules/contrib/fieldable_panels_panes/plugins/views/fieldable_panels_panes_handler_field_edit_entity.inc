<?php
/**
 * @file
 * Provide views handlers for fieldable panel panes
 */

/**
 * Field handler to present a link node edit.
 */
class fieldable_panels_panes_handler_field_edit_entity extends fieldable_panels_panes_handler_field_view_entity {
  /**
   * Renders the link.
   */
  function render_link($entity, $values) {
    if (fieldable_panels_panes_access('update', $entity)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "admin/structure/fieldable-panels-panes/view/$entity->fpid/edit";
      $this->options['alter']['query'] = drupal_get_destination();

      $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');
      return $text;
    }
    else {
      $this->options['alter']['path'] = '';
    }
  }
}
