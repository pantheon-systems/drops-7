<?php
/**
 * @file
 * Starter page.
 */

/**
 * Page callback to display the initial help page.
 */
function metatag_importer_admin_page() {
  $output = array(
    '#type' => 'markup',
    '#markup' => t('Use the links above to import data from another module.'),
    '#prefix' => '<p>',
    '#suffix' => '<p>',
  );
  return $output;
}
