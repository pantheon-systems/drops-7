<?php

/**
 * @file
 * Page callbacks for adding, editing, deleting, and revisions management for content.
 */

/**
 * Provides a listing of all embed types you can add.
 *
 * @return string
 *   Markup for list of embed types.
 */
function et_add_page() {
  $item = menu_get_item();
  $content = system_admin_menu_block($item);

  // Bypass the listing if only one content type is available.
  if (count($content) == 1) {
    $item = array_shift($content);
    drupal_goto($item['href']);
  }
  $output = theme('node_add_list', array('content' => $content));

  // Need to check and see if output is empty so we can change the help message sent to the user.
  if (is_string($output) && strpos($output, 'You have not created any content types yet') != FALSE) {
    $output = '<p>You have not created any Embed Template types yet. Please refer to the module\'s readme file or <a href="https://www.drupal.org/project/embed_templates">instructions the module listing page</a> in order to create a template type.</p>';
  }
  return $output;
}
