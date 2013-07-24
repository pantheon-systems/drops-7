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
  private static function getDefault(){
    return (LINGOTEK_DEV == TRUE);
  }

  public static function info($msg, $data, $tag = 'info') {
    self::log($msg, $data, $depth = 1, WATCHDOG_INFO, $tag);
  }

  public static function error($msg, $data, $tag = 'error') {
    self::log($msg, $data, $depth = 1, WATCHDOG_ERROR, $tag);
  }

  public static function warning($msg, $data, $tag = 'warning') {
    self::log($msg, $data, $depth = 1, WATCHDOG_WARNING, $tag);
  }

  public static function trace($msg, $data = NULL, $tag = 'trace') {
    if (!variable_get('lingotek_trace_log', FALSE)) {
      return;
    }
    self::log($msg, $data, $depth = 1, WATCHDOG_INFO, $tag);
  }

  public static function format($obj) {
    return is_string($obj)? $obj : '<pre>' . print_r($obj, TRUE) . '</pre>'; //htmlspecialchars(var_export($obj, TRUE))
  }

  public static function log($msg, $data = NULL, $depth = 0, $severity = WATCHDOG_NOTICE, $tag = '') {
    if ($severity == WATCHDOG_WARNING && variable_get('lingotek_warning_log', self::getDefault())) {
      return;
    }
    else if (strcasecmp($tag, 'api') == 0 && !variable_get('lingotek_api_debug', self::getDefault())) {
      return;
    }

    $backtrace = debug_backtrace();
    $location = $backtrace[$depth]['file'] . ':' . $backtrace[$depth]['line'];
    $function = $backtrace[$depth + 1]['function'];
    $args = @json_encode($backtrace[$depth + 1]['args']);

    $data_output = "";
    if (isset($data)) {
      $data_output = json_encode($data);
    }
    $suffix = is_string($tag) && strlen($tag) ? ' - ' . $tag : '';
    $data_array = array();
    if(is_array($data)){
      foreach($data as $k=>$v){
        $data_array[$k] = LingotekLog::format($v);
      }
    }
    
    watchdog
        ('lingotek' . $suffix, t($msg, $data_array) . ' <div style="word-break: break-all; padding-top: 10px; color: #666;"><b>FUNCTION:</b> %function<br /><b>ARGS:</b> %args<br /><b>FILE:</b> %location<br /><b>MESSAGE:</b> %msg <br /><b>DATA:</b> %data <br /></div>', array(
      '%msg' => $msg,
      '%data' => $data_output,
      '%location' => $location,
      '%function' => $function,
      '%args' => $args,
        ), $severity
    );

    if (variable_get('lingotek_error_log', FALSE) && $tag == 'error') {
      error_log("FUNCTION: $function ARGS: $args  FILE: $location MESSAGE: $msg DATA: $data_output ");
    }
  }

}

?>