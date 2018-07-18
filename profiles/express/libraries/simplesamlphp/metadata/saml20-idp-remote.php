<?php
/**
 * SAML 2.0 remote IdP metadata for SimpleSAMLphp.
 *
 * Remember to remove the IdPs you don't use from this file.
 *
 * See: https://simplesamlphp.org/docs/stable/simplesamlphp-reference-idp-remote 
 */


/**
 * Guest IdP. allows users to sign up and register. Great for testing!
 */
$metadata['https://fedauth.colorado.edu/idp/shibboleth'] = array(
  'name' => array(
    'en' => 'Web Express Service',
  ),
  'description'          => 'Used to login users of the Web Express service for sites hosted on Web Express.',
  'SingleSignOnService'  => 'https://fedauth.colorado.edu/idp/profile/SAML2/Redirect/SSO',
  'SingleLogoutService'  => 'https://fedauth.colorado.edu/idp/profile/SAML2/Redirect/SLO',
  'certFingerprint'      => '8EC5DB22C8FD44E442A52C00B45409ECEF4610DC'
);
