<?php

/**
 * @file
 * Defines LingotekProfileManager
 */

/**
 * A class wrapper for Lingotek Profiles
 */
class LingotekProfileManager {

  protected static $profiles;

  public static function getProfiles() {
    if (!empty(self::$profiles)) {
      return self::$profiles;
    }
    self::$profiles = variable_get('lingotek_profiles', array());

    if (empty($profiles)) {
      self::$profiles[] = array(
        'name' => 'Automatic',
        'auto_upload' => 1,
        'auto_download' => 1,
      );
      self::$profiles[] = array(
        'name' => 'Manual',
        'auto_upload' => 0,
        'auto_download' => 0,
      );
      variable_set('lingotek_profiles', self::$profiles);
    }
    return self::$profiles;
  }

}
