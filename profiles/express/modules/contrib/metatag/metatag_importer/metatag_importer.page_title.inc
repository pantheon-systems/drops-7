<?php
/**
 * @file
 * Functionality for migrating data from the Page Title module.
 */

/**
 * FormAPI callback for the Page Title importer.
 */
function metatag_importer_page_title_form($form, &$form_state) {
  $form['help'] = array(
    '#markup' => t('To migrate data from Page Title it is necessary to use Drush. See "drush metatag-convert-page-title" for details.'),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );
  return $form;
}

/**
 * Migrate data from the page_title table, if available.
 *
 * @return int
 *   The number of records that were converted.
 */
function metatag_importer_for_page_title() {
  $converted = 0;
  $page_titles = db_select('page_title', 'pt')
    ->fields('pt', array('type', 'id', 'page_title'))
    ->execute();

  // Get general metatag config settings.
  $metatag_config_global = metatag_config_load('global');
  $metatag_config_node = metatag_config_load('node');
  $metatag_config_taxonomy_term = metatag_config_load('taxonomy_term');
  $metatag_config_user = metatag_config_load('user');

  // Track any records that are skipped.
  $skipped = array();

  // Loop over each of the page_title records.
  while ($pt_data = $page_titles->fetchObject()) {
    $entity_type = $pt_data->type;
    $entity_id = $pt_data->id;

    // Load the entity.
    $entity = entity_load($entity_type, array($entity_id));
    if (empty($entity)) {
      $skipped[] = $entity_type . ':' . $entity_id;
      continue;
    }

    $entity = reset($entity);

    // Extract additional values from the entity.
    $langcode = metatag_entity_get_language($entity_type, $entity);
    list($entity_id, $revision_id) = entity_extract_ids($entity_type, $entity);

    // Load any possible existing meta tags for this object.
    $data = metatag_metatags_load($entity_type, $entity_id);

    // Drop back one level because the results will be keyed by revision_id.
    if (!empty($data)) {
      $data = reset($data);
    }

    switch ($entity_type) {
      case 'node':
        $metatag_config_node_type = metatag_config_load('node:' . $entity->type);

        if (!empty($metatag_config_node_type) && isset($metatag_config_node_type->config['title'])) {
          $title_setting = $metatag_config_node_type->config['title']['value'];
        }
        elseif (isset($metatag_config_node->config['title'])) {
          $title_setting = $metatag_config_node->config['title']['value'];
        }
        else {
          $title_setting = $metatag_config_global->config['title']['value'];
        }
        $metatag_title = str_replace('[current-page:title]', trim($pt_data->page_title), $title_setting);
        $metatag_title = str_replace('[node:title]', trim($pt_data->page_title), $metatag_title);
        break;

      case 'taxonomy_term':
        $metatag_config_vocabulary_type = metatag_config_load('taxonomy_term:' . $entity->vocabulary_machine_name);

        if (!empty($metatag_config_vocabulary_type) && isset($metatag_config_vocabulary_type->config['title'])) {
          $title_setting = $metatag_config_vocabulary_type->config['title']['value'];
        }
        elseif (isset($metatag_config_taxonomy_term->config['title'])) {
          $title_setting = $metatag_config_taxonomy_term->config['title']['value'];
        }
        else {
          $title_setting = $metatag_config_global->config['title']['value'];
        }
        $metatag_title = str_replace('[current-page:title]', trim($pt_data->page_title), $title_setting);
        $metatag_title = str_replace('[term:name]', trim($pt_data->page_title), $metatag_title);
        break;

      case 'user':
        if (isset($metatag_config_user->config['title'])) {
          $title_setting = $metatag_config_user->config['title']['value'];
        }
        else {
          $title_setting = $metatag_config_global->config['title']['value'];
        }
        $metatag_title = str_replace('[current-page:title]', trim($pt_data->page_title), $title_setting);
        $metatag_title = str_replace('[user:name]', trim($pt_data->page_title), $metatag_title);
        break;

      // Something else? Leave such records for another time.
      default:
        $skipped[] = $entity_type . ':' . $entity_id;
        // Jump back to the outer for() loop.
        continue 2;
    }
    $data[$langcode]['title']['value'] = $metatag_title;

    metatag_metatags_save($entity_type, $entity_id, $revision_id, $data);
    $converted += db_delete('page_title')
      ->condition('type', $entity_type)
      ->condition('id', $entity_id)
        ->execute();
  }

  // Log any records that were skipped.
  if (!empty($skipped)) {
    watchdog('metatag_importer', "Failed to convert the following page_title records: :records", array(':records' => implode(', ', $skipped)));
  }

  return $converted;
}
