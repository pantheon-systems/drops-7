<?php

/**
 * @file
 * Definition of google_analytics_reports_handler_filter_date.
 */

/**
 * A handler to provide filters for Google Analytics dates.
 *
 * @ingroup views_filter_handlers
 */
class google_analytics_reports_handler_filter_date extends views_handler_filter_date {

  function operators() {
    $operators = array(
      '=' => array(
        'title' => t('Is equal to'),
        'method' => 'op_simple',
        'short' => t('='),
        'values' => 1,
      ),
    );

    return $operators;
  }

  function query() {
    $this->ensure_my_table();
    $field = $this->real_field;

    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field);
    }
  }

  function op_simple($field) {
    $value = intval(strtotime($this->value['value'], 0));

    if (!empty($this->value['type']) && $this->value['type'] == 'offset') {
      $value = REQUEST_TIME + $value;
    }

    $this->query->add_where($this->options['group'], $field, $value, $this->operator);
  }

}
