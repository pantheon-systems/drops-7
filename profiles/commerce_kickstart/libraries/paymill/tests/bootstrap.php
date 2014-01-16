<?php

include_once('../vendor/autoload.php');

/**
 * $apiHost should be set to default unit test host for external users
 * can be overriden with environment variable PAYMILL_TEST_API_HOST
 */
if (!defined('API_HOST') && getenv('PAYMILL_TEST_API_HOST'))
    define('API_HOST', getenv('PAYMILL_TEST_API_HOST'));

defined('API_HOST')
    || define('API_HOST', 'https://api.paymill.com/v2/');

/**
 * $apiKey should be set to api test key
 * can be overriden with environment variable API_TEST_KEY
 */
if (!defined('API_TEST_KEY') && getenv('API_TEST_KEY'))
    define('API_TEST_KEY', getenv('API_TEST_KEY'));

define('TOKEN', '098f6bcd4621d373cade4e832627b4f6');
