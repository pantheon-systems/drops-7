<?php
/**
 * @file
 * commerce_kickstart_social.oauthconnector.inc
 */

/**
 * Implements hook_default_oauthconnector_provider().
 */
function commerce_kickstart_social_default_oauthconnector_provider() {
  $export = array();

  $provider = new stdClass();
  $provider->disabled = FALSE; /* Edit this to true to make a default provider disabled initially */
  $provider->api_version = 1;
  $provider->name = 'facebook';
  $provider->title = 'Facebook';
  $provider->url = 'https://graph.facebook.com';
  $provider->consumer_advanced = array(
    'oauth2' => 1,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => '',
    'request token endpoint' => '/oauth/request_token',
    'authorization scope' => 'email',
    'authorization endpoint' => 'https://www.facebook.com/dialog/oauth',
    'access token endpoint' => '/oauth/access_token',
  );
  $provider->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'https://graph.facebook.com/me',
        'method post' => 0,
        'field' => 'id',
        'querypath' => FALSE,
        'sync_with_field' => '',
      ),
      'email' => array(
        'resource' => 'https://graph.facebook.com/me',
        'method post' => 0,
        'field' => 'email',
        'querypath' => FALSE,
        'sync_with_field' => 'mail',
      ),
      'name' => array(
        'resource' => 'https://graph.facebook.com/me',
        'method post' => 0,
        'field' => 'name',
        'querypath' => FALSE,
        'sync_with_field' => 'name',
      ),
      'avatar' => array(
        'resource' => '',
        'method post' => 0,
        'field' => '',
        'querypath' => FALSE,
        'sync_with_field' => '',
      ),
    ),
    'format' => 'json',
  );
  $export['facebook'] = $provider;

  $provider = new stdClass();
  $provider->disabled = FALSE; /* Edit this to true to make a default provider disabled initially */
  $provider->api_version = 1;
  $provider->name = 'google';
  $provider->title = 'Google';
  $provider->url = 'https://accounts.google.com';
  $provider->consumer_advanced = array(
    'oauth2' => 1,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => 'authorization_code',
    'request token endpoint' => 'none',
    'authorization scope' => 'https://www.googleapis.com/auth/userinfo.profile https://www.googleapis.com/auth/userinfo.email',
    'authorization endpoint' => '/o/oauth2/auth',
    'access token endpoint' => '/o/oauth2/token',
  );
  $provider->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'https://www.googleapis.com/oauth2/v1/userinfo',
        'method post' => 0,
        'field' => 'id',
        'querypath' => FALSE,
        'sync_with_field' => '',
      ),
      'email' => array(
        'resource' => 'https://www.googleapis.com/oauth2/v1/userinfo',
        'method post' => 0,
        'field' => 'email',
        'querypath' => FALSE,
        'sync_with_field' => 'mail',
      ),
      'name' => array(
        'resource' => 'https://www.googleapis.com/oauth2/v1/userinfo',
        'method post' => 0,
        'field' => 'name',
        'querypath' => FALSE,
        'sync_with_field' => 'name',
      ),
      'avatar' => array(
        'resource' => '',
        'method post' => 0,
        'field' => 'picture',
        'querypath' => FALSE,
        'sync_with_field' => '',
      ),
    ),
    'format' => 'json',
  );
  $export['google'] = $provider;

  $provider = new stdClass();
  $provider->disabled = FALSE; /* Edit this to true to make a default provider disabled initially */
  $provider->api_version = 1;
  $provider->name = 'twitter';
  $provider->title = 'Twitter';
  $provider->url = 'https://api.twitter.com';
  $provider->consumer_advanced = array(
    'oauth2' => 0,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => '',
    'request token endpoint' => '/oauth/request_token',
    'authorization scope' => '',
    'authorization endpoint' => '/oauth/authorize',
    'access token endpoint' => '/oauth/access_token',
  );
  $provider->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'https://api.twitter.com/1.1/account/verify_credentials.json',
        'method post' => 0,
        'field' => 'id',
        'querypath' => FALSE,
        'sync_with_field' => '',
      ),
      'name' => array(
        'resource' => 'https://api.twitter.com/1.1/account/verify_credentials.json',
        'method post' => 0,
        'field' => 'name',
        'querypath' => FALSE,
        'sync_with_field' => 'name',
      ),
      'avatar' => array(
        'resource' => 'https://api.twitter.com/1.1/account/verify_credentials.json',
        'method post' => 0,
        'field' => 'profile_image_url',
        'querypath' => FALSE,
        'sync_with_field' => '',
      ),
    ),
    'format' => 'json',
  );
  $export['twitter'] = $provider;

  return $export;
}
