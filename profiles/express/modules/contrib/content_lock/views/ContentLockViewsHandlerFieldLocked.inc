<?php

/**
 * @file
 * Handler to identify if node is locked or not.
 */

/**
 * Class ContentLockViewsHandlerFieldLocked.
 */
class ContentLockViewsHandlerFieldLocked extends views_handler_field_boolean {
  /**
   * Init.
   */
  public function construct() {
    parent::construct();
    $this->additional_fields['timestamp'] = 'timestamp';
  }

  /**
   * Query.
   */
  public function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Render.
   */
  public function render($values) {
    $value = $values->content_lock_timestamp ? TRUE : FALSE;
    if (!empty($this->options['not'])) {
      $value = !$value;
    }

    switch ($this->options['type']) {
      case 'true-false':
        return $value ? t('True') : t('False');

      case 'on-off':
        return $value ? t('On') : t('Off');

      case 'yes-no':
      default:
        return $value ? t('Yes') : t('No');
    }
  }

}
