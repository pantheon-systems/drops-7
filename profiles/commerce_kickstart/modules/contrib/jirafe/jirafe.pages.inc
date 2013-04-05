<?php

/**
 * Page callback: render an iframe that will load the dashboard.
 */
function _jirafe_dashboard_page() {
  $output['jirafe']['#attached']['css'][] = drupal_get_path('module', 'jirafe') . '/jirafe.css';
  $config = variable_get('jirafe_config');
  if (!empty($config['token']) && !empty($config['app_id'])) {
    $markup = theme('jirafe_dashboard', array('url' => url('admin/commerce/jirafe/inline')));
  }
  else {
    global $base_url;
    $markup = t('Your Jirafe account is not configured yet! If you already got your application ID and token, enter them !configuration_link.<br/><br/>To create your Jirafe account visit the Commerce Guys !marketplace', array('!configuration_link' =>  l('here', 'admin/commerce/config/jirafe'), '!marketplace' => l('Marketplace', url('https://marketplace.commerceguys.com/service/jirafe', array('query' => array('source' => $base_url))))));
  }
  $output['jirafe']['#markup'] = $markup;
  return $output;
}

/**
 * Page callback: render the dashboard itself.
 */
function _jirafe_dashboard_page_inline() {
  $config = variable_get('jirafe_config');

  $variables = array(
    'data' => array(
      'api_url' => 'https://api.jirafe.com/v1',
      'api_token' => $config['token'],
      'app_id' => $config['app_id'],
      'version' => 'drupal-v0.1.0',
    ),
  );
  echo theme('jirafe_dashboard_iframe', $variables);

  return drupal_exit();
}
