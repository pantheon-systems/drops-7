<?php

/**
 * @file
 * Yottaa Drupal API Class.
 */

class YottaaDrupalAPI extends YottaaAPI {

  public $apiKeyVarname = 'yottaa_api_key';
  public $userIdVarname = 'yottaa_user_id';
  public $siteIdVarname = 'yottaa_site_id';
  public $autoClearCacheVarname = 'yottaa_auto_clear_cache';
  public $enableLoggingVarname = 'yottaa_enable_logging';

  /**
   * Constructs the object.
   */
  public function __construct() {
    $key = variable_get($this->apiKeyVarname, '');
    $uid = variable_get($this->userIdVarname, '');
    $sid = variable_get($this->siteIdVarname, '');
    parent::__construct($key, $uid, $sid);
  }

  /**
   * Gets variables from the database.
   *
   * @return array
   *   An associative array with yottaa api settigs.
   */
  public function getParameters() {
    return array(
      "api_key" => variable_get($this->apiKeyVarname, ""),
      "user_id" => variable_get($this->userIdVarname, ""),
      "site_id" => variable_get($this->siteIdVarname, ""),
    );
  }

  /**
   * Updates the variables in the database.
   *
   * @param string $key
   *   Api key.
   * @param string $uid
   *   Api user id.
   * @param string $sid
   *   Site id.
   *
   * @return void
   *   Nothing to return.
   */
  public function updateParameters($key, $uid, $sid) {
    variable_set($this->userIdVarname, $uid);
    variable_set($this->apiKeyVarname, $key);
    variable_set($this->siteIdVarname, $sid);
    parent::updateParameters($key, $uid, $sid);
  }

  /**
   * Deletes the variables from the database.
   *
   * @return void
   *   Nothing to return.
   */
  public function deleteParameters() {
    variable_del($this->userIdVarname);
    variable_del($this->apiKeyVarname);
    variable_del($this->siteIdVarname);
    parent::deleteParameters();
  }

  /**
   * Checks if "auto clear cache" is enabled.
   *
   * @return int
   *   Returns 0 or 1.
   */
  public function getAutoClearCacheParameter() {
    return variable_get($this->autoClearCacheVarname, 1);
  }

  /**
   * Sets "auto clear cache" parameter.
   * 
   * @param int $enabled
   *   A value to set.
   *
   * @return void
   *   Nothing to return.
   */
  public function setAutoClearCacheParameter($enabled) {
    variable_set($this->autoClearCacheVarname, $enabled);
  }

  /**
   * Checks if "enable logging" is enabled.
   *
   * @return int
   *   Returns 0 or 1.
   */
  public function getEnableLoggingParameter() {
    return variable_get($this->enableLoggingVarname, 1);
  }

  /**
   * Sets "enable logging" parameter.
   *
   * @param int $enabled
   *   A value to set.
   *
   * @return void
   *   Nothing to return.
   */
  public function setEnableLoggingParameter($enabled) {
    variable_set($this->enableLoggingVarname, $enabled);
  }

  /**
   * Post-process settings return from Yottaa service.
   *
   * @param array $json_output
   *   An array.
   *
   * @return array
   *   An array of settings.
   */
  protected function postProcessingSettings($json_output) {
    if (!isset($json_output["error"])) {

      $full_pages_key = "(.*)";
      $configure_pages_key1 = "admin/";
      $configure_pages_key2 = "admin%252F";
      $edit_pages_key = "/edit";

      $site_pages_caching = 'unknown';
      $only_cache_anonymous_users = 'unknown';
      $edit_pages_caching = 'unknown';
      $configure_pages_caching = 'unknown';
      $configure_pages_caching1 = 'unknown';
      $configure_pages_caching2 = 'unknown';

      $exclusions = '';

      $excluded_sess_cookie = 'unknown';

      if (isset($json_output["defaultActions"]) && isset($json_output["defaultActions"]["resourceActions"]) && isset($json_output["defaultActions"]["resourceActions"]["htmlCache"])) {
        $html_cachings = $json_output["defaultActions"]["resourceActions"]["htmlCache"];
        foreach ($html_cachings as &$html_caching) {
          if ($html_caching["enabled"]) {
            $site_pages_caching = 'included';
          }
          if (isset($html_caching["filters"])) {
            $filters = $html_caching["filters"];
            foreach ($filters as &$filter) {
              if (isset($filter["match"])) {
                $direction = $filter["direction"] == 1 ? "included" : "excluded";
                $matches = $filter["match"];
                foreach ($matches as &$match) {
                  if (isset($match["condition"])) {
                    if ($match["condition"] == $full_pages_key && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "REGEX") {
                      $only_cache_anonymous_users = $direction;
                    }
                    if ($match["name"] == "Cookie" && $match["condition"] == "SESS" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $excluded_sess_cookie = "set";
                    }
                    if ($match["condition"] == $edit_pages_key && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $edit_pages_caching = $direction;
                    }
                    if ($match["condition"] == $configure_pages_key1 && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $configure_pages_caching1 = $direction;
                    }
                    if ($match["condition"] == $configure_pages_key2 && $match["name"] == "URI" && $match["type"] == "0" && $match["operator"] == "CONTAIN") {
                      $configure_pages_caching2 = $direction;
                    }
                  }
                }
                if ($configure_pages_caching1 == "excluded" && $configure_pages_caching2 == "excluded") {
                  $configure_pages_caching = "excluded";
                }
                if ($only_cache_anonymous_users == "unknown" || $excluded_sess_cookie != "set") {
                  $only_cache_anonymous_users = "unknown";
                  $excluded_sess_cookie = "unknown";
                }
              }
            }
          }
        }
      }

      if (isset($json_output["defaultActions"]) && isset($json_output["defaultActions"]["filters"])) {
        $filters = $json_output["defaultActions"]["filters"];
        foreach ($filters as &$filter) {
          if (isset($filter["match"])) {
            if ($filter["direction"] == 0) {
              $matches = $filter["match"];
              foreach ($matches as &$match) {
                if (isset($match["condition"])) {
                  if ($exclusions != '') {
                    $exclusions = $exclusions . ' ; ';
                  }
                  $exclusions = $exclusions . $match["condition"];
                }
              }
            }
          }
        }
      }

      return array(
        'site_pages_caching' => $site_pages_caching,
        'edit_pages_caching' => $edit_pages_caching,
        'configure_pages_caching' => $configure_pages_caching,
        'only_cache_anonymous_users' => $only_cache_anonymous_users,
        'exclusions' => $exclusions,
      );
    }
    else {
      return $json_output;
    }
  }

  /**
   * Logs a message.
   *
   * @param string $message
   *   A text message to log.
   *
   * @return void
   *   Nothing to return.
   */
  public function log($message) {
    if ($this->getEnableLoggingParameter() == 1) {
      if (is_array($message) || is_object($message)) {
        watchdog('yottaa', print_r($message, TRUE));
      }
      else {
        watchdog('yottaa', $message);
      }
    }
  }
}
/**
 * Wrapper function for Yottaa Drupal API Class.
 */
function yottaa_api_drupal() {
  static $api;
  if (!isset($api)) {
    $api = new YottaaDrupalAPI();
  }
  return $api;
}
