<?php

/**
 * @file
 * Hooks provided by the Google Analytics Reports API module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Allow modules to alter Google Analytics reported data after executing.
 *
 * @param string $name
 *   Name of Google Analytics field without "ga:" at the beginning.
 * @param mixed $value
 *   Value of current Google Analytics field.
 */
function hook_google_analytics_reports_api_reported_data_alter(&$name, &$value) {
  switch ($name) {
    case 'userType':
      $value = ($value == 'New Visitor') ? t('New Visitor') : t('Returning Visitor');
      break;

    case 'date':
      $value = strtotime($value);
      break;

    case 'yearMonth':
      $value = strtotime($value . '01');
      break;

    case 'userGender':
      $value = ($value == 'male') ? t('Male') : t('Female');
      break;
  }
}

/**
 * @} End of "addtogroup hooks".
 */
