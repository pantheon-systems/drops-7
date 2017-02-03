<?php
/**
 * @file
 * Provide views handlers for fieldable panel panes
 */

/**
 * Field handler to present a link node delete.
 */
class fieldable_panels_panes_handler_field_delete_revision extends fieldable_panels_panes_handler_field_view_revision {
  /**
   * Renders the link.
   */
  function render_link($entity, $values) {
    if (fieldable_panels_panes_access('delete', $entity)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "admin/structure/fieldable-panels-panes/view/$entity->fpid/revision/$entity->vid/delete";
      $this->options['alter']['query'] = drupal_get_destination();

      $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
      return $text;
    }
    else {
      $this->options['alter']['path'] = '';
    }
  }
}
