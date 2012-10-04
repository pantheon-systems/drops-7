<?php
/**
 * @file
 * openpublish_roles.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function openpublish_roles_user_default_permissions() {
  $permissions = array();

  // Exported permission: access content overview.
  $permissions['access content overview'] = array(
    'name' => 'access content overview',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: access dashboard.
  $permissions['access dashboard'] = array(
    'name' => 'access dashboard',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'dashboard',
  );

  // Exported permission: access overlay.
  $permissions['access overlay'] = array(
    'name' => 'access overlay',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'overlay',
  );

  // Exported permission: access toolbar.
  $permissions['access toolbar'] = array(
    'name' => 'access toolbar',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'toolbar',
  );

  // Exported permission: administer menu.
  $permissions['administer menu'] = array(
    'name' => 'administer menu',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'menu',
  );

  // Exported permission: administer nodes.
  $permissions['administer nodes'] = array(
    'name' => 'administer nodes',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: administer taxonomy.
  $permissions['administer taxonomy'] = array(
    'name' => 'administer taxonomy',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: create openpublish_article content.
  $permissions['create openpublish_article content'] = array(
    'name' => 'create openpublish_article content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: create openpublish_blog_post content.
  $permissions['create openpublish_blog_post content'] = array(
    'name' => 'create openpublish_blog_post content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: create openpublish_photo_gallery content.
  $permissions['create openpublish_photo_gallery content'] = array(
    'name' => 'create openpublish_photo_gallery content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: create openpublish_site_page content.
  $permissions['create openpublish_site_page content'] = array(
    'name' => 'create openpublish_site_page content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: create openpublish_video content.
  $permissions['create openpublish_video content'] = array(
    'name' => 'create openpublish_video content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: create profile content.
  $permissions['create profile content'] = array(
    'name' => 'create profile content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: create section_front content.
  $permissions['create section_front content'] = array(
    'name' => 'create section_front content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any openpublish_article content.
  $permissions['delete any openpublish_article content'] = array(
    'name' => 'delete any openpublish_article content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any openpublish_blog_post content.
  $permissions['delete any openpublish_blog_post content'] = array(
    'name' => 'delete any openpublish_blog_post content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any openpublish_photo_gallery content.
  $permissions['delete any openpublish_photo_gallery content'] = array(
    'name' => 'delete any openpublish_photo_gallery content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any openpublish_site_page content.
  $permissions['delete any openpublish_site_page content'] = array(
    'name' => 'delete any openpublish_site_page content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any openpublish_video content.
  $permissions['delete any openpublish_video content'] = array(
    'name' => 'delete any openpublish_video content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any profile content.
  $permissions['delete any profile content'] = array(
    'name' => 'delete any profile content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete any section_front content.
  $permissions['delete any section_front content'] = array(
    'name' => 'delete any section_front content',
    'roles' => array(
      0 => 'administrator',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own openpublish_article content.
  $permissions['delete own openpublish_article content'] = array(
    'name' => 'delete own openpublish_article content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own openpublish_blog_post content.
  $permissions['delete own openpublish_blog_post content'] = array(
    'name' => 'delete own openpublish_blog_post content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own openpublish_photo_gallery content.
  $permissions['delete own openpublish_photo_gallery content'] = array(
    'name' => 'delete own openpublish_photo_gallery content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own openpublish_site_page content.
  $permissions['delete own openpublish_site_page content'] = array(
    'name' => 'delete own openpublish_site_page content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own openpublish_video content.
  $permissions['delete own openpublish_video content'] = array(
    'name' => 'delete own openpublish_video content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own profile content.
  $permissions['delete own profile content'] = array(
    'name' => 'delete own profile content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete own section_front content.
  $permissions['delete own section_front content'] = array(
    'name' => 'delete own section_front content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete revisions.
  $permissions['delete revisions'] = array(
    'name' => 'delete revisions',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: delete terms in 1.
  $permissions['delete terms in 1'] = array(
    'name' => 'delete terms in 1',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: delete terms in 2.
  $permissions['delete terms in 2'] = array(
    'name' => 'delete terms in 2',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: edit any openpublish_article content.
  $permissions['edit any openpublish_article content'] = array(
    'name' => 'edit any openpublish_article content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any openpublish_blog_post content.
  $permissions['edit any openpublish_blog_post content'] = array(
    'name' => 'edit any openpublish_blog_post content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any openpublish_photo_gallery content.
  $permissions['edit any openpublish_photo_gallery content'] = array(
    'name' => 'edit any openpublish_photo_gallery content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any openpublish_site_page content.
  $permissions['edit any openpublish_site_page content'] = array(
    'name' => 'edit any openpublish_site_page content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any openpublish_video content.
  $permissions['edit any openpublish_video content'] = array(
    'name' => 'edit any openpublish_video content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any profile content.
  $permissions['edit any profile content'] = array(
    'name' => 'edit any profile content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit any section_front content.
  $permissions['edit any section_front content'] = array(
    'name' => 'edit any section_front content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit boxes.
  $permissions['edit boxes'] = array(
    'name' => 'edit boxes',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'boxes',
  );

  // Exported permission: edit own openpublish_article content.
  $permissions['edit own openpublish_article content'] = array(
    'name' => 'edit own openpublish_article content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own openpublish_blog_post content.
  $permissions['edit own openpublish_blog_post content'] = array(
    'name' => 'edit own openpublish_blog_post content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own openpublish_photo_gallery content.
  $permissions['edit own openpublish_photo_gallery content'] = array(
    'name' => 'edit own openpublish_photo_gallery content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own openpublish_site_page content.
  $permissions['edit own openpublish_site_page content'] = array(
    'name' => 'edit own openpublish_site_page content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own openpublish_video content.
  $permissions['edit own openpublish_video content'] = array(
    'name' => 'edit own openpublish_video content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own profile content.
  $permissions['edit own profile content'] = array(
    'name' => 'edit own profile content',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit own section_front content.
  $permissions['edit own section_front content'] = array(
    'name' => 'edit own section_front content',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: edit terms in 1.
  $permissions['edit terms in 1'] = array(
    'name' => 'edit terms in 1',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: edit terms in 2.
  $permissions['edit terms in 2'] = array(
    'name' => 'edit terms in 2',
    'roles' => array(
      0 => 'administrator',
      1 => 'editor',
    ),
    'module' => 'taxonomy',
  );

  // Exported permission: revert revisions.
  $permissions['revert revisions'] = array(
    'name' => 'revert revisions',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'node',
  );

  // Exported permission: use text format full_html.
  $permissions['use text format full_html'] = array(
    'name' => 'use text format full_html',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'filter',
  );

  // Exported permission: view the administration theme.
  $permissions['view the administration theme'] = array(
    'name' => 'view the administration theme',
    'roles' => array(
      0 => 'administrator',
      1 => 'author',
      2 => 'editor',
    ),
    'module' => 'system',
  );

  return $permissions;
}
