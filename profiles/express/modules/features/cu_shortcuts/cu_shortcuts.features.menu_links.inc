<?php
/**
 * @file
 * cu_shortcuts.features.menu_links.inc
 */

/**
 * Implements hook_menu_default_menu_links().
 */
function cu_shortcuts_menu_default_menu_links() {
  $menu_links = array();

  // Exported menu link: shortcut-set-1_add-content:node/add
  $menu_links['shortcut-set-1_add-content:node/add'] = array(
    'menu_name' => 'shortcut-set-1',
    'link_path' => 'node/add',
    'router_path' => 'node/add',
    'link_title' => 'Add content',
    'options' => array(
      'identifier' => 'shortcut-set-1_add-content:node/add',
    ),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => -50,
    'customized' => 0,
  );
  // Exported menu link: shortcut-set-1_blocks:admin/content/blocks
  $menu_links['shortcut-set-1_blocks:admin/content/blocks'] = array(
    'menu_name' => 'shortcut-set-1',
    'link_path' => 'admin/content/blocks',
    'router_path' => 'admin/content/blocks',
    'link_title' => 'Blocks',
    'options' => array(
      'identifier' => 'shortcut-set-1_blocks:admin/content/blocks',
    ),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => -48,
    'customized' => 0,
  );
  // Exported menu link: shortcut-set-1_context:admin/structure/context
  $menu_links['shortcut-set-1_context:admin/structure/context'] = array(
    'menu_name' => 'shortcut-set-1',
    'link_path' => 'admin/structure/context',
    'router_path' => 'admin/structure/context',
    'link_title' => 'Context',
    'options' => array(
      'identifier' => 'shortcut-set-1_context:admin/structure/context',
    ),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => -46,
    'customized' => 0,
  );
  // Exported menu link: shortcut-set-1_find-content:admin/content
  $menu_links['shortcut-set-1_find-content:admin/content'] = array(
    'menu_name' => 'shortcut-set-1',
    'link_path' => 'admin/content',
    'router_path' => 'admin/content',
    'link_title' => 'Find content',
    'options' => array(
      'identifier' => 'shortcut-set-1_find-content:admin/content',
    ),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => -49,
    'customized' => 0,
  );
  // Exported menu link: shortcut-set-1_main-menu:admin/structure/menu/manage/main-menu
  $menu_links['shortcut-set-1_main-menu:admin/structure/menu/manage/main-menu'] = array(
    'menu_name' => 'shortcut-set-1',
    'link_path' => 'admin/structure/menu/manage/main-menu',
    'router_path' => 'admin/structure/menu/manage/%',
    'link_title' => 'Main Menu',
    'options' => array(
      'identifier' => 'shortcut-set-1_main-menu:admin/structure/menu/manage/main-menu',
    ),
    'module' => 'menu',
    'hidden' => 0,
    'external' => 0,
    'has_children' => 0,
    'expanded' => 0,
    'weight' => -47,
    'customized' => 0,
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Add content');
  t('Blocks');
  t('Context');
  t('Find content');
  t('Main Menu');


  return $menu_links;
}
