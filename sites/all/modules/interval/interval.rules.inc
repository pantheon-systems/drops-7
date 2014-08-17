<?php
/**
 * @file
 * Provides rules integration for interval module
 * @copyright Copyright(c) 2011 Lee Rowlands
 * @license GPL v2+ http://www.fsf.org/licensing/licenses/gpl.html
 * @author Lee Rowlands contact at rowlandsgroup dot com
 *
 */

/**
 * Implements hook_rules_action_info().
 */
function interval_rules_action_info() {

  $actions['interval_apply'] = array(
    'label' => t('Apply an interval to a date'),
    'parameter' => array(
      'interval' => array(
        'type' => 'integer',
        'label' => t('Interval number'),
        'description' => t('The number of multiples of the interval period.'),
      ),
      'period' => array(
        'type' => 'token',
        'label' => t('Interval period'),
        'options list' => 'interval_period_options_list',
      ),
      'date' => array(
        'type' => 'date',
        'label' => t('Date'),
      ),
    ),
    'provides' => array(
      'date' => array(
        'type' => 'date',
        'label' => t('New date'),
      ),
    ),
    'group' => t('Data'),
    'base' => 'interval_rules_apply_interval',
  );
  return $actions;
}

/**
 * Rules action callback: Apply an interval to a date.
 */
function interval_rules_apply_interval($interval, $period, $datestamp) {
  $date = new DateObject("@$datestamp");
  $item = array(
    'interval' => $interval,
    'period' => $period
  );
  interval_apply_interval($date, $item);
  return array('date' => $date->format('U'));
}
