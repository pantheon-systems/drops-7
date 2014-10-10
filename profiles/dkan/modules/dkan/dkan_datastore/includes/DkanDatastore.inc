<?php

/**
 * @file
 * DKAN Datatore implementation of Datastore class.
 */

class DkanDatastore extends Datastore implements DatastoreForm {

  public $endpoint = 'api/action/datastore/search.json';

  /**
   * Sets default items for datastore object.
   */
  protected function __construct($uuid) {
    $this->uuid = $uuid;
    $this->node = $this->node($uuid);
    $this->importerId = $this->importerId();
    $this->source = $this->source();
    $this->importer = $this->importer();
    $this->sourceId = dkan_datastore_api_get_feeds_source($this->node->nid);
    $this->tableName = feeds_flatstore_processor_table_name($this->sourceId, $this->node->nid);
    $this->status = $this->status();
  }

  /**
   * Retrieves importer id.
   */
  public function importerId() {
    $id = db_query("SELECT id FROM {feeds_source} WHERE feed_nid = :nid", array(':nid' => $this->node->nid))->fetchField();
    return $id;
  }

  /**
   * Revrieves source from $node if it exists.
   */
  public function source() {
    if ($this->importerId) {
      $source = feeds_source($this->importerId, $this->node->nid);
      if ($source) {
        return $source;
      }
    }
    return FALSE;
  }

  /**
   * Revrieves importer from $node if it exists.
   */
  public function importer() {
    if ($this->importerId) {
      $importer = feeds_importer($this->importerId, $this->node->nid);
      if ($importer) {
        return $importer;
      }
    }
    return FALSE;
  }

  /**
   * Returns Importer ID.
   */

  /**
   * Retrieves importers from node and instantiates FeedsSource.
   */
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

