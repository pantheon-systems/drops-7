<?php

/**
 * @file
 * A memcache based implementation of a locking mechanism.
 * See includes/lock.inc for documenation
 */

// Set up a define to make the code more readable, so we know we're setting a
// value for our lock and not simply passing dmemcache a flag.
define('LOCK_VALUE', TRUE);


/**
 * Initialize the locking system.
 */
function lock_initialize() {
  global $locks;

  $locks = array();
}

/**
 * Acquire (or renew) a lock, but do not block if it fails.
 *
 * @param $name
 *   The name of the lock.
 * @param $timeout
 *   A number of seconds (int) before the lock expires (minimum of 1).
 * @return
 *   TRUE if the lock was acquired, FALSE if it failed.
 */
function lock_acquire($name, $timeout = 30) {
  global $locks;

  // Ensure that the timeout is at least 1 sec. This is a limitation
  // imposed by memcached.
  $timeout = max($timeout, 1);
  $now = microtime(TRUE);
  $expire = $now + $timeout;

  $result = dmemcache_get($name, 'semaphore');
  if ($result && isset($locks[$name]) && $locks[$name] > $now) {
    // Only renew the lock if we already set it and it has not expired.
    if (dmemcache_set($name, LOCK_VALUE, $timeout, 'semaphore')) {
      $locks[$name] = $expire;
    }
  }
  else if (dmemcache_add($name, LOCK_VALUE, $timeout, 'semaphore')) {
    $locks[$name] = $expire;
  }
  else {
    // Failed to acquire the lock.  Unset the key from the $locks array even if
    // not set, PHP 5+ allows this without error or warning.
    unset($locks[$name]);
  }

  return isset($locks[$name]);
}

/**
 * Check if lock acquired by a different process may be available.
 *
 * If an existing lock has expired, it is removed.
 *
 * @param $name
 *   The name of the lock.
 * @return
 *   TRUE if there is no lock or it was removed, FALSE otherwise.
 */
function lock_may_be_available($name) {
  return !dmemcache_get($name, 'semaphore');
}

/**
 * Wait for a lock to be available.
 *
 * This function may be called in a request that fails to acquire a desired
 * lock. This will block further execution until the lock is available or the
 * specified delay in seconds is reached. This should not be used with locks
 * that are acquired very frequently, since the lock is likely to be acquired
 * again by a different request during the sleep().
 *
 * @param $name
 *   The name of the lock.
 * @param $delay
 *   The maximum number of seconds to wait, as an integer.
 * @return
 *   TRUE if the lock holds, FALSE if it is available.
 */
function lock_wait($name, $delay = 30) {
  $delay = (int) $delay;
  while ($delay--) {
    // This function should only be called by a request that failed to get a
    // lock, so we sleep first to give the parallel request a chance to finish
    // and release the lock.
    sleep(1);
    if (!dmemcache_get($name, 'semaphore')) {
      // No longer need to wait.
      return FALSE;
    }
  }
  // The caller must still wait longer to get the lock.
  return TRUE;
}

/**
 * Release a lock previously acquired by lock_acquire().
 *
 * This will release the named lock if it is still held by the current request.
 *
 * @param $name
 *   The name of the lock.
 */
function lock_release($name) {
  global $locks;

  dmemcache_delete($name, 'semaphore');
  // We unset unconditionally since caller assumes lock is released anyway.
  unset($locks[$name]);
}

