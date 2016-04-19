<?php

/**
 * Implementation of hook_views_plugins
 */
function menu_badges_views_plugins() {
  return array(
    'display' => array(
      'menu_badge' => array(
        'title' => t('Menu badge'),
        'admin' => t('Menu badge'),
        'help' => t('Make this content available as a menu badge.'),
        'handler' => 'views_menu_badges_display_menu_badge',
        'path' => drupal_get_path('module', 'menu_badges') . '/views',
        'theme path' => drupal_get_path('module', 'menu_badges') . '/theme',
        'theme' => 'menu_badges_views_view',
        'register theme' => FALSE,
        'use ajax' => FALSE,
        'use pager' => FALSE,
        'use more' => FALSE,
        'accept attachments' => FALSE,
      ),
    ),
  );
}