<?php

/**
 * @file
 * Additional configuration to add to the Drupal settings file.
 */

// Never allow updating modules through UI.
$conf['allow_authorize_operations'] = FALSE;

// Caching across all of wwwng.
$conf['cache'] = 1;

// Compress cached pages always off; we use mod_deflate
// I'm not sure mod_deflate is on Lando or Valet.
// $conf['page_compression'] = 0;.

// Min cache lifetime 0, max 5 mins * 60 = 300 seconds.
$conf['cache_lifetime'] = 0;
$conf['page_cache_maximum_age'] = 300;

// Aggregate css and js files.
$conf['preprocess_css'] = TRUE;
$conf['preprocess_js'] = TRUE;

// Drupal doesn't cache if we invoke hooks during bootstrap.
$conf['page_cache_invoke_hooks'] = FALSE;

// Setup cache_form bin.
$conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

// Disable poorman cron.
$conf['cron_safe_threshold'] = 0;

// No IP blocking from the UI, we'll take care of that at a higher level.
$conf['blocked_ips'] = array();

// Need to do this to until we can properly support SSL.
$conf['ldap_servers_require_ssl_for_credentails'] = '0';

// Gets rid of the error on the status report page.
// To check if this is an actual problem, see if the Update module checks work.
$conf['drupal_http_request_fails'] = FALSE;

// Get rid of node grants blocking block caching.
$conf['block_cache'] = 1;
$conf['block_cache_bypass_node_grants'] = TRUE;

// Used for Laravel Valet sharing functionality.
if (isset($_SERVER['HTTP_X_FORWARDED_HOST'])) {
  $base_url = $_SERVER['HTTP_X_FORWARDED_PROTO'] . '://' . $_SERVER['HTTP_X_FORWARDED_HOST'];
}
