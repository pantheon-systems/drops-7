<?php

// Ensure secure pages is enabled.
//$conf['securepages_enable'] = TRUE;

// Never allow updating modules through UI.
$conf['allow_authorize_operations'] = FALSE;

// Caching across all of wwwng.
$conf['cache'] = TRUE;
$conf['block_cache'] = TRUE;

// Compress cached pages always off; we use mod_deflate
$conf['page_compression'] = 0;

// Min cache lifetime 0, max 5 mins * 60 = 300 seconds.
$conf['cache_lifetime'] = 0;
$conf['page_cache_maximum_age'] = 300;

// Aggregate css and js files.
$conf['preprocess_css'] = TRUE;
$conf['preprocess_js'] = TRUE;

// Drupal doesn't cache if we invoke hooks during bootstrap.
$conf['page_cache_invoke_hooks'] = FALSE;

// Memcache and Varnish Backends.
/*
$conf['cache_backends'] = array(
  'profiles/express/modules/contrib/varnish/varnish.cache.inc',
  'profiles/express/modules/contrib/memcache/memcache.inc',
);
*/

// Setup cache_form bin.
$conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

// Set varnish as the page cache.
//$conf['cache_class_cache_page'] = 'VarnishCache';

// Set memcache as default.
//$conf['cache_default_class'] = 'MemCacheDrupal';

// Memcache bins and stampede protection.
//$conf['memcache_bins'] = array('cache' => 'default');

// Set to FALSE on Jan 5, 2012 - drastically improved performance.
/*
$conf['memcache_stampede_protection'] = FALSE;
$conf['memcache_stampede_semaphore'] = 15;
$conf['memcache_stampede_wait_time'] = 5;
$conf['memcache_stampede_wait_limit'] = 3;
*/

// Disable poorman cron.
$conf['cron_safe_threshold'] = 0;

// No IP blocking from the UI, we'll take care of that at a higher level.
$conf['blocked_ips'] = array();

// Tell Drupal about reverse proxy
//$conf['reverse_proxy'] = TRUE;
// Drupal will look for IP in $_SERVER['X-Forwarded-For']
//$conf['reverse_proxy_header'] = 'X-Forwarded-For';
// Varnish version
//$conf['varnish_version'] = 3;

//$base_url = 'http://express.local/' . $path;

# Need to do this to until we can properly support SSL.
$conf['securepages_enable'] = FALSE;
$conf['ldap_servers_require_ssl_for_credentails'] = '0';

$databases = array(
  'default' => array(
    'default' => array(
      'database' => 'drupal',
      'username' => 'root',
      'password' => '',
      'host' => 'localhost',
      'port' => '',
      'driver' => 'mysql',
      'prefix' => '',
    ),
  ),
);

// Define Varnish Server Pool
/*
$conf['reverse_proxy_addresses'] = array('localhost',);
$conf['varnish_control_terminal'] = 'localhost:6082';
$conf['varnish_control_key'] = substr(file_get_contents('/etc/varnish/secret'), 0, -1);
*/

// Memcache
//$conf['memcache_key_prefix'] = 'drupal';

// Define tmp directory
$conf['file_temporary_path'] = '/tmp';

// Turn on error reporting only for serious errors.
// Warnings were causing dumb exceptions in Behat and the messages don't
// interfere with the tests.
error_reporting(E_ERROR | E_PARSE);
