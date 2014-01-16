<?php

/**
 * @file
 * Provides a license type plugin.
 */

$plugin = array(
  'title' => t('Web Service'),
  'description' => t('Implements First Data Global Gateway e4 Web Service API V12.'),
  'class' => 'CommerceFirstDataGGE4ComponentWS',
  'weight' => -10,
  'hook_info' => array('ws_request_validate', 'ws_request_build_alter', 'response_process'),
);
