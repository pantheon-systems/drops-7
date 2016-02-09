<?php
/**
 * @file
 * Metatag integration for the Metatag:Facebook module.
 */

/**
 * Implements hook_metatag_info().
 */
function metatag_mobile_metatag_info() {
  $info['groups']['mobile'] = array(
    'label' => t('Mobile & UI Adjustments'),
    'description' => t("Meta tags used to control the mobile browser experience. Some of these meta tags have been replaced by newer mobile browsers. These meta tags usually only need to be set globally, rather than per-page."),
    'form' => array(
      '#weight' => 80,
    ),
  );
  $info['groups']['apple_mobile'] = array(
    'label' => t('Apple & iOS'),
    'description' => t("Custom meta tags used by Apple's software, iOS, Safari, etc."),
    'form' => array(
      '#weight' => 81,
    ),
  );
  $info['groups']['android_mobile'] = array(
    'label' => t('Android'),
    'description' => t("Custom meta tags used by the Android OS, browser, etc."),
    'form' => array(
      '#weight' => 82,
    ),
  );
  $info['groups']['windows_mobile'] = array(
    'label' => t('Windows & Windows Mobile'),
    'description' => t("Custom meta tags used by the Windows and Windows Mobile OSes, IE browser, etc."),
    'form' => array(
      '#weight' => 83,
    ),
  );

  $weight = 80;

  // Default values for each meta tag.
  $tag_info_defaults = array(
    'description' => '',
    'class' => 'DrupalTextMetaTag',
    'group' => 'mobile',
  );

  $info['tags']['theme-color'] = array(
    'label' => t('Theme Color'),
    'description' => t('A color in hexidecimal format, e.g. "#0000ff" for blue; must include the "#" symbol. Used by some browsers to control the background color of the toolbar, the color used with an icon, etc.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['MobileOptimized'] = array(
    'label' => t('Mobile Optimized'),
    'description' => t('Using the value "width" tells certain mobile Internet Explorer browsers to display as-is, without being resized. Alternatively a numerical width may be used to indicate the desired page width the page should be rendered in: "240" is the suggested default, "176" for older browsers or "480" for newer devices with high DPI screens.'),
    'weight' => ++$weight,
    'multiple' => TRUE,
  ) + $tag_info_defaults;

  $info['tags']['HandheldFriendly'] = array(
    'label' => t('Handheld-Friendly'),
    'description' => t('Some older mobile browsers will expect this meta tag to be set to "true" to indicate that the site has been designed with mobile browsers in mind.'),
    'weight' => ++$weight,
    'multiple' => TRUE,
  ) + $tag_info_defaults;

  $info['tags']['viewport'] = array(
    'label' => t('Viewport'),
    'description' => t('Used by most contemporary browsers to control the display for mobile browsers. Please read a guide on responsive web design for details of what values to use.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['cleartype'] = array(
    'label' => t('Cleartype'),
    'description' => t('A legacy meta tag for older versions of Internet Explorer on Windows, use the value "on" to enable it; this tag is ignored by all other browsers.'),
    'weight' => ++$weight,
    'element' => array(
      '#theme' => 'metatag_http_equiv',
    ),
  ) + $tag_info_defaults;

  // Default values for each meta tag.
  $tag_info_defaults = array(
    'description' => '',
    'class' => 'DrupalTextMetaTag',
    'group' => 'apple_mobile',
  );

  $info['tags']['apple-itunes-app'] = array(
    'label' => t('iTunes App details'),
    'description' => t('This informs iOS devices to display a banner to a specific app. If used, it must provide the "app-id" value, the "affiliate-data" and "app-argument" values are optional.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['apple-mobile-web-app-capable'] = array(
    'label' => t('Web app capable?'),
    'description' => t('If set to "yes", the application will run in full-screen mode; the default behavior is to use Safari to display web content.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['apple-mobile-web-app-status-bar-style'] = array(
    'label' => t('Status bar color'),
    'description' => t('Requires the "Web app capable" meta tag to be set to "yes". May be set to "default", "black", or "black-translucent".'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['format-detection'] = array(
    'label' => t('Format detection'),
    'description' => t('If set to "telephone=no" the page will not be checked for phone numbers, which would be presented .'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['ios-app-link-alternative'] = array(
    'label' => t('iOS app link alternative'),
    'description' => t('A custom string for deeplinking to an iOS mobile app. Should be in the format "itunes_id/scheme/host_path", e.g. 123456/example/hello-screen". The "ios-app://" prefix will be included automatically.'),
    'class' => 'DrupalLinkMetaTag',
    'weight' => ++$weight,
    'element' => array(
      '#theme' => 'metatag_mobile_ios_app',
    ),
    'header' => FALSE,
  ) + $tag_info_defaults;

  // Default values for each meta tag.
  $tag_info_defaults = array(
    'description' => '',
    'class' => 'DrupalTextMetaTag',
    'group' => 'android_mobile',
  );

  $info['tags']['android-app-link-alternative'] = array(
    'label' => t('Android app link alternative'),
    'description' => t('A custom string for deeplinking to an Android mobile app. Should be in the format "package_name/host_path", e.g. "com.example.android/example/hello-screen". The "android-app://" prefix will be included automatically.'),
    'class' => 'DrupalLinkMetaTag',
    'weight' => ++$weight,
    'element' => array(
      '#theme' => 'metatag_mobile_android_app',
    ),
    'header' => FALSE,
  ) + $tag_info_defaults;

  $info['tags']['android-manifest'] = array(
    'label' => t('Manifest'),
    'description' => t('A URL to a manifest.json file that describes the application. The <a href="https://developer.chrome.com/multidevice/android/installtohomescreen">JSON-based manifest</a> provides developers with a centralized place to put metadata associated with a web application.'),
    'class' => 'DrupalLinkMetaTag',
    'weight' => ++$weight,
    'element' => array(
      '#name' => 'manifest',
    ),
  ) + $tag_info_defaults;

  // Default values for each meta tag.
  $tag_info_defaults = array(
    'description' => '',
    'class' => 'DrupalTextMetaTag',
    'group' => 'windows_mobile',
  );

  $info['tags']['x-ua-compatible'] = array(
    'label' => t('X-UA-Compatible'),
    'description' => t('Indicates to IE which rendering engine should be used for the current page.'),
    'weight' => ++$weight,
    'element' => array(
      '#theme' => 'metatag_http_equiv',
    ),
  ) + $tag_info_defaults;

  $info['tags']['application-name'] = array(
    'label' => t('Application name'),
    'description' => t('The default name displayed with the pinned sites tile (or icon). Set the content attribute to the desired name.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-allowDomainApiCalls'] = array(
    'label' => t('MSApplication - Allow domain API calls'),
    'description' => t('Allows tasks to be defined on child domains of the fully qualified domain name associated with the pinned site. Should be either "true" or "false".'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-allowDomainMetaTags'] = array(
    'label' => t('MSApplication - Allow domain meta tags'),
    'description' => t('Allows tasks to be defined on child domains of the fully qualified domain name associated with the pinned site. Should be either "true" or "false".'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-badge'] = array(
    'label' => t('MSApplication - Badge'),
    'description' => t('A semi-colon -separated string that must contain the "polling-uri=" value with the full URL to a <a href="http://go.microsoft.com/fwlink/p/?LinkID=314019">Badge Schema XML file</a>. May also contain "frequency=" value set to either 30, 60, 360, 720 or 1440 (default) which specifies (in minutes) how often the URL should be polled.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-config'] = array(
    'label' => t('MSApplication - Config'),
    'description' => t('Should contain the full URL to a <a href="https://msdn.microsoft.com/en-us/library/dn320426(v=vs.85).aspx">Browser configuration schema</a> file that further controls tile customizations.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-navbutton-color'] = array(
    'label' => t('MSApplication - Nav button color'),
    'description' => t('Controls the color of the Back and Forward buttons in the pinned site browser window.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-notification'] = array(
    'label' => t('MSApplication - Notification'),
    'description' => t('A semi-colon -separated string containing "polling-uri=" (required), "polling-uri2=", "polling-uri3=", "polling-uri4=" and "polling-uri5=" to indicate the URLs for notifications. May also contain a "frequency=" value to specify how often (in minutes) the URLs will be polled; limited to 30, 60, 360, 720 or 1440 (default). May also contain the value "cycle=" to control the notifications cycle.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-square150x150logo'] = array(
    'label' => t('MSApplication - Square logo, 150px x 150px'),
    'description' => t('The URL to a logo file that is 150px by 150px.'),
    'image' => TRUE,
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-square310x310logo'] = array(
    'label' => t('MSApplication - Square logo, 310px x 310px'),
    'description' => t('The URL to a logo file that is 310px by 310px.'),
    'image' => TRUE,
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-square70x70logo'] = array(
    'label' => t('MSApplication - Square logo, 70px x 70px'),
    'description' => t('The URL to a logo file that is 70px by 70px.'),
    'image' => TRUE,
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-wide310x150logo'] = array(
    'label' => t('MSApplication - Wide logo, 310px x 150px'),
    'description' => t('The URL to a logo file that is 310px by 150px.'),
    'image' => TRUE,
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-starturl'] = array(
    'label' => t('MSApplication - Start URL'),
    'description' => t('The URL to the root page of the site.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-task'] = array(
    'label' => t('MSApplication - Task'),
    'description' => t('A semi-colon -separated string defining the "jump" list task. Should contain the "name=" value to specify the task\'s name, the "action-uri=" value to set the URL to load when the jump list is clicked, the "icon-uri=" value to set the URL to an icon file to be displayed, and "window-type=" set to either "tab" (default), "self" or "window" to control how the link opens in the browser.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-task-separator'] = array(
    'label' => t('MSApplication - Task separator'),
    'description' => t(''),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-tilecolor'] = array(
    'label' => t('MSApplication - Tile color'),
    'description' => t('The HTML color to use as the background color for the live tile.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-tileimage'] = array(
    'label' => t('MSApplication - Tile image'),
    'description' => t('The URL to an image to use as the background for the live tile.'),
    'image' => TRUE,
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-tooltip'] = array(
    'label' => t('MSApplication - Tooltip'),
    'description' => t('Controls the text shown in the tooltip for the pinned site\'s shortcut.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  $info['tags']['msapplication-window'] = array(
    'label' => t('MSApplication - Window'),
    'description' => t('A semi-colon -separated value that controls the dimensions of the initial window. Should contain the values "width=" and "height=" to control the width and height respectively.'),
    'weight' => ++$weight,
  ) + $tag_info_defaults;

  return $info;
}
