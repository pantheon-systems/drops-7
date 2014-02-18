<?php

/**
 * @file
 * Callbacks for datastore pages.
 */

/**
 * Callback for Data API instructions.
 */
function dkan_datastore_datastore_api($node) {
  $output = '<h2>' . t('DKAN Datastore API') . '</h2>';
  if ($importer_ids = feeds_get_importer_ids($node->type, $node->nid)) {
    foreach ($importer_ids as $importer_id) {
      $source_config = feeds_source($importer_id, $node->nid);
      $fetcher = get_class($source_config->importer->fetcher);
      $source = isset($source_config->config[$fetcher]['source']) ? $source_config->config[$fetcher]['source'] : '';
      if ($source) {
        $table_name = feeds_flatstore_processor_table_name($source_config->id, $source_config->feed_nid);
        if (!db_table_exists($table_name)) {
          $output .= t('This resources has a file that has not been added to the datastore.');
        }
        else {
          $progress = $source_config->progressImporting();
          if ($progress != FEEDS_BATCH_COMPLETE) {
            $output .= t('This data source has not yet completed importing. Import is at @progress%. The API for this data source will be available upon the completion of the import process.', array('@progress' => number_format(100 * $progress, 0)));
          }
          else {
            $output .= t('Access resource data via a web API with powerful query support.');
            $output .= '<h3>' . t('Resource ID') . '</h3>';
            $output .= t('The Resource ID for this resource is ') . $node->uuid;
            $output .= '<h3>' . t('Example Query') . '</h3>';
            global $base_url;
            $url = $base_url . '/api/action/datastore/search.json?resource_id=' . $node->uuid . '&limit=5';
            $output .= '<p>' . l($url, $url) . '</p>';
            $output .= '<p>' . t('Query this datastore and return first five results') . '</p>';
            $output .= '<h3>' . t('Documentation') . '</h3>';
            $output .= '<p>' . t('See DKAN API documentation for more details: ') . l(t('DKAN Datastore API'), 'http://docs.getdkan.com/docs/dkan-documentation/dkan-api/datastore-api') . '</p>';
          }
        }
        return $output;
      }
    }
  }
  else {
    $output .= t('No files have been added to the datastore.');
  }
  return $output;
}

/**
 * Callback for download tab.
 */
function dkan_datastore_download($node) {
  $node_wrapper = entity_metadata_wrapper('node', $node);
  if ($node_wrapper->field_upload->value()) {
    $url = file_create_url($node_wrapper->field_upload->value()->uri);
    return drupal_goto($url);
  }
  else {
    drupal_set_message(t('No download available for this resource'));
    return '';
  }
}

/**
 * Callback for back link.
 */
function dkan_datastore_back($node) {
  $node_wrapper = entity_metadata_wrapper('node', $node);
  $dataset = $node_wrapper->field_dataset_ref->value();
  if ($dataset) {
    drupal_goto('node/' . $dataset->nid);
  }
  return '';
}

/**
 * Callback for 'Add Resouce' tab.
 */
function dkan_datastore_add_resource($node) {
  drupal_goto('node/add/resource', array('query' => array('dataset' => $node->nid)));
}

/**
 * Render a feeds import form on node/id/import pages.
 */
