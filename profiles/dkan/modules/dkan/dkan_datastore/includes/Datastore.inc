<?php

/**
 * @file
 * Datastore class and helper functions.
 */

/**
 * Creates Datastore class.
 */
abstract class Datastore {
  protected $uuid;
  protected $config;
  public $imported;
  public $endpoint;
  public $node;

  /**
   * Constructor, set uuid and load default configuration.
   */
  protected function __construct($uuid) {
    // Set this object's id.
    $this->uuid = $uuid;
    // Make sure configuration is populated.
    $this->config = $this->configDefaults();
    $this->node = $this->node($uuid);
  }

  public function configDefaults() {
  }

  /**
   * Loads node from uuuid.
   */
  protected function node($uuid) {
    static $nodes = array();
    if (!isset($node[$uuid])) {
      $nid = $this->getNid($uuid);
      if (!$nid) {
        throw new Exception(t("uuid !uuid not found.", array('!uuid' => $uuid)));
        return FALSE;
      }
      else {
        $nodes[$uuid] = node_load($nid);
      }
    }
    return $nodes[$uuid];
  }

  /**
   * Gets nid using uuid.
   */
  public function getNid($uuid) {
    $nid = db_query('SELECT nid FROM {node} WHERE uuid = :uuid', array(':uuid' => $uuid))->fetchField();
    if ($nid) {
      return $nid;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Instantiate a Datastore object with some baseline caching.
   *
   * Don't use directly, use dkan_datastore_go() instead.
   */
  public static function instance($class, $id) {
    // This is useful at least as long as we're developing.
    if (empty($id)) {
      throw new Exception(t('Empty configuration identifier.'));
    }
    static $instances = array();
    if (!isset($instances[$class][$id])) {
      $instances[$class][$id] = new $class($id);
    }
    return $instances[$class][$id];
  }

  /**
   * Imports file contents into the datastore.
   */
  public function import() {
  }

  /**
   * Drops datastore.
   */
  public function drop($form) {
  }

  /**
   * Retrieves headers.
   */
  public function headers() {
  }

  /**
   * Provides number of rows in datastore.
   */
  public function rows() {
  }

  /**
   * Provides API endpoint.
   */
  public function apiUri() {
  }

  /**
   * Deletes rows from the Datastore.
   */
  public function deleteRows($rows = 'all') {
  }

  /**
   * Unlocks locked datastore for updates.
   */
  public function unlock() {
  }

  /**
   * Retrieves file object from datastore.
   */
  public function file() {
    if ($file = dkan_datastore_file_field($this->node)) {
      return $file;
    }
  }

  /**
   * Retrieves file URI.
   */
  public function fileUri() {
    $file = $this->file();
    return $file->uri;
  }

  /**
   * Retrieves file URL.
   */
  public function fileUrl() {
    $file = $this->file();
    return file_create_url($file->uri);
  }


  /**
   * Retrieves timestamp for last import.
   */
  public function lastImport() {
  }

  /**
   * Retrieves progress of datastore import.
   */
  public function progress() {
  }

  /**
   * Retrieves status of the datastore.
   *
   * TODO: Document status levels.
   */
  public function status() {
  }

}

/**
 * Declares interface for forms required for interaction with Datastores through
 * the DKAN user interface.
 */
interface DatastoreForm {

  public function manageForm(&$form_state);

  public function manageFormSubmit(&$form_state);

  public function deleteForm(&$form_state);

  public function deleteFormSubmit(&$form_state);

  public function dropForm(&$form_state);

  public function dropFormSubmit(&$form_state);

  public function apiForm();

}
