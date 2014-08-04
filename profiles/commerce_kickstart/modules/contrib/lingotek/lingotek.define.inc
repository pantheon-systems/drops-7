<?php

/**
 * @file
 * Lingotek Translation module Constants.
 */
/*
 * lingotek_define - only defines the constant if it has not been defined previously (in local override file)
 */
function lingotek_define($key, $val) {
  if (!defined($key)) {
    define($key, $val);
  }
}

/**
 * Local Override - a local override configuration may be used by: 
 * (1) inserting variable overrides into the Drupal "sites/default/settings.php" file (recommended)
 * (2) inserting variable into a "config.php" file in The Lingotek Translation module folder 
 * (3) setting the environment var 'LINGOTEK_CONFIG_PATH' to a file path elsewhere on your system
 */
$local_env_config_filepath = function_exists('getenv') ? getenv('LINGOTEK_CONFIG_PATH') : (array_key_exists('LINGOTEK_CONFIG_PATH', $_SERVER) ? $_SERVER['LINGOTEK_CONFIG_PATH'] : FALSE);
$local_config_file = $local_env_config_filepath !== FALSE ? $local_env_config_filepath : getcwd() . '/' . drupal_get_path('module', 'lingotek') . '/config.php';
$use_local_override = file_exists($local_config_file);
if ($use_local_override) {
  include_once $local_config_file;
}

/**
 *  Config 
 * (Note: if any of the following variables 'def' variables are set prior to this, then those settings will take precedence rather than these.)
 */
if (variable_get('lingotek_use_stage_servers', FALSE)) {
  // Stage Servers
  lingotek_define('LINGOTEK_GMC_SERVER', 'https://gmc.lingotek.com/cms');
  lingotek_define('LINGOTEK_API_SERVER', 'http://cms.lingotek.com');
  lingotek_define('LINGOTEK_AP_OAUTH_KEY', 'd944c2ae-b66e-4322-b37e-40ba0a495eb7');
  lingotek_define('LINGOTEK_AP_OAUTH_SECRET', 'e4ae98ca-835b-4d9f-8faf-116ce9c69424');
}
else {
  // Production Servers
  lingotek_define('LINGOTEK_GMC_SERVER', 'https://gmc.lingotek.com');
  lingotek_define('LINGOTEK_API_SERVER', 'https://myaccount.lingotek.com');
  lingotek_define('LINGOTEK_AP_OAUTH_KEY', 'd944c2ae-b66e-4322-b37e-40ba0a495eb7');
  lingotek_define('LINGOTEK_AP_OAUTH_SECRET', 'e4ae98ca-835b-4d9f-8faf-116ce9c69424');
}

lingotek_define('LINGOTEK_DEV', variable_get('lingotek_dev', FALSE));

/**
 * Defines the path URLs
 */
define('LINGOTEK_MENU_LANG_BASE_URL', 'admin/config/regional/lingotek');
define('LINGOTEK_MENU_MAIN_BASE_URL', 'admin/settings/lingotek'); //admin/config/lingotek
define('LINGOTEK_NOTIFY_URL', 'lingotek/notify'); // notification callback URL

/**
 * Defines the config documents' segment size in rows, config strings' maxlength, Localize magic number for taxonomies
 */
define('LINGOTEK_CONFIG_CHUNK_SIZE', 200);
define('LINGOTEK_CONFIG_MAX_SOURCE_LENGTH', 4096);
define('LINGOTEK_I18N_ENABLED_VALUE', 1);
define('LINGOTEK_TAXONOMY_LOCALIZE_VALUE', 1);
define('LINGOTEK_MENU_LOCALIZE_TRANSLATE_VALUE', 5);

/**
 * Defines STDIN, STDOUT, STDERR in case they're not already defined by PHP
 */
lingotek_define('STDIN', fopen('php://stdin', 'r'));
lingotek_define('STDOUT', fopen('php://stdout', 'w'));
lingotek_define('STDERR', fopen('php://stderr', 'w'));

/**
 * Define file-processing timeout
 */
lingotek_define('LINGOTEK_FILE_PROCESS_TIMEOUT', 120);
lingotek_define('LINGOTEK_DEFAULT_WORKFLOW_TEMPLATE', 2);