function dkan_datastore_import_tab_form($form, &$form_state, $node) {
  module_load_include('inc', 'feeds', 'feeds.pages');

  $total_progress = 0;

  $importer_ids = feeds_get_importer_ids($node->type, $node->nid);

  $form = array();
  $form['source_status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Datastore Status'),
    '#tree' => TRUE,
  );
  if ($importer_ids) {
    $form['#feed_nid'] = $node->nid;
    $form['#redirect'] = 'node/' . $node->nid;
    foreach ($importer_ids as $importer_id => $weight) {
      $source = feeds_source($importer_id, $node->nid);
      $importer = feeds_importer($importer_id, $node->nid);
      $source_input = isset($source->config[$source->importer->config['fetcher']['plugin_key']]['source']) ? $source->config[$source->importer->config['fetcher']['plugin_key']]['source'] : '';
      $default_values = $source->config[$source->importer->config['parser']['plugin_key']];
      if ($source_input) {
        $form[$importer_id]['source_status'] = array(
          '#type' => 'fieldset',
          '#title' => t('@source_name', array('@source_name' => $source->importer->config['name'])),
          '#tree' => TRUE,
          '#value' => feeds_source_status($source),
        );
        $form[$importer_id]['settings'] = array(
          '#type' => 'fieldset',
          '#title' => t('Settings'),
        );
        $form[$importer_id]['settings'] = $source->configForm($form_state);
        $form[$importer_id]['settings']['#submit'] = array('feeds_form_submit');
        $form[$importer_id]['#count'] = $source->itemCount();
        $progress = $source->progressImporting();
        $total_progress += $progress;
      }
      else {
        unset($importer_ids[$importer_id]);
      }
    }
    if (count($importer_ids) == 1) {
      $form = confirm_form($form, t('Import all content from source?'), 'node/' . $node->nid, '', t('Import'), t('Cancel'), 'confirm feeds update');
      $form['importer_ids'] = array(
        '#type' => 'value',
        '#value' => $importer_ids,
      );
    }
    elseif ($importer_ids) {
      $form = confirm_form($form, t('Import all content from source?'), 'node/' . $node->nid, '', t('Import'), t('Cancel'), 'confirm feeds update');
      $options = array();
      foreach ($importer_ids as $importer_id => $weight) {
        $source = feeds_source($importer_id, $node->nid);
        $options[$importer_id] = $source->importer->config['name'];
      }
      $form['importer_ids'] = array(
        '#type' => 'checkboxes',
        '#options' => $options,
        '#default_value' => array_keys($options),
        '#title' => t('Sources'),
        '#description' => t('Select the sources to import.'),
      );
    }
    else {
      $form['no_source'] = array(
        '#markup' => t('The file type for this resource is not supported by the datastore.'),
      );
    }
    if (count($importer_ids)) {
      $progress = $total_progress / count($importer_ids);
      if ($progress !== FEEDS_BATCH_COMPLETE) {
        $form['actions']['submit']['#disabled'] = TRUE;
        $form['actions']['submit']['#value'] = t('Importing (@progress %)', array('@progress' => number_format(100 * $progress, 0)));
      }
    }
  }
  else {
    $form['no_source'] = array(
      '#markup' => t('There is nothing to manage! You need to upload or link to a file in order to use the datastore.'),
    );
  }
  return $form;
}

/**
 * Submit function for import tab.
 */
function dkan_datastore_import_tab_form_submit($form, &$form_state) {
  $form_state['redirect'] = $form['#redirect'];
  foreach (array_filter($form_state['values']['importer_ids']) as $importer_id) {
    $source = feeds_source($importer_id, $form_state['build_info']['args'][0]->nid);
    $parser = $source->importer->config['parser']['plugin_key'];
    $source->addConfig(array($parser => $form_state['values'][$parser]));
    $source->save();
    feeds_source($importer_id, $form['#feed_nid'])->startImport();
  }
}

/**
 * Render a feeds delete form.
 *
 * Used on both node pages and configuration pages.
 * Therefore $node may be missing.
 */
function dkan_datastore_delete_tab_form($form, &$form_state, $importer_id, $node = NULL) {
  module_load_include('inc', 'feeds', 'feeds.pages');
  $total_progress = 0;

  $form = array();
  if (empty($node)) {
    $source = feeds_source($importer_id);
    $form['#redirect'] = 'import/' . $source->id;
    $importer_ids = array($importer_id);
  }
  else {
    $importer_ids = feeds_get_importer_ids($node->type, $node->nid);
    $form['#redirect'] = 'node/' . $node->nid;
  }

  if ($importer_ids) {
    // Form cannot pass on source object.
    $form['#feed_nid'] = $node->nid;
    foreach ($importer_ids as $importer_id => $weight) {
      $source = feeds_source($importer_id, $node->nid);
      $form[$importer_id]['source_status'] = array(
        '#type' => 'fieldset',
        '#title' => t('@source_name: Status', array('@source_name' => $source->importer->config['name'])),
        '#tree' => TRUE,
        '#value' => feeds_source_status($source),
      );
      $progress = $source->progressClearing();
      $total_progress += $progress;
    }

    if (count($importer_ids) == 1) {
      $form['importer_ids'] = array(
        '#type' => 'value',
        '#value' => array($importer_id),
      );
    }
    else {
      $options = array();
      foreach ($importer_ids as $importer_id => $weight) {
        $source = feeds_source($importer_id, $node->nid);
        $options[$importer_id] = $source->importer->config['name'];
      }
      $form['importer_ids'] = array(
        '#type' => 'checkboxes',
        '#options' => $options,
        '#default_value' => array_keys($options),
        '#title' => t('Sources'),
        '#description' => t('Select the sources to delete items from.'),
      );
    }

    $form = confirm_form($form, t('Delete all items from source?'), $form['#redirect'], '', t('Delete'), t('Cancel'), 'confirm feeds update');

    if (count($importer_ids)) {
      $progress = $total_progress / count($importer_ids);
      if ($progress !== FEEDS_BATCH_COMPLETE) {
        $form['actions']['submit']['#disabled'] = TRUE;
        $form['actions']['submit']['#value'] = t('Deleting (@progress %)', array('@progress' => number_format(100 * $progress, 0)));
      }
    }
  }
  else {
    $form['no_source'] = array(
      '#markup' => t('No feeds sources added to node.'),
    );
  }

  $form['#submit'] = array('feeds_delete_tab_form_submit');
  return $form;
}

