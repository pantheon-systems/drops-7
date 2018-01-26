<?php

/**
 * @file
 * A memcache based implementation of a locking mechanism.
 * See includes/lock.inc for documenation
 */

require_once dirname(__FILE__) . '/dmemcache.inc';

// Check if memcached is available - if not include default lock handler.
// @todo get rid of this conditional include as soon as this is done:
// http://drupal.org/node/1225404
$lock_file = dirname(__FILE__) . '/memcache-lock-code.inc';
$mc = dmemcache_object('semaphore');
// dmemcache_object always returns TRUE, we don't need these stats but it forces
// us to try and connect to memcache. If this fails, we can't store locks in
// memcache.
if (!$mc->getStats()) {
  $lock_file = DRUPAL_ROOT . '/includes/lock.inc';
}
require_once $lock_file;
