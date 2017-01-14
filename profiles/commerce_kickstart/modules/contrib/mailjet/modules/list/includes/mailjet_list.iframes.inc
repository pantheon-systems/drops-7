<?php

/**
 * Iframe for the lists.
 */
function mailjet_list_iframe() {
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


    $callbackurl = 'r=commerceguys&t='.$token.'&show_menu=none&sp=display&u=Drupal-3.0&locale='.$locale;

    $default_url = IFRAME_URL."contacts?";

    return '<iframe src="' . $default_url . $callbackurl . '" width="100%" height="1080px" frameborder="0"></iframe>';
  }
  return t('Unable to generate campaign.');
}
