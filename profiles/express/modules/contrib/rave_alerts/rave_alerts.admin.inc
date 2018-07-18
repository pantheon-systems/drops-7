<?php
/**
 * @file
 * Administrative configurations for the RAVE Alerts module
 */

/**
 * Page callback for admin/config/system/cu_alerts.
 */
function rave_alerts_admin_settings_form() {
  $form = array();

  // @TODO: Add connection confirmation
  // @TODO: Add cache_clear_all('*', 'cache_rave_alerts', TRUE);

  //if (variable_get('rave_alerts_rss_url', FALSE)) {
    //$data = cu_alerts_remote_data();
    //dsm($data);
  //}

  $form['rave_alerts_rss_url'] = array(
    '#type' => 'textfield',
    '#title' => 'RAVE RSS URL',
    '#default_value' => variable_get('rave_alerts_rss_url', 'http://www.getrave.com/rss/DemoUniversityAlert/channel8'),
  );
  $form['rave_alerts_deafult_read_more_url'] = array(
    '#type' => 'textfield',
    '#title' => 'Default Read More URL',
    '#default_value' => variable_get('rave_alerts_deafult_read_more_url', ''),
    '#description' => t('This will be included as a Read More link in the banner unless a link is provided from RSS')
  );
  $form['rave_alerts_network_fail_message'] = array(
    '#type' => 'textarea',
    '#cols' => 40,
    '#rows' => 5,
    '#default_value' => variable_get('rave_alerts_network_fail_message', 'Network Failure: Please go directly to [YOUR ALERT SITE] for more information.'),
    '#title' => t('Network Failure Message'),
    '#description' => t('This message is displayed when the RAVE Alerts module cannot connect to the url of source multiple times.'),
  );
  $form['rave_alerts_clear_text'] = array(
    '#type' => 'textfield',
    '#title' => 'Clear Text',
    '#default_value' => variable_get('rave_alerts_clear_text', '[CLEAR]'),
    '#description' => t('When this text appears in the description field of a Rave Alert, the website alerts will stop displaying'),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advanced']['rave_alerts_check_enable'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('rave_alerts_check_enable', 1),
    '#title' => t('Enable Alerts Check'),
    '#description' => t('This must be enabled to check the source URL for new RSS items.'),
  );
  $form['advanced']['rave_alerts_display'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('rave_alerts_display', 0),
    '#title' => t('Display Alerts'),
    '#description' => t('This will be on when the module sees a new alert in the RSS feed and remain on untill a item that includes the CLEAR token is found.'),
  );

  /**
* DO NOT USE YET.  THIS WILL GENERATE MORE TRAFFIC THAN RAVE MAY BE ABLE TO HANDLE
  $form['advanced']['rave_alerts_active_text'] = array(
    '#type' => 'textfield',
    '#title' => 'Active Mode Text',
    '#default_value' => variable_get('rave_alerts_active_text', '[ACTIVE]'),
    '#description' => t('When this text appears in the description field of a Rave Alert, the website alerts will switch to active mode and use javascript to update the alert from every client every 10 seconds.  ONLY USE WITH REVERSE PORXY CACHE LIKE VARNISH.'),
  );
 */

  return system_settings_form($form);
}
