<?php
/**
 * @file
 * Provide views handlers for fieldable panel panes
 */

/**
 * Field handler to present a link node edit.
 */
class fieldable_panels_panes_handler_field_make_current extends fieldable_panels_panes_handler_field_view_revision {
  /**
   * Renders the link.
   */
  function render_link($entity, $values) {
    if (fieldable_panels_panes_entity_make_current_access($entity)) {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "admin/structure/fieldable-panels-panes/view/$entity->fpid/revision/$entity->vid/make-current";
      $this->options['alter']['query'] = drupal_get_destination();
      $this->options['alter']['query']['token'] = drupal_get_token($entity->fpid . ':' . $entity->vid);

      $text = !empty($this->options['text']) ? $this->options['text'] : t('make current');
      return $text;
    }
    else {
      $this->options['alter']['path'] = '';
    }
  }
}
