Metatag: Mobile
---------------
This submodule of Metatag adds a number of new meta tags commonly used for
tailoring the experience of people using mobile devices.

Mobile:
  <meta name="theme-color" content="[VALUE]" />
  <meta name="MobileOptimized" content="[VALUE]" />
  <meta name="HandheldFriendly" content="[VALUE]" />
  <meta name="viewport" content="[VALUE]" />
  <meta http-equiv="cleartype" content="[VALUE]" />
  <link rel="amphtml" href="[VALUE]" />

iOS:
  <meta name="apple-itunes-app" content="[VALUE]" />
  <meta name="apple-mobile-web-app-capable" content="[VALUE]" />
  <meta name="apple-mobile-web-app-status-bar-style" content="[VALUE]" />
  <meta name="apple-mobile-web-app-title" content="[VALUE]" />
  <meta name="format-detection" content="[VALUE]" />
  <link rel="alternate" href="ios-app://[VALUE]" />

Android:
  <link rel="manifest" href="[VALUE]" />
  <link rel="alternate" href="android-app://[VALUE]" />

Windows:
  <meta http-equiv="X-UA-Compatible" content="[VALUE]" />
  <meta name="application-name" content="[VALUE]" />
  <meta name="msapplication-allowDomainApiCalls" content="[VALUE]" />
  <meta name="msapplication-allowDomainMetaTags" content="[VALUE]" />
  <meta name="msapplication-badge" content="[VALUE]" />
  <meta name="msapplication-config" content="[VALUE]" />
  <meta name="msapplication-navbutton" content="[VALUE]" />
  <meta name="msapplication-notification" content="[VALUE]" />
  <meta name="msapplication-square150x150logo" content="[VALUE]" />
  <meta name="msapplication-square310x310logo" content="[VALUE]" />
  <meta name="msapplication-square70x70logo" content="[VALUE]" />
  <meta name="msapplication-wide310x150logo" content="[VALUE]" />
  <meta name="msapplication-starturl" content="[VALUE]" />
  <meta name="msapplication-task" content="[VALUE]" />
  <meta name="msapplication-task-separator" content="[VALUE]" />
  <meta name="msapplication-tilecolor" content="[VALUE]" />
  <meta name="msapplication-tileimage" content="[VALUE]" />
  <meta name="msapplication-tooltip" content="[VALUE]" />
  <meta name="msapplication-window" content="[VALUE]" />


Configuration
--------------------------------------------------------------------------------
By default the two link alternate meta tags include a prefix - "android-app://" and "ios-app://". To remove this prefix just change the theme
functions, e.g.:

/**
 * Implements theme_metatag_mobile_android_app().
 *
 * Remove the default prefix.
 */
function MYTHEME_metatag_mobile_android_app($variables) {
  // Pass everything through to the normal 'link' tag theme.
  $variables['element']['#name'] = 'alternate';

  // Don't actually want this.
  // $variables['element']['#value'] = 'android-app://' . $variables['element']['#value'];

  return theme('metatag_link_rel', $variables);
}

/**
 * Implements theme_metatag_mobile_ios_app().
 *
 * Remove the default prefix.
 */
function MYTHEME_metatag_mobile_ios_app($variables) {
  // Pass everything through to the normal 'link' tag theme.
  $variables['element']['#name'] = 'alternate';

  // Don't actually want this.
  // $variables['element']['#value'] = 'ios-app://' . $variables['element']['#value'];

  return theme('metatag_link_rel', $variables);
}


Credits / Contact
--------------------------------------------------------------------------------
Originally developed by Damien McKenna [1].


References
--------------------------------------------------------------------------------
1: https://www.drupal.org/u/damienmckenna.
