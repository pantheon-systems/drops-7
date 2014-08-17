<?php
/**
 * @file
 * Administrative page callbacks for the AddThis-module.
 */

/**
 * Form handler for admin_settings_form.
 */
function addthis_admin_settings_form($form_state) {
  AddThis::getInstance()->addStylesheets();

  // Visual settings.
  $form['fieldset_compact_menu'] = array(
    '#type' => 'fieldset',
    '#title' => t('Compact menu'),
    '#collapsible' => FALSE,
    '#collapsed' => TRUE,
    '#description' => '<p>' . t('Configure the global behavior and style of the compact menu and some additional settings related to the interface.') . '</p>'
  );
  $form['fieldset_compact_menu']['fieldset_menu_style'] = array(
    '#type' => 'fieldset',
    '#title' => t('Style'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fieldset_compact_menu']['fieldset_menu_style'][AddThis::CO_BRAND_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('Branding text'),
    '#description' => t('Additional branding message to be rendered in the upper-right-hand corner of the compact menus.<br />Should be less than 15 characters in most cases to render properly.'),
    '#default_value' => AddThis::getInstance()->getCoBrand(),
    '#required' => FALSE,
    '#maxlength' => 15,
  );
  $form['fieldset_compact_menu']['fieldset_menu_style'][AddThis::UI_HEADER_COLOR_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('Header text color'),
    '#default_value' => AddThis::getInstance()->getUiHeaderColor(),
    '#description' => t('Something like #FFFFFF'),
    '#size' => 8,
    '#maxlength' => 7,
    '#required' => FALSE,
  );
  $form['fieldset_compact_menu']['fieldset_menu_style'][AddThis::UI_HEADER_BACKGROUND_COLOR_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('Header background color'),
    '#default_value' => AddThis::getInstance()->getUiHeaderBackgroundColor(),
    '#description' => t('Something like #000000'),
    '#size' => 8,
    '#maxlength' => 7,
    '#required' => FALSE,
  );
  $form['fieldset_compact_menu']['fieldset_menu_style'][AddThis::CLICK_TO_OPEN_COMPACT_MENU_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Open compact menu on click'),
    '#description' => t('Default behavior is to open compact menu on hover.'),
    '#default_value' => AddThis::getInstance()->isClickToOpenCompactMenuEnabled(),
    '#required' => FALSE,
  );
  $form['fieldset_compact_menu']['fieldset_menu_style'][AddThis::OPEN_WINDOWS_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Use pop-up windows'),
    '#description' => t('If checked, all shares will open in a new pop-up window instead of a new tab or regular browser window.'),
    '#default_value' => AddThis::getInstance()->isOpenWindowsEnabled(),
    '#required' => FALSE,
  );
  $form['fieldset_compact_menu']['fieldset_menu_style'][AddThis::UI_DELAY_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('Menu open delay'),
    '#description' => t('Delay, in milliseconds, before compact menu appears when mousing over a regular button. Capped at 500 ms.'),
    '#default_value' => AddThis::getInstance()->getUiDelay(),
    '#required' => FALSE,
    '#size' => 3,
    '#maxlength' => 3,
  );

  // Enabled services settings.
  $form['fieldset_compact_menu']['enabled_services_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Compact menu enabled services'),
    '#description' => t('The sharing services you select here will be displayed in the compact menu. If you select no services, AddThis will provide a list of frequently used services. This list is updated regularly. <b>Notice that this setting does not define what services should be display in a toolbox.</b>'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fieldset_compact_menu']['enabled_services_fieldset'][AddThis::ENABLED_SERVICES_KEY] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enabled services'),
    '#options' => AddThis::getInstance()->getServices(),
    '#default_value' => AddThis::getInstance()->getEnabledServices(),
    '#required' => FALSE,
    '#columns' => 3,
  );

  // Additional visual settings.
  $form['fieldset_compact_menu']['fieldset_additionals'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['fieldset_compact_menu']['fieldset_additionals'][AddThis::STANDARD_CSS_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Use standard AddThis stylesheet'),
    '#description' => t('If not checked, AddThis will not load standard CSS file, allowing you to style everything yourself without incurring the cost of an additional load.'),
    '#default_value' => AddThis::getInstance()->isStandardCssEnabled(),
    '#required' => FALSE,
  );
  $form['fieldset_compact_menu']['fieldset_additionals'][AddThis::COMPLIANT_508_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('508 compliant'),
    '#description' => 'If checked, clicking the AddThis button will open a new window to a page that is keyboard navigable.',
    '#default_value' => AddThis::getInstance()->get508Compliant(),
    '#required' => FALSE,
  );
  $form['fieldset_compact_menu']['fieldset_additionals'][AddThis::ADDRESSBOOK_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Use addressbook'),
    '#description' => 'If checked, the user will be able import their contacts from popular webmail services when using AddThis\'s email sharing.',
    '#default_value' => AddThis::getInstance()->isAddressbookEnabled(),
    '#required' => FALSE,
  );

  // Analytics settings.
  $profile_id = AddThis::getInstance()->getProfileId();
  $can_track_clicks = empty($profile_id) ? FALSE : TRUE;
  $form['fieldset_analytics'] = array(
    '#type' => 'fieldset',
    '#title' => t('Analytics & Tracking'),
    '#collapsible' => TRUE,
    '#collapsed' => $can_track_clicks ? TRUE : FALSE,
  );

  if (!$can_track_clicks) {
    $form['fieldset_analytics']['can_track_notice'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'div',
      '#value' => t('For click analytics and statistics you have to provide a ProfileID from <a href="http://www.addthis.com">AddThis.com</a>. Register <a href="https://www.addthis.com/register" targt="_blank">here</a>.'),
      '#attributes' => array('class' => array('messages', 'warning')),
    );
  }
  $form['fieldset_analytics'][AddThis::PROFILE_ID_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('AddThis ProfileID'),
    '#default_value' => AddThis::getInstance()->getProfileId(),
    '#required' => FALSE,
    '#size' => 25,
    '#description' => t('ProfileID at <a href="http://addthis.com/" target="_blank">AddThis.com</a>. Required for statistics.'),
  );
  $form['fieldset_analytics'][AddThis::CLICKBACK_TRACKING_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Track clickback'),
    '#description' => 'Check to allow AddThis to append a variable to your URLs upon sharing. AddThis will use this to track how many people come back to your content via links shared with AddThis. Highly recommended. Always global.',
    '#default_value' => AddThis::getInstance()->isClickbackTrackingEnabled(),
    '#required' => FALSE,
  );

  // Facebook Like tracking requires a namespace to be added.
  $rdf_enabled = module_exists('rdf');
  if (!$rdf_enabled) {
    $rdf_description = '<span class="admin-disabled">' . t('The RDF module needs to be enabled to support Facebook Like tracking support.<br />Enable the module on <a href="!modules">modules</a> page.',
      array('!modules' => base_path() . 'admin/modules')
    ) . '</span>';
  }
  else {
    $rdf_description = t('Check to enable Facebook Like tracking support. Always global.');
  }
  $form['fieldset_analytics']['title_facebook'] = array(
    '#theme' => 'html_tag',
    '#tag' => 'div',
    '#value' => '<b>' . t('Facebook') . '</b>',
  );
  $form['fieldset_analytics']['facebook_notice'] = array(
    '#theme' => 'html_tag',
    '#tag' => 'p',
    '#value' => $rdf_description,
    '#access' => !$rdf_enabled,
  );

  $form['fieldset_analytics'][AddThis::FACEBOOK_LIKE_COUNT_SUPPORT_ENABLED] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Facebook Like tracking'),
    '#description' => filter_xss($rdf_description, array('span')),
    '#default_value' => AddThis::getInstance()->isFacebookLikeCountSupportEnabled(),
    '#required' => FALSE,
    '#disabled' => !$rdf_enabled,
  );

  // Google Analytics and Google Social Tracking support.
  $can_do_google_social_tracking = module_exists('googleanalytics');
  $is_google_analytics_setup = $can_do_google_social_tracking && variable_get('googleanalytics_account', '');
  $form['fieldset_analytics']['google_analytics'] = array(
    '#theme' => 'html_tag',
    '#tag' => 'div',
    '#value' => '<b>' . t('Google Analytics') . '</b>',
  );
  if (!$can_do_google_social_tracking) {
    $form['fieldset_analytics']['can_do_google_analytics'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'p',
      '#value' => '<span class="admin-disabled">' . t('Install/enable the <a href="http://drupal.org/project/google_analytics" target="_blank">Google Analytics</a> module for Social Tracking support.') . '</span>',
    );
  }
  elseif ($can_do_google_social_tracking && !$is_google_analytics_setup) {
    $form['fieldset_analytics']['can_do_google_analytics'] = array(
      '#theme' => 'html_tag',
      '#tag' => 'p',
      '#value' => '<span class="admin-disabled">' . t('Configure the Google Analytics module correctly with the account code to use this feature.') . '</span>',
    );
  }
  $form['fieldset_analytics'][AddThis::GOOGLE_ANALYTICS_TRACKING_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Track with Google Analytics'),
    '#description' => t('Check to track shares in your Google Analytics account reports (<a href="http://www.addthis.com/help/google-analytics-integration">more info</a>). Always global.'),
    '#default_value' => AddThis::getInstance()->isGoogleAnalyticsTrackingEnabled(),
    '#required' => FALSE,
    '#disabled' => !$is_google_analytics_setup,
  );
  $form['fieldset_analytics'][AddThis::GOOGLE_ANALYTICS_SOCIAL_TRACKING_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Track with Google Analytics social'),
    '#description' => t('Check to track shares in the new Google Analytics social interaction reports (<a href="http://www.addthis.com/help/google-analytics-integration#social">more info</a>). Always global.'),
    '#default_value' => AddThis::getInstance()->isGoogleAnalyticsSocialTrackingEnabled(),
    '#required' => FALSE,
    '#disabled' => !$is_google_analytics_setup,
  );

  // Third party settings.
  $form['third_party_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Third party settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['third_party_fieldset']['twitter_service'] = array(
    '#type' => 'fieldset',
    '#title' => t('Twitter'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['third_party_fieldset']['twitter_service'][AddThis::TWITTER_VIA_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('Send via'),
    '#description' => t('When sending a tweet this is the screen name of the user to attribute the Tweet to. (Relates to Tweet Button)'),
    '#default_value' => AddThis::getInstance()->getTwitterVia(),
    '#size' => 15,
  );
  $form['third_party_fieldset']['twitter_service'][AddThis::TWITTER_TEMPLATE_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('Template text'),
    '#description' => t('The {{title}} and {{url}} are replaced from the Twitter Button.'),
    '#default_value' => AddThis::getInstance()->getTwitterTemplate(),
  );

  return system_settings_form($form);
}

/**
 * Form handler for admin_settings_advanced_form.
 */
function addthis_admin_settings_advanced_form($form_state) {

  // Service URL's settings.
  $form['service_urls_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Service URLs'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['service_urls_fieldset'][AddThis::BOOKMARK_URL_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('AddThis bookmark URL'),
    '#default_value' => AddThis::getInstance()->getBaseBookmarkUrl(),
    '#required' => TRUE,
  );
  $form['service_urls_fieldset'][AddThis::SERVICES_CSS_URL_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('AddThis services stylesheet URL'),
    '#default_value' => AddThis::getInstance()->getServicesCssUrl(),
    '#required' => TRUE,
  );
  $form['service_urls_fieldset'][AddThis::SERVICES_JSON_URL_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('AddThis services json URL'),
    '#default_value' => AddThis::getInstance()->getServicesJsonUrl(),
    '#required' => TRUE,
  );
  $form['service_urls_fieldset'][AddThis::WIDGET_JS_URL_KEY] = array(
    '#type' => 'textfield',
    '#title' => t('AddThis javascript widget URL'),
    '#default_value' => AddThis::getInstance()->getBaseWidgetJsUrl(),
    '#required' => TRUE,
  );

  // Advanced settings.
  $form['advanced_settings_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#access' => user_access(AddThis::PERMISSION_ADMINISTER_ADVANCED_ADDTHIS),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advanced_settings_fieldset'][AddThis::CUSTOM_CONFIGURATION_CODE_ENABLED_KEY] = array(
    '#type' => 'checkbox',
    '#title' => t('Use custom AddThis configuration code'),
    '#default_value' => AddThis::getInstance()->isCustomConfigurationCodeEnabled(),
    '#required' => FALSE,
    '#description' => t('Use custom AddThis configuration code. If checked, custom configuration will be used instead of other configuration settings provided in AddThis administration user interface.'),
  );
  $form['advanced_settings_fieldset'][AddThis::CUSTOM_CONFIGURATION_CODE_KEY] = array(
    '#type' => 'textarea',
    '#title' => t('AddThis custom configuration code'),
    '#default_value' => AddThis::getInstance()->getCustomConfigurationCode(),
    '#required' => FALSE,
    '#description' => t('AddThis custom configuration code. See format at <a href="http://addthis.com/" target="_blank">AddThis.com</a>'),
  );
  $form['advanced_settings_fieldset'][AddThis::WIDGET_JS_LOAD_TYPE] = array(
    '#type' => 'select',
    '#title' => t('When to load the widget js.'),
    '#options' => array(
      'async' => t('When the page loads, but load assets asynchronously. (default)'),
      'include' => t('When the page loads.'),
      'domready' => t('When the DOM is ready at the script dynamicly.'),
    ),
    '#default_value' => AddThis::getInstance()->getWidgetJsLoadType(),
    '#required' => FALSE,
  );
  return system_settings_form($form);
}
