<?php

/**
 * @file
 * Provides a license type plugin.
 */

$plugin = array(
  'title' => t('Hosted Payment Pages'),
  'description' => t('Implements First Data Global Gateway e4 Hosted Payment Pages.'),
  'class' => 'CommerceFirstDataGGE4ComponentHPP',
  'weight' => 5,
  'hook_info' => array('hpp_post_data_alter', 'hpp_payment_form_alter', 'response_process'),
);
