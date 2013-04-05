<?php

/**
 * @file
 * Lingotek Module Constants.
 */
/*
 * def - only defines the constant if it has not been defined previously
 */
function def($key, $val) {
    if (!defined($key)) {
        define($key, $val);
    }
}


/**
 * Local Override - a local override config file may be used by either: 
 * (1) setting the environment var 'LINGOTEK_CONFIG_PATH' to a file, or by
 * (2) having a "config.php" file in your lingotek module folder 
 */
$local_env_config_filepath = getenv('LINGOTEK_CONFIG_PATH');
$local_config_file = $local_env_config_filepath !== FALSE ? $local_env_config_filepath : getcwd() . '/' . drupal_get_path('module', 'lingotek') . '/config.php';
$use_local_override = file_exists($local_config_file);
if ($use_local_override) {
  include_once $local_config_file;
}

/**
 *  Config
 */
def('LINGOTEK_GMC_SERVER', 'https://gmc.lingotek.com');
def('LINGOTEK_API_SERVER', 'https://myaccount.lingotek.com');
def('LINGOTEK_BILLING_SERVER', 'https://billing-service.lingotek.com/billing/account.json');
def('LINGOTEK_AP_OAUTH_KEY', 'd944c2ae-b66e-4322-b37e-40ba0a495eb7');
def('LINGOTEK_AP_OAUTH_SECRET', 'e4ae98ca-835b-4d9f-8faf-116ce9c69424');
def('LINGOTEK_DEV', FALSE);

/**
 * Identifies a content type which has Lingotek translation enabled.
 */
define('LINGOTEK_ENABLED', 'lingotek');



/**
 * Defines the local path URLs
 */
define('LINGOTEK_BASE_URL', 'admin/config/regional/lingotek');
define('LINGOTEK_NOTIFICATIONS_URL', 'lingotek/notify'); // notification callback URL