<?php

/**
 * @file
 * Basic textfield argument to handle known ldap attributes
 */
class ldap_views_handler_argument extends views_handler_argument {

  /**
   * Build the query based upon the formula
   */
  function query() {
    if ($this->real_field == 'basedn' || $this->real_field == 'filter') {
      $add_method = 'add_' . $this->real_field;
      $this->query->$add_method($this->argument);
    }
    else {
      $this->query->add_where(0, $this->real_field, $this->argument, '=');
    }
  }
}