/**
 * Render a feeds unlock form.
 *
 * Used on both node pages and configuration pages.
 * Therefore $node may be missing.
 */
function dkan_datastore_unlock_tab_form($form, &$form_state, $importer_id, $node = NULL) {
  module_load_include('inc', 'feeds', 'feeds.pages');
  if (empty($node)) {
    $source = feeds_source($importer_id);
    $form['#redirect'] = 'import/' . $source->id;
  }
  else {
    $importer_id = feeds_get_importer_id($node->type);
    $source = feeds_source($importer_id, $node->nid);
    $form['#redirect'] = 'node/' . $source->feed_nid;
  }
  // Form cannot pass on source object.
  $form['#importer_id'] = $source->id;
  $form['#feed_nid'] = $source->feed_nid;
  $form['source_status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Status'),
    '#tree' => TRUE,
    '#value' => feeds_source_status($source),
  );
  $form = confirm_form($form, t('Unlock this importer?'), $form['#redirect'], '', t('Delete'), t('Cancel'), 'confirm feeds update');
  if ($source->progressImporting() == FEEDS_BATCH_COMPLETE && $source->progressClearing() == FEEDS_BATCH_COMPLETE) {
    $form['source_locked'] = array(
      '#type' => 'markup',
      '#title' => t('Not Locked'),
      '#tree' => TRUE,
      '#markup' => t('This importer is not locked, therefore it cannot be unlocked.'),
    );
    $form['actions']['submit']['#disabled'] = TRUE;
    $form['actions']['submit']['#value'] = t('Unlock (disabled)');
  }
  else {
    $form['actions']['submit']['#value'] = t('Unlock');
  }
  $form['#submit'] = array('feeds_unlock_tab_form_submit');
  return $form;
}

/*
* Render a datastore drop form on node/id/drop pages
*/
function dkan_datastore_drop_tab_form($form, &$form_state, $node) {
  module_load_include('module', 'data', 'data');
  module_load_include('inc', 'feeds', 'feeds.pages');
  $form['#redirect'] = 'node/' . $node->nid;
  $form = confirm_form($form, t('Drop this datastore?'), $form['#redirect'], '', t('Drop'), t('Cancel'), 'confirm drop');
  
  $importer_ids = feeds_get_importer_ids($node->type);
  $tables_to_drop = array();

  foreach ($importer_ids as $importer_id) {
    $source = feeds_source($importer_id, $node->nid);
    $table_name = feeds_flatstore_processor_table_name($source->id, $source->feed_nid);
    $table = data_get_table($table_name);
    if($table){
      array_push($tables_to_drop, $table_name);
    }
  }

  if (!count($tables_to_drop)){
    $form['tables_absent'] = array(
      '#type' => 'markup',
      '#title' => t('Can\'t drop table'),
      '#tree' => TRUE,
      '#markup' => t('You need to have a file or link imported to the datastore in order to drop it.'),
    );
    $form['actions']['submit']['#disabled'] = TRUE;
    $form['actions']['submit']['#value'] = t('Drop (disabled)');
  }else{
    $form['tables_present'] = array(
      '#type' => 'markup',
      '#title' => t('Drop tables'),
      '#tree' => TRUE,
      '#markup' => t('Are you sure you want to drop the datastore?'),
    );
    $form['#tables_to_drop'] = $tables_to_drop;
    $form['actions']['submit']['#value'] = t('Drop');
  }
  return $form;
}

/**
 * Submit function for drop tab.
 */
function dkan_datastore_drop_tab_form_submit($form, &$form_state) {
  module_load_include('module', 'data', 'data');
  foreach($form['#tables_to_drop'] as $table_name){
    $table = data_get_table($table_name);
    $table->drop();
  }
  drupal_set_message(t('Datastore dropped!'));
  $form_state['redirect'] = $form['#redirect'];
}
