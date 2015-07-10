<?php

/**
 * Registration iframe form.
 */
function mailjet_register_iframe() {
    global $base_url;
    global $language;
  
    $langCodesMap = array('en' => 'en_US', 'fr' => 'fr_FR', 'de' => 'de_DE', 'es' => 'es_ES');
    $defaultLang = 'en';
    $locale = isset($langCodesMap[$language->language]) ? $langCodesMap[$language->language] : $langCodesMap[$defaultLang];
    
    $default_url = IFRAME_URL."reseller/signup?r=commerceguys&p=drupal-3.0&locale={$locale}&cb=";
    $callbackurl = urlencode($base_url . '?q=/admin/config/system/mailjet/api/register/alter_callback');
    return '<iframe src="' . $default_url . $callbackurl . '" width="100%" height="1080px" frameborder="0"></iframe>';
}

/**
 * Check for user api.
 */
function mailjet_plans_iframe() {
  if (!variable_get('mailjet_username') && !variable_get('mailjet_password')) {
    drupal_set_message(t('You need to add your Mailjet API details before you can continue'), 'warning');
    drupal_goto('admin/config/system/mailjet/api');
  }

  $token = variable_get("APItoken");
  if (!empty($token)) {
      
    global $language;
  
    $langCodesMap = array('en' => 'en_US', 'fr' => 'fr_FR', 'de' => 'de_DE', 'es' => 'es_ES');
    $defaultLang = 'en';
    $locale = isset($langCodesMap[$language->language]) ? $langCodesMap[$language->language] : $langCodesMap[$defaultLang];
    
    $callbackurl = 'r=commerceguys&t='.$token.'&show_menu=none&locale='.$locale;

    $default_url = IFRAME_URL."reseller/pricing/?";

    return '<iframe src="' . $default_url . $callbackurl . '" width="100%" height="1080px" frameborder="0"></iframe>';
  }
  return t('Unable to generate upgrade form.');
}