  /**
   * Returns number of records in the datastore.
   */
  public function records() {
    if ($records = dkan_datastore_records($this->node->nid)) {
      return $records;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Retrieves current Datastore status.
   */
  public function status() {
    if ($file = dkan_datastore_file_field($this->node)) {
      if ((is_object($file) && $file->filemime == 'text/csv') || (is_array($file) && $file['filemime'] == 'text/csv')) {
        if ($this->records()) {
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

  /**
   * Construcs api endpoint.
   */
  public function apiUri() {
    return url($this->endpoint, array(
      'query' => array('resource_id' => $this->uuid, 'limit' => 5), 'absolute' => TRUE));
  }

  /**
   * Retrievs number of rows from the Datastore.
   */
  public function rows() {
    if ($this->status == DKAN_DATASTORE_EXISTS) {
      $result = db_select($this->tableName, 't')->addExpression('count(t.timestamp)')->execute();
      return $result->fetchField();
    }
    return FALSE;
  }

  /**
   * Retrieves headers from Datastore.
   */
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

  /**
   * Creates manage form for Datastore.
   */
  public function manageForm(&$form_state) {
    $node = $this->node;
    module_load_include('inc', 'feeds', 'feeds.pages');
    $total_progress = 0;
    $importer_id = $this->importerId;
    $form = array();
    if ($importer_id) {
      $source = $this->source;
      $importer = $this->importer;
      $plugin = $source->importer->config['fetcher']['plugin_key'];
      $form[$importer_id]['source_status'] = array(
        '#type' => 'item',
        '#title' => t('@source_name: Status', array('@source_name' => $source->importer->config['name'])),
        '#markup' => $this->statusMessage(),
      );
      $form['#feed_nid'] = $node->nid;
      $form['#redirect'] = 'node/' . $node->nid;
      $source_input = isset($source->config[$plugin]['source']) ? $source->config[$plugin]['source'] : '';
      $default_values = $source->config[$plugin];
      if ($source_input) {
        $form[$importer_id]['settings'] = array(
          '#type' => 'fieldset',
          '#title' => t('Settings'),
        );
        $form[$importer_id]['settings'] = $source->configForm($form_state);
        $form[$importer_id]['settings']['#submit'] = array('feeds_form_submit');
        $form[$importer_id]['#count'] = $source->itemCount();
        $progress = $source->progressImporting();
        $form = confirm_form($form, t('Import all content from source?'), 'node/' . $node->nid, '', t('Import'), t('Cancel'), 'confirm feeds update');
        $form[$importer_id]['description']['#weight'] = '-10';
        $form[$importer_id]['source_status']['#weight'] = '-9';
        if (isset($form[$importer_id]['settings']['FeedsCSVParser']['help'])) {
          unset($form[$importer_id]['settings']['FeedsCSVParser']['help']);
        }
        $form['importer_id'] = array(
          '#type' => 'value',
          '#value' => $importer_id,
        );
        if ($progress !== FEEDS_BATCH_COMPLETE) {
          $form['actions']['submit']['#disabled'] = TRUE;
          $form['actions']['submit']['#value'] = t('Importing (@progress %)', array('@progress' => number_format(100 * $progress, 0)));
        }
      }
      else {
        $form['no_source'] = array(
          '#markup' => t('The file type for this resource is not supported by the datastore.'),
        );
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
   * Takes values from form submit, saves updated config and starts import.
   */
  public function manageFormSubmit(&$form_state) {
    $importer_id = $form_state['values']['importer_id'];
    $source = feeds_source($importer_id, $this->node->nid);
    $parser = $source->importer->config['parser']['plugin_key'];
    $processor = $source->importer->config['processor']['plugin_key'];
    $this->addConfig(array($parser => $form_state['values'][$parser]));
    $this->addConfig(array($processor => $form_state['values'][$processor]));
    $this->save();
    $this->import();
  }

  /**
   * Adds configuration to feeds source.
   */
  public function addConfig($config) {
    $this->source->addConfig($config);
    $this->source->addConfig($config);
  }

  /**
   * Saves feeds source.
   */
  public function save() {
    $this->source->save();
  }

  /**
   * Provides themed status message for import.
   */
  public function statusMessage() {
    return feeds_source_status($this->source);
  }

  /**
   * Provides delete form.
   */
  public function deleteForm(&$form_state) {
    module_load_include('inc', 'feeds', 'feeds.pages');
    $total_progress = 0;
    $node = $this->node;

    $form = array();
    $importer_id = $this->importerId;
    $form['#redirect'] = 'node/' . $node->nid;

    if ($importer_id) {
      // Form cannot pass on source object.
      $form['#feed_nid'] = $node->nid;
      $source = feeds_source($importer_id, $node->nid);
      $form[$importer_id]['source_status'] = array(
        '#type' => 'item',
        '#title' => t('@source_name: Status', array('@source_name' => $source->importer->config['name'])),
        '#tree' => TRUE,
        '#markup' => $this->statusMessage(),
      );
      $progress = $source->progressClearing();
      $form['importer_ids'] = array(
        '#type' => 'value',
        '#value' => array($importer_id),
      );

      $form = confirm_form($form, t('Delete all items from source?'), $form['#redirect'], '', t('Delete'), t('Cancel'), 'confirm feeds update');

      if ($progress !== FEEDS_BATCH_COMPLETE) {
        $form['actions']['submit']['#disabled'] = TRUE;
        $form['actions']['submit']['#value'] = t('Deleting (@progress %)', array('@progress' => number_format(100 * $progress, 0)));
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

  /**
   * Provides form to drop datastore.
   */
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
      $table = data_get_table($table_name) ? data_get_table($table_name) : db_table_exists($table_name);
      if ($table) {
        array_push($tables_to_drop, $table_name);
      }
    }
    if (!count($tables_to_drop)) {
      $form['tables_absent'] = array(
        '#type' => 'markup',
        '#title' => t("Can't drop table"),
        '#tree' => TRUE,
        '#markup' => t('You need to have a file or link imported to the datastore in order to drop it.'),
      );
      $form['actions']['submit']['#disabled'] = TRUE;
      $form['actions']['submit']['#value'] = t('Drop (disabled)');
    }
    else {
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
   * Drop form submision.
   */
  public function dropFormSubmit(&$form_state) {
    module_load_include('module', 'data', 'data');
    foreach ($form_state['#tables_to_drop'] as $table_name) {
      $table = data_get_table($table_name);
      if ($table) {
        $table->drop();
      }
      elseif (db_table_exists($table_name)) {
        db_drop_table($table_name);
      }
    }
    drupal_set_message(t('Datastore dropped!'));
  }

  /**
   * Provides API form.
   */
  public function apiForm() {
    // TODO: Make a theme function.
    $output = '<h2>' . t('DKAN Datastore API') . '</h2>';
    if ($importer_id = $this->importerId) {
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
            $output .= t("The Resource ID for this resource is %id", array('%id' => $this->node->uuid));
            $output .= '<h3>' . t('Example Query') . '</h3>';
            $url = $this->apiUri();
            $output .= '<p>' . l($url, $url) . '</p>';
            $output .= '<p>' . t('Query this datastore and return first five results') . '</p>';
            $output .= '<h3>' . t('Documentation') . '</h3>';
            $doc_link = l(t('DKAN Datastore API'), 'http://docs.getdkan.com/docs/dkan-documentation/dkan-api/datastore-api');
            $output .= '<p>' . t('See DKAN API documentation for more details: %link', array('%link' => $doc_link)) . '</p>';
          }
        }
        return $output;
      }
    }
    else {
      $output .= t('No files have been added to the datastore.');
    }
    return $output;

  }

  /**
   * Starts import.
   */
  public function import() {
    $feed = isset($this->feed) ? $this->feed : $this->feed($this->node);
    $feed['source']->startImport();
  }

}
