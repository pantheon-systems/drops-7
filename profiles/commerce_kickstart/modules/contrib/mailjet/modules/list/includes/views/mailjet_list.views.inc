<?php

/**
 * @file
 *   Adds views support for mailjet lists
 */

/**
 * Implements hook_views_plugins().
 */
function mailjet_list_views_plugins() {

  $path = drupal_get_path('module', 'mailjet_list') . '/views';

  return array(
    'module' => 'mailjet_list',
    'style' => array(
      'mailjet_list' => array(
        'title' => t('Mailjet List'),
        'path' => $path . '/plugins',
        'help' => t('Format for syncronysing Mailjet Lists.'),
        'handler' => 'views_plugin_mailjet_list',
        //  'theme' => FALSE,
        //      'theme file' => FALSE,
        //    'theme path' => FALSE,
        'uses row plugin' => FALSE,
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
        'even empty' => TRUE,
      ),
    ),
  );
}
