<?php
/**
 * @file
 * Provide an accordion style plugin for Views. This file is autoloaded by views.
 */

/**
 * Implements hook_views_plugin().
 */
function views_accordion_views_plugins() {
  return array(
    'style' => array(
      'views_accordion' => array(
        'title' => t('Views Accordion'),
        'theme' => 'views_view_accordion',
        'help' => t('Display a JQuery accordion with the results. The first field will be used as the header and trigger.'),
        'handler' => 'views_accordion_style_plugin',
        'uses row plugin' => TRUE,
        'uses row class' => TRUE,
        'uses options' => TRUE,
        'uses fields' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
}
