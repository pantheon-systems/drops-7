<?php
/**
 * @file
 * Commerce Checkout Progress - Admin functions.
 */

/**
 * Overriding the default text area and turning it into checkboxes.
 * The text area is on the block configuration page where a user says what pages
 * should be applied to the visibility settings. (show on, don't show on, etc)
 * We only want to give site admins the ability to enable on checkout/* pages.
 */
function _commerce_checkout_progress_form_block_admin_configure_alter(&$form, $form_state, $form_id) {
  // Visibility is the text area where site admins type any path they want the block visible on.
  $path = &$form['visibility']['path'];
  // Hide and make unalterable.
  $path['visibility']['#type'] = 'value';
  // Show on only the listed pages.
  $path['visibility']['#value'] = 1; // this gets replaced later

  // New field of checkboxes that has valid and possible checkout pages to display the block on.
  $path['pages']['#title'] = t('Show block on specific pages');
  $path['pages']['#description'] = t('Select the pages on which you want to show the checkout
    progress indication. Leaving all checkboxes unselected will show the progress on all checkout
    pages.');
  $path['pages']['#type'] = 'checkboxes';

  $options = array();
  // Check if the cart module is enabled, and add it as an option if it is
  if (module_exists('commerce_cart')) {
    $options['cart'] = t('Cart');
  }

  // Grab possibilities from commerce.
  $pages = commerce_checkout_pages();
  foreach ($pages as $page_id => $page) {
    $options[$page_id] = t($page['title']);
  }

  $path['pages']['#options'] = $options;
  $path['pages']['#default_value'] = variable_get('commerce_checkout_progress_block_pages', array());
  // Add our custom submit handler so that we can process the 'pages' array into text.
  array_unshift($form['#submit'], 'commerce_checkout_progress_form_block_admin_configure_submit');
}

/**
 * Convert array to text before saving to database.
 */
function commerce_checkout_progress_form_block_admin_configure_submit($form, &$form_state) {
  if (is_array($form_state['values']['pages'])) {
    $pages = array();
    // Setting the block 'visiblity' text area based on 'pages' array.
    foreach ($form_state['values']['pages'] as $path => $checked) {
      if ($checked) {
        $pages[] = $path;
      }
    }
    variable_set('commerce_checkout_progress_block_pages', $pages);
  }
  // The block view callback should be always called on checkout pages.
  $form_state['values']['pages'] = "checkout/*\ncart";
}
