<?php

/**
 * @file
 * Yottaa Cache include file, to be added to settings.php file.
 */

/**
 * Yottaa cache implementation.
 */
class YottaaCache implements DrupalCacheInterface {
  protected $bin;

  /**
   * The constructor.
   */
  public function __construct($bin) {
    $this->bin = $bin;
  }

  /**
   * Gets the cache.
   */
  public function get($cid) {
    return FALSE;
  }

  /**
   * Gets multiple caches.
   */
  public function getMultiple(&$cids) {
    return array();
  }

  /**
   * Sets the cache.
   */
  public function set($cid, $data, $expire = CACHE_PERMANENT) {
  }

  /**
   * Clears the cache.
   */
  public function clear($cid = NULL, $wildcard = FALSE) {
    watchdog('yottaa', 'Clear method is invoked with @cid.', array('@cid' => $cid), WATCHDOG_DEBUG);
    // Check if Yottaa module installed.
    if (!module_exists('yottaa')) {
      return;
    }

    if (class_exists('YottaaDrupalAPI')) {
      $yottaa_api = yottaa_api_drupal();

      if (empty($cid)) {
        watchdog('yottaa', 'Clear request with empty cid.', array(), WATCHDOG_DEBUG);
      }
      else {
        if ($wildcard) {
          if ($cid == '*') {
            $yottaa_api->flush();
          }
          else {
            $path = $cid . '(.*)';
            $yottaa_api->flushPaths(array(array(
              "condition" => $path,
              "name" => "URI",
              "operator" => "REGEX", "type" => "html"),
            ));
          }
        }
        elseif (is_array($cid)) {
          yottaa_expire_cache($cid);
        }
        else {
          yottaa_expire_cache(array($cid));
        }
      }
    }
  }

  /**
   * Checks if the cache is empty.
   */
  public function isEmpty() {
    return FALSE;
  }
}
