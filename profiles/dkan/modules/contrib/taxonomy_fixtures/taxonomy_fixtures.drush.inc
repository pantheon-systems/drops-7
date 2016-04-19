<?php
/**
 * @file
 * Drush commands for taxonomy fixtures.
 */

/**
 * Implements hook_drush_command().
 */
function taxonomy_fixtures_drush_command() {
  $correct_args = array('import', 'export', 'delete');
  $correct_args = implode(', ', $correct_args);
  return array(
    'taxonomy-fixtures' => array(
      'description' => 'LTAR Collections drush commands',
      'aliases' => array('tf'),
      'arguments' => array(
        'operation' => 'The type of operation (' . $correct_args . ').',
        'vocabulary' => 'The set of vocabularies to be processed',
      ),

    ),
  );
}

/**
 * Implements drush_command().
 */
function drush_taxonomy_fixtures($operation = 'import') {
  $correct_args = array('import', 'export', 'delete');
  // Validate operation.
  if (!in_array($operation, $correct_args)) {
    $correct_args = implode(', ', $correct_args);
    $error_message = dt(
      '"@operation" is not a valid operation. Please choose between @options.',
      array(
        '@operation' => $operation,
        '@options' => $correct_args,
      )
    );
    return drush_set_error($error_message);
  }
  // Validate vocabularies to be exported (if any).
  $op_vocabularies = drush_get_arguments();
  array_shift($op_vocabularies);
  array_shift($op_vocabularies);
  $subscribed_vocabularies = taxonomy_fixtures_get_vocabularies();
  $vocabularies = array();
  if (count($op_vocabularies) > 0) {
    foreach ($op_vocabularies as $vocabulary) {
      if (!in_array($vocabulary, array_keys($subscribed_vocabularies))) {
        $error_message = dt(
          '"@vocabulary" does not implements hook_taxonomy_fixtures_vocabulary().',
          array(
            '@vocabulary' => $vocabulary,
          )
        );
        return drush_set_error($error_message);
      }
      $vocabularies[$vocabulary] = $subscribed_vocabularies[$vocabulary];
    }
  }
  else {
    $vocabularies = $subscribed_vocabularies;
  }
  // Run the operation with arguments (if any).
  $function = 'taxonomy_fixtures_' . $operation . '_terms';
  $function($vocabularies);
}
