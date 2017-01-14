<?php

/**
 * @file
 * Drush command file
 */

/**
 * Implements hook_drush_help().
 */
function lingotek_drush_help($command) {
  switch ($command) {
    case 'drush:lt-push':
      return dt('Push entities to Lingotek TMS');
    case 'drush:lt-cleanup':
      return dt('Run the Lingotek field language cleanup utility');
    case 'drush:lt-prepare-blocks':
      return dt('Run the Lingotek prepare blocks utility');
    case 'drush:lt-prepare-taxonomies':
      return dt('Run the Lingotek prepare vocabularies utility');
    case 'drush:lt-prepare-menus':
      return dt('Run the Lingotek prepare menus utility');
    // Commented out: error reported /home/{user}/.composer/vendor/drush/drush/commands/core/drupal/batch.inc on line 45 and 137
    // case 'drush:lt-prepare-builtins':
    //   return dt('Run the Lingotek prepare built-ins utility');
  }
}

/**
 * Implements hook_drush_command().
 */
function lingotek_drush_command() {
  $items = array();
  $items['lt-push'] = array(
    'command-hook' => 'push',
    'description' => dt('Push all the instances of an entity up to Lingotek TMS'),
    'arguments' => array(
      'entity' => 'The entity type to be pushed (config, node, etc.)',
    ),
    'required-arguments' => TRUE,
    'options' => array(
      'bundles' => 'Comma delimited list of bundles (e.g. article, blog)',
    ),
    'examples' => array(
      'drush lt-push config' => 'Upload all checked config to the TMS',
      'drush lt-push node --bundles=article,blog' => 'upload nodes of type article and blog to the TMS',
    ),
  );
  $items['lt-cleanup'] = array(
    'command-hook' => 'cleanup',
    'description' => dt('Run the Lingotek field language cleanup utility'),
    'arguments' => array(
      'entity' => 'The entity type to process (taxonomy_term, node, etc.)',
    ),
    'required-arguments' => TRUE,
    'examples' => array(
      'drush lt-cleanup node' => 'Cleanup language field settings on nodes',
    ),
  );
  $items['lt-prepare-blocks'] = array(
    'command-hook' => 'prepare_blocks',
    'callback arguments' => array(
      array(
        'blocks' => 'blocks',
      ),
      'lingotek_translate_config_blocks',
    ),
    'description' => dt('Prepare blocks for Lingotek translation.'),
    'examples' => array(
      'drush lt-prepare-blocks' => '',
    ),
  );
  $items['lt-prepare-taxonomies'] = array(
    'command-hook' => 'prepare_vocab',
    'callback arguments' => array(
      array(
        'taxonomy' => 'taxonomy',
      ),
      'lingotek_translate_config_taxonomies',
    ),
    'description' => dt('Prepare vocabularies for Lingotek translation.'),
    'examples' => array(
      'drush lt-prepare-taxonomies' => '',
    ),
  );
  $items['lt-prepare-menus'] = array(
    'command-hook' => 'prepare_menus',
    'callback arguments' => array(
      array(
        'menu' => 'menu',
      ),
      'lingotek_translate_config_menus',
    ),
    'description' => dt('Prepare menus for Lingotek translation.'),
    'examples' => array(
      'drush lt-prepare-menus' => '',
    ),
  );
  // Commented out: error reported /home/{user}/.composer/vendor/drush/drush/commands/core/drupal/batch.inc on line 45 and 137
  // $items['lt-prepare-builtins'] = array(
  //   'command-hook' => 'prepare_builtins',
  //   'description' => dt('Prepare built-in strings for Lingotek translation.'),
  //   'examples' => array(
  //     'drush lt-prepare-builtins' => '',
  //   ),
  // );

  return $items;
}

/**
 * Callback function for drush command batch upload translatable content
 */
