<?php

/**
 * Iframe for the campaigns.
 */
function mailjet_campaign_iframe() {
  if (!variable_get('mailjet_username') && !variable_get('mailjet_password')) {
    drupal_set_message(t('You need to add your Mailjet API details before you can continue'), 'warning');
    drupal_goto('admin/config/system/mailjet');
  }

  $token = variable_get("APItoken");

  if (!empty($token)) {
    global $base_url;
    global $language;

    $langCodesMap = array('en' => 'en_US', 'fr' => 'fr_FR', 'de' => 'de_DE', 'es' => 'es_ES');
    $defaultLang = 'en';
    $locale = isset($langCodesMap[$language->language]) ? $langCodesMap[$language->language] : $langCodesMap[$defaultLang];

    $callbackurl = urlencode($base_url . '?q=/admin/mailjet/campaign/alter_callback');

    $default_url = IFRAME_URL."campaigns/?f=ams&r=commerceguys&t={$token}&show_menu=none&sp=display&locale={$locale}&cb=";

    $runningBJs = variable_get('runningBtachJobs', array());
    $still_running = array();
    foreach ($runningBJs as $key => $value) {
      if(is_batchjob_running($value)){
        $still_running[] = $value;
        drupal_set_message(t('Running batcjob: '.$value.'Please note that your campaign will be sent only to the processed contacts. Maybe wait and check later if your synchronization is complete!'),'warning');
      }
    }
    variable_get('runningBtachJobs', $still_running);
    $callbackurl = urldecode($callbackurl);
    $default_url = urldecode($default_url);

    return '<iframe src="' . $default_url . $callbackurl . '" width="100%" height="1080px" frameborder="0"></iframe>';
  }
  return t('Unable to generate campaign.');
}
