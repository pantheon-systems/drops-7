<?php

/**
 * @file
 * Definition of administerusersbyrole_handler_field_user_link_edit.
 */

/**
 * Field handler to present a link to user edit.
 *
 * Identical to the base class except change call to user_edit_access() for our updated access function.
 *
 * @ingroup views_field_handlers
 */
class administerusersbyrole_handler_field_user_link_edit extends views_handler_field_user_link_edit {
  function render_link($data, $values) {
    // Pull the access information from the menu router item.
    $item = menu_get_item("user/$data/edit");

    if ($item['access']) {
      $this->options['alter']['make_link'] = TRUE;

      $text = !empty($this->options['text']) ? $this->options['text'] : t('edit');

      $this->options['alter']['path'] = "user/$data/edit";
      $this->options['alter']['query'] = drupal_get_destination();

      return $text;
    }
  }
}
