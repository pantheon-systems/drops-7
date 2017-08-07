<?php

/**
 * @file
 * Definition of views_handler_field_user_link_cancel.
 */

/**
 * Field handler to present a link to user cancel.
 *
 * Identical to the base class except change call to user_cancel_access() for _administerusersbyrole_can_cancel_user().
 *
 * @ingroup views_field_handlers
 */
class administerusersbyrole_handler_field_user_link_cancel extends views_handler_field_user_link_cancel {

  function render_link($data, $values) {
    $uid = $values->{$this->aliases['uid']};

    // Pull the access information from the menu router item.
    $item = menu_get_item("user/$uid/cancel");

    if ($item['access']) {
      $this->options['alter']['make_link'] = TRUE;

      $text = !empty($this->options['text']) ? $this->options['text'] : t('Cancel account');

      $this->options['alter']['path'] = "user/$uid/cancel";
      $this->options['alter']['query'] = drupal_get_destination();

      return $text;
    }
  }
}
