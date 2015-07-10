<?php

/**
 * @file
 * LingotekLog
 */

/**
 * Wrapper logging class for watchdog
 */
class LingotekLog {
  /*
    WATCHDOG_EMERGENCY: Emergency, system is unusable.
    WATCHDOG_ALERT: Alert, action must be taken immediately.
    WATCHDOG_CRITICAL: Critical conditions.
    WATCHDOG_ERROR: Error conditions.
    WATCHDOG_WARNING: Warning conditions.
    WATCHDOG_NOTICE: (default) Normal but significant conditions.
    WATCHDOG_INFO: Informational messages.
    WATCHDOG_DEBUG: Debug-level messages.
   */

  private static function getDefault() {
    return (LINGOTEK_DEV == TRUE);
  }

  public static function api($msg, $data, $tag = 'api') {
    if (!variable_get('lingotek_api_debug', self::getDefault())) {
      return;
    }
    self::log($msg, $data, $depth = 1, WATCHDOG_INFO, $tag);
  }

  public static function info($msg, $data, $tag = 'info') {
    if (!variable_get('lingotek_api_debug', self::getDefault())) {
      return;
    }
    self::log($msg, $data, $depth = 1, WATCHDOG_INFO, $tag);
  }

  public static function error($msg, $data, $tag = 'error') {
    if (!variable_get('lingotek_error_log', TRUE)) {
      return;
    }
    self::log($msg, $data, $depth = 1, WATCHDOG_ERROR, $tag);
  }

  public static function warning($msg, $data, $tag = 'warning') {
    if (!variable_get('lingotek_warning_log', TRUE)) {
      return;
    }
    self::log($msg, $data, $depth = 1, WATCHDOG_WARNING, $tag);
  }

  public static function trace($msg, $data = NULL, $tag = 'trace') {
    if (!variable_get('lingotek_trace_log', FALSE)) {
      return;
    }
    self::log($msg, $data, $depth = 1, WATCHDOG_DEBUG, $tag);
  }

  public static function format($obj) {
    return is_string($obj) ? $obj : '<pre>' . print_r($obj, TRUE) . '</pre>'; //htmlspecialchars(var_export($obj, TRUE))
  }

  public static function log($msg, $data = NULL, $depth = 0, $severity = WATCHDOG_NOTICE, $tag = 'general') {
    $backtrace = debug_backtrace();
    $location = $backtrace[$depth]['file'] . ':' . $backtrace[$depth]['line'];
    $function = $backtrace[$depth + 1]['function'];
    $args = @json_encode($backtrace[$depth + 1]['args']);
    if (!$data) {
      $data = array();
    }
    $data_output = json_encode($data);

    $data_array = array(
      '@tag' => $tag,
      '%location' => $location,
      '%function' => $function,
      '%args' => $args,
    );
    // format the passed-in arguments
    foreach ($data as $k => $v) {
      $data_array[$k] = LingotekLog::format($v);
    }

    watchdog('lingotek', '[@tag] <div style="word-break: break-all; padding-top: 10px; color: #666;">' . $msg . '<br /><b>FUNCTION:</b> %function<br /><b>ARGS:</b> %args<br /><b>FILE:</b> %location<br /></div>', $data_array, $severity);

    if (variable_get('lingotek_error_log', FALSE) && $tag == 'error') {
      error_log("FUNCTION: $function ARGS: $args  FILE: $location MESSAGE: $msg DATA: $data_output ");
    }
  }

}

?>
