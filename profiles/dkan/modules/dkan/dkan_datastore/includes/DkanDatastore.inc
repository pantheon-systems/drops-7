<?php

/**
 * @file
 * DKAN Datatore implementation of Datastore class.
 */

class DkanDatastore extends Datastore implements DatastoreForm {

  public $endpoint = 'api/action/datastore/search.json';

  protected function __construct($uuid) {
    $this->uuid = $uuid;
    $this->config = $this->configDefaults();
    $this->node = $this->node($uuid);
    $this->importerIds = feeds_get_importer_ids($this->node->type, $this->node->nid);
    $this->sourceId = dkan_datastore_api_get_feeds_source($this->node->nid);
    $this->tableName = feeds_flatstore_processor_table_name($this->sourceId , $this->node->nid);
    $this->status = $this->status();
  }

  public function feed($node) {
    $importer_ids = feeds_get_importer_ids($node->type, $node->nid);
    foreach ($importer_ids as $importer_id) {
      $importer = feeds_importer($importer_id, $node->nid);
      $source = feeds_source($importer_id, $node->nid);
      $source_input = isset($source->config[$source->importer->config['fetcher']['plugin_key']]['source']) ? $source->config[$source->importer->config['fetcher']['plugin_key']]['source'] : '';
      if ($source_input) {
        return array('source' => $source, 'importer' => $importer);
      }
    }
    return FALSE;
  }

  public function status() {
    if ($file = dkan_datastore_file_field($this->node)) {
      if ((is_object($file) && $file->filemime == 'text/csv') || (is_array($file) && $file['filemime'] == 'text/csv')) {
        if (dkan_datastore_records($this->node->nid)) {
          return DKAN_DATASTORE_EXISTS;
        }
        else {
          return DKAN_DATASTORE_FILE_EXISTS;
        }
      }
      else {
        return DKAN_DATASTORE_WRONG_TYPE;
      }
    }
    return FALSE;
  }

  public function apiUri() {
    return url($this->endpoint, array('query' => array('resource_id' => $this->uuid, 'limit' => 5), 'absolute' => TRUE));
  }

  public function rows() {
    if ($this->status == DKAN_DATASTORE_EXISTS) {
      return db_query("SELECT count(timestamp) FROM {$this->tableName}")->fetchField();
    }
    return FALSE;
  }

  public function headers() {
    if ($this->status == DKAN_DATASTORE_EXISTS) {
      $table = data_get_table($this->tableName);
      if (isset($table->meta['fields'])) {
        $headers = array();
        foreach ($table->meta['fields'] as $safe_name => $label) {
          $headers[$safe_name] = $label['label'];
        }
        return $headers;
      }
    }
    return FALSE;
  }

  public function manageForm(&$form_state) {
    $node = $this->node;
    module_load_include('inc', 'feeds', 'feeds.pages');

    $total_progress = 0;

    $importer_ids = $this->importerIds;

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
        $form['description']['#markup'] = t('Manage datastore import settings below. To import click "submit"');
        $form['description']['#weight'] = '-10';
        $form['source_status']['#weight'] = '-11';
        $importer = array_keys($importer_ids);
        if (isset($form[$importer[0]]['settings']['FeedsCSVParser']['help'])) {
          unset($form[$importer[0]]['settings']['FeedsCSVParser']['help']);
        }
        $form[$importer[0]]['source_status']['#title'] = '';
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

  public function manageFormSubmit(&$form_state) {
    foreach (array_filter($form_state['values']['importer_ids']) as $importer_id) {
      $source = feeds_source($importer_id, $this->node->nid);
      $parser = $source->importer->config['parser']['plugin_key'];
      $processor = $source->importer->config['processor']['plugin_key'];
      $source->addConfig(array($parser => $form_state['values'][$parser]));
      $source->addConfig(array($processor => $form_state['values'][$processor]));
      // TODO: pass to datastore save / import functions.
      $source->save();
      $source->startImport();
    }
  }

  public function deleteForm(&$form_state) {
    module_load_include('inc', 'feeds', 'feeds.pages');
    $total_progress = 0;
    $node = $this->node;

    $form = array();
    $importer_ids = feeds_get_importer_ids($node->type, $node->nid);
    $form['#redirect'] = 'node/' . $node->nid;

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
    return $form;
  }

  /**
   * Feeds does the magic on this.
   *
   * TODO: undo feeds magic.
   */
  public function deleteFormSubmit(&$form_submit) {}

  public function dropForm(&$form_state) {
    module_load_include('module', 'data', 'data');
    module_load_include('inc', 'feeds', 'feeds.pages');
    $node = $this->node;
    $form = array();
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

  public function dropFormSubmit(&$form_state) {
    module_load_include('module', 'data', 'data');
    foreach($form_state['#tables_to_drop'] as $table_name){
      $table = data_get_table($table_name);
      $table->drop();
    }
    drupal_set_message(t('Datastore dropped!'));
  }

  public function apiForm() {
    // TODO: Make a theme function.
    $output = '<h2>' . t('DKAN Datastore API') . '</h2>';
    if ($importer_ids = $this->importerIds) {
      foreach ($importer_ids as $importer_id) {
        $source_config = feeds_source($importer_id, $this->node->nid);
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
              $output .= t('This data source has not yet completed importing. Import is at @progress%. The API for this data source will be available upon the completion of the import process.', array('@ progress' => number_format(100 * $progress, 0)));
            }
            else {
              $output .= t('Access resource data via a web API with powerful query support.');
              $output .= '<h3>' . t('Resource ID') . '</h3>';
              $output .= t('The Resource ID for this resource is ') . $this->node->uuid;
              $output .= '<h3>' . t('Example Query') . '</h3>';
              $url = $this->apiUri();
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

  public function import() {
    $feed = isset($this->feed) ? $this->feed : $this->feed($this->node);
    $feed['source']->startImport();
  }

}
