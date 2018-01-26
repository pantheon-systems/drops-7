<?php
/**
 * @file
 * metatag_panels_tests.pages_default.inc
 */

/**
 * Implements hook_default_page_manager_pages().
 */
function metatag_panels_tests_default_page_manager_pages() {
  $page = new stdClass();
  $page->disabled = FALSE; /* Edit this to true to make a default page disabled initially */
  $page->api_version = 1;
  $page->name = 'metatag_panels_test';
  $page->task = 'page';
  $page->admin_title = 'Metatag:Panels test';
  $page->admin_description = '';
  $page->path = 'metatag-panels-test';
  $page->access = array();
  $page->menu = array(
    'type' => 'none',
    'title' => '',
    'weight' => '0',
    'name' => 'navigation',
    'parent' => array(
      'type' => 'none',
      'title' => '',
      'weight' => 0,
      'name' => 'navigation',
    ),
  );
  $page->arguments = array();
  $page->conf = array(
    'admin_paths' => FALSE,
  );
  $page->default_handlers = array();
  $handler = new stdClass();
  $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
  $handler->api_version = 1;
  $handler->name = 'page_metatag_panels_test__default';
  $handler->task = 'page';
  $handler->subtask = 'metatag_panels_test';
  $handler->handler = 'panel_context';
  $handler->weight = 0;
  $handler->conf = array(
    'title' => 'Page',
    'no_blocks' => 0,
    'pipeline' => 'standard',
    'body_classes_to_remove' => '',
    'body_classes_to_add' => '',
    'css_id' => '',
    'css' => '',
    'contexts' => array(
      0 => array(
        'identifier' => 'User',
        'keyword' => 'user',
        'name' => 'user',
        'type' => 'select',
        'uid' => '1',
        'id' => 1,
      ),
    ),
    'relationships' => array(),
    'name' => '',
    'metatag_panels' => array(
      'enabled' => 1,
      'metatags' => array(
        'title' => array(
          'value' => 'Test page title',
        ),
        'description' => array(
          'value' => 'Test page description.',
        ),
        'keywords' => array(
          'value' => 'Test, page, keywords',
        ),
        'abstract' => array(
          'value' => 'http://www.example.com/user/%user:uid/full-bio-%281%29.pdf',
        ),
      ),
    ),
  );
  $display = new panels_display();
  $display->layout = 'onecol';
  $display->layout_settings = array();
  $display->panel_settings = array(
    'style_settings' => array(
      'default' => NULL,
      'middle' => NULL,
    ),
  );
  $display->cache = array();
  $display->title = 'Testing';
  $display->uuid = '14a1dc5e-6abd-41d5-a361-56c1c5404f32';
  $display->content = array();
  $display->panels = array();
  $pane = new stdClass();
  $pane->pid = 'new-8537d8cc-5b2d-4eac-b093-6b62f3fdc38a';
  $pane->panel = 'middle';
  $pane->type = 'custom';
  $pane->subtype = 'custom';
  $pane->shown = TRUE;
  $pane->access = array();
  $pane->configuration = array(
    'admin_title' => 'Hello',
    'title' => 'Hello there',
    'body' => 'Hi.',
    'format' => 'filtered_html',
    'substitute' => TRUE,
  );
  $pane->cache = array();
  $pane->style = array(
    'settings' => NULL,
  );
  $pane->css = array();
  $pane->extras = array();
  $pane->position = 0;
  $pane->locks = array();
  $pane->uuid = '8537d8cc-5b2d-4eac-b093-6b62f3fdc38a';
  $display->content['new-8537d8cc-5b2d-4eac-b093-6b62f3fdc38a'] = $pane;
  $display->panels['middle'][0] = 'new-8537d8cc-5b2d-4eac-b093-6b62f3fdc38a';
  $display->hide_title = PANELS_TITLE_FIXED;
  $display->title_pane = 'new-8537d8cc-5b2d-4eac-b093-6b62f3fdc38a';
  $handler->conf['display'] = $display;
  $page->default_handlers[$handler->name] = $handler;
  $pages['metatag_panels_test'] = $page;

  return $pages;
}