function drush_lingotek_push($entity_type) {
  try {
    $operations = array();
    if ($entity_type == 'config') {
      // adapted from lingotek_config_upload_selected() in lingotek.confg.inc
      $query = db_select('locales_source', 'ls');
      $query->addField('ls', 'textgroup');
      $query->addField('ls', 'lid');
      $results = $query->execute();
      $lid_map = array();
      foreach ($results as $result) {
        $lid_map[$result->textgroup][$result->lid] = $result->lid;
      }
      $unique_set_ids = LingotekConfigSet::bulkGetSetId($lid_map);
      foreach ($lid_map as $textgroup) {
        LingotekConfigSet::markLidsNotCurrent($textgroup);
      }
      if (!empty($not_current_lids)) {
        LingotekConfigSet::markLidsNotCurrent($not_current_lids);
      }
      $operations = lingotek_get_sync_upload_config_batch_elements($unique_set_ids);
    }
    else {
      $query = new EntityFieldQuery();
      $entity_ids = array();
      $query->entityCondition('entity_type', $entity_type);
      if ($bundles = drush_get_option('bundles')) {
        $list = explode(',', $bundles);
        $query->entityCondition('bundle', $list, 'IN');
      }
      $result = $query->execute();
      if (is_array($result[$entity_type])) {
        $entity_ids = array_keys($result[$entity_type]);
      }
      if (!empty($entity_ids)) {
        foreach ($entity_ids as $id) {
          $operations[] = array(
            'lingotek_entity_upload',
            array($id, $entity_type)
          );
        }
      }
      else {
        throw new Exception(dt('No entities of @type were found in the database', array('@type' => $entity_type)));
      }
    }
    $batch = array(
      'operations' => $operations,
      'finished' => 'lingotek_drush_push_complete',
      'title' => dt('Upload entities to Lingotek TMS'),
      'init_message' => dt('Beginning batch uploads of @type to Lingotek', array('@type' => $entity_type)),
    );
    batch_set($batch);
    drush_backend_batch_process();
  } catch (Exception $e) {
    drush_set_error('lt-push', $e->getMessage());
  }
}

/**
 * Callback function for drush command lt-cleanup
 */
function drush_lingotek_cleanup($entity_type) {
  try {
    $batch = lingotek_field_language_data_cleanup_batch_create($entity_type, FALSE);
    batch_set($batch);
    drush_backend_batch_process();
  } catch (Exception $e) {
    drush_set_error('lt-cleanup', $e->getMessage());
  }
}

/**
 * Implements callback_batch_finished()
 *
 */
function lingotek_drush_push_complete($success, $results, $operations) {
  if ($success) {
    drush_log(dt('Push to Lingotek TMS is complete.'), 'ok');
  }
}

/**
 * Callback function for drush command lt-prepare-blocks
 */
function drush_lingotek_prepare_blocks($config_groups, $variable) {
  try {
    lingotek_admin_prepare_blocks();
    drush_lingotek_prepare_item($config_groups, $variable);
  } catch (Exception $e) {
    drush_set_error('lt-prepare-blocks', $e->getMessage());
  }
}

/**
 * Callback function for drush command lt-prepare-taxonomies
 */
function drush_lingotek_prepare_vocab($config_groups, $variable) {
  try {
    lingotek_admin_prepare_taxonomies();
    drush_lingotek_prepare_item($config_groups, $variable);
  } catch (Exception $e) {
    drush_set_error('lt-prepare-taxonomies', $e->getMessage());
  }
}

/**
 * Callback function for drush command lt-prepare-menus
 */
function drush_lingotek_prepare_menus($config_groups, $variable) {
  try {
    lingotek_admin_prepare_menus();
    drush_lingotek_prepare_item($config_groups, $variable);
  } catch (Exception $e) {
    drush_set_error('lt-prepare-menus', $e->getMessage());
  }
}

// Commented out: error reported /home/{user}/.composer/vendor/drush/drush/commands/core/drupal/batch.inc on line 45 and 137
/**
 * Callback function for drush command lt-prepare-builtins
 */
// function drush_lingotek_prepare_builtins() {
//   try {
//     lingotek_admin_prepare_builtins($additional_operations);
//     drush_backend_batch_process();
//   } catch (Exception $e) {
//     drush_set_error('lt-prepare-builtins', $e->getMessage());
//   }
// }

/**
 * @param $config_groups
 *   array: of the form ('menu' => 'menu)
 * @param $variable
 *   name of a drupal variable to set.
 */
function drush_lingotek_prepare_item($config_groups, $variable) {
  try {
    // refresh all strings for this config type
    // combine string refresh operations with other additional operations
    $config_refresh_batch = i18n_string_refresh_batch($config_groups, $delete = FALSE);
    lingotek_admin_setup_nonbuiltins_batch($config_refresh_batch['operations']);
    drush_backend_batch_process();
    variable_set($variable, 1);
  } catch (Exception $e) {
    throw $e;
  }
}
