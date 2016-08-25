<?php
/**
 * @file
 * Metatag integration for the Metatag: Verification module.
 */

/**
 * Implements hook_metatag_info().
 */
function metatag_verification_metatag_info() {
  $info['groups']['verification'] = array(
    'label' => t('Site verification'),
    'description' => t('These meta tags are used to confirm site ownership with search engines and other services.'),
    'form' => array(
      '#weight' => 110,
    ),
  );

  // Stack the verification codes after most others.
  $weight = 100;

  // Defaults used for all meta tags.
  $defaults = array(
    'class' => 'DrupalTextMetaTag',
    'context' => array('global'),
    'group' => 'verification',
  );

  $info['tags']['alexaVerifyID'] = array(
    'label' => t('Alexa'),
    'description' => t('A string provided by <a href="@alexa">Alexa</a>, which can be obtained from the <a href="@verify_url">Alexa "Claim Your Site" page</a>.', array('@alexa' => 'http://www.alexa.com/', '@verify_url' => 'http://www.alexa.com/siteowners/claim')),
    'weight' => ++$weight,
  ) + $defaults;

  $info['tags']['msvalidate.01'] = array(
    'label' => t('Bing'),
    'description' => t('A string provided by <a href="@bing">Bing</a>, full details are available from the <a href="@verify_url">Bing online help</a>.', array('@bing' => 'http://www.bing.com/', '@verify_url' => 'http://www.bing.com/webmaster/help/how-to-verify-ownership-of-your-site-afcfefc6')),
    'weight' => ++$weight,
  ) + $defaults;

  $info['tags']['baidu-site-verification'] = array(
    'label' => t('Baidu'),
    'description' => t('A string provided by <a href="@baidu">Baidu</a>.', array('@baidu' => 'http://www.baidu.com/')),
    'weight' => ++$weight,
  ) + $defaults;

  $info['tags']['google-site-verification'] = array(
    'label' => t('Google'),
    'description' => t('A string provided by <a href="@google">Google</a>, full details are available from the <a href="@verify_url">Google online help</a>.', array('@google' => 'https://www.google.com/', '@verify_url' => 'https://support.google.com/webmasters/answer/35179?hl=en')),
    'weight' => ++$weight,
  ) + $defaults;

  $info['tags']['norton-safeweb-site-verification'] = array(
    'label' => t('Norton Safe Web'),
    'description' => t('A string provided by <a href="@norton">Norton Safe Web</a>, full details are available from the <a href="@verify_url">Norton Safe Web online help</a>.', array('@norton' => 'https://safeweb.norton.com/', '@verify_url' => 'https://safeweb.norton.com/help/site_owners')),
    'weight' => ++$weight,
  ) + $defaults;

  $info['tags']['p:domain_verify'] = array(
    'label' => t('Pinterest'),
    'description' => t('A string provided by <a href="@pinterest">Pinterest</a>, full details are available from the <a href="@verify_url">Pinterest online help</a>.', array('@pinterest' => 'https://www.pinterest.com/', '@verify_url' => 'https://help.pinterest.com/en/articles/verify-your-website')),
    'weight' => ++$weight,
  ) + $defaults;

  $info['tags']['y_key'] = array(
    'label' => t('Yahoo'),
    'description' => t('A string provided by <a href="@yahoo">Yahoo</a>.', array('@yahoo' => 'http://www.yahoo.com/')),
    'weight' => ++$weight,
  ) + $defaults;

  $info['tags']['yandex-verification'] = array(
    'label' => t('Yandex'),
    'description' => t('A string provided by <a href="@yandex">Yandex</a>, full details are available from the <a href="@verify_url">Yandex online help</a>.', array('@yandex' => 'http://www.yandex.com/', '@verify_url' => 'http://api.yandex.com/webmaster/doc/dg/reference/hosts-type.xml')),
    'weight' => ++$weight,
  ) + $defaults;

  return $info;
}
