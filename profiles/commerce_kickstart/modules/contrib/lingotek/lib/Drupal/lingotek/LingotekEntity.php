<?php

/**
 * @file
 * Defines LingotekEntity.
 */
 
/**
 * A class wrapper for Lingotek-specific behavior on nodes.
 */
class LingotekEntity implements LingotekTranslatableEntity {  
  /**
   * A Drupal node.
   *
   * @var object
   */
  protected $entity;
  
  /**
   * The Drupal entity type associated with this class
   */
  protected $entity_type;
  
  /**
   * A reference to the Lingotek API.
   *
   * @var LingotekApi
   */
  protected $api = NULL;
  
  public $language = '';

  /**
   * Constructor.
   *
   * This is private since we want consumers to instantiate via the factory methods.
   *
   * @param object $node
   *   A Drupal node.
   */
  private function __construct($entity, $entity_type) {
    $this->entity = $entity;
    $this->language = $entity->language;
    $this->language_targets = Lingotek::getLanguagesWithoutSource($this->language);
    $this->entity_type = $entity_type;
    $this->info = entity_get_info($this->entity_type);
  }
  
  /**
   * Injects reference to an API object.
   *
   * @param LingotekApi $api
   *   An instantiated Lingotek API object.
   */
  public function setApi(LingotekApi $api) {
    $this->api = $api;
  }
  
  /**
   * Factory method for getting a loaded LingotekEntity object
   *
   * @param object $entity
   *   A Drupal entity.
   *
   * @return LingotekEntity
   *   A loaded LingotekEntity object.
   */
  public static function load($entity, $entity_type) {
    $entity = new LingotekEntity($entity, $entity_type);
    $entity->setApi(LingotekApi::instance());
    return $entity;
  }
  
  /**
   * Loads a LingotekNode by Lingotek Document ID.
   *
   * @param string $lingotek_document_id
   *   The Document ID whose corresponding node should be loaded.
   *
   * @return mixed
   *   A LingotekNode object on success, FALSE on failure.
   */
  public static function loadByLingotekDocumentId($lingotek_document_id) {
    $entity = FALSE;
    
    $query = db_select('{lingotek_entity_metadata}', 'l')->fields('l');
    $query->condition('entity_key', 'document_id');
    $query->condition('value', $lingotek_document_id);
    $result = $query->execute();

    if ($record = $result->fetchAssoc()) {
      $id = $record['entity_id'];
      $entity_type = $record['entity_type'];
    }
    
    if ($id) {
      $entity = self::loadById($id, $entity_type);
    }
    
    return $entity;
  }


  /**
   * Gets the Lingotek document ID for this entity.
   *
   * @return mixed
   *   The integer document ID if the entity is associated with a 
   *   Lingotek document. FALSE otherwise.
   */
  public function lingotekDocumentId() {
    return $this->entity->lingotek['document_id'];
  }
  
  /**
   * Gets the contents of this item formatted as XML that can be sent to Lingotek.
   *
   * @return string
   *   The XML document representing the entity's translatable content.
   */
  public function documentLingotekXML() {
    $xml = lingotek_entity_xml_body($this->entity_type, $this->entity);
    return $xml;
  }  
  
  /**
   * Magic get for access to node and node properties.
   */  
  public function __get($property_name) {
    $property = NULL;
    
    if ($property === 'node') {
      $property = $this->entity;
    }
    elseif (isset($this->entity->$property_name)) {
      $property = $this->entity->$property_name;
    } else {
      $val = lingotek_keystore($this->getEntityType(), $this->getId(), $property_name); 
      $property = ($val !== FALSE) ? $val : $property;
    } 
    
    return $property;
  }
  

  /**
   * Gets the local Lingotek metadata for this entity.
   *
   * @return array
   *   An array of key/value data for the current entity.
   */
  protected function metadata() {
    $metadata = array();

    $results = db_select('{lingotek_entity_metadata}', 'meta')
        ->fields('meta')
      ->condition('entity_id', $this->getId())
      ->condition('entity_type', $this->entity_type)
      ->execute();

    foreach ($results as $result) {
      $metadata[$result->entity_key] = $result->value;
    }

    return $metadata;
  }

  /**
   * Gets a Lingotek metadata value for this item.
   *
   * @param string $key
   *   The key whose value should be returned.
   *
   * @return string
   *   The value for the specified key, if it exists.
   */
  public function getMetadataValue($key) {
    return db_select('{lingotek_entity_metadata}', 'meta')
            ->fields('meta', array('value'))
      ->condition('entity_key', $key)
      ->condition('entity_id', $this->getId())
      ->condition('entity_type', $this->getEntityType())
      ->execute()
      ->fetchField();
  }

  /**
   * Sets a Lingotek metadata value for this item.
   *
   * @param string $key
   *   The key for a name/value pair.
   * @param string $value
   *   The value for a name/value pair.
   */
  public function setMetadataValue($key, $value) {
    $metadata = $this->metadata();
    if (!isset($metadata[$key])) {
      db_insert('{lingotek_entity_metadata}')
          ->fields(array(
          'entity_id' => $this->getId(),
          'entity_type' => $this->getEntityType(),
          'entity_key' => $key,
          'value' => $value,
        ))
        ->execute();

    }
    else {
      db_update('{lingotek_entity_metadata}')
          ->fields(array(
          'value' => $value
        ))
        ->condition('entity_id', $this->getId())
        ->condition('entity_type', $this->getEntityType())
        ->condition('entity_key', $key)
        ->execute();
    }
  }
  
  /**
   * Deletes a Lingotek metadata value for this item
   * 
   * @param string $key
   *  The key for a name/value pair
   */
  public function deleteMetadataValue($key) {
    $metadata = $this->metadata();
    if (isset($metadata[$key])) {
      db_delete('{lingotek_entity_metadata}')
          ->condition('entity_id', $this->getId())
        ->condition('entity_type', $this->getEntityType())
        ->condition('entity_key', $key, 'LIKE')
        ->execute();
    }
  }

  /**
   * Updates the local content of $target_code with data from a Lingotek Document
   *
   * @param string $lingotek_locale
   *   The code for the language that needs to be updated.
   * @return bool
   *   TRUE if the content updates succeeded, FALSE otherwise.
   */
  public function downloadTriggered($lingotek_locale) {
    if (module_exists('rules')) {
      rules_invoke_event('lingotek_entity_translation_ready', new EntityDrupalWrapper($this->entity_type, $this->entity));
    }
    return lingotek_entity_download_triggered($this->entity, $this->entity_type, $lingotek_locale);
  }
  
  public function getWorkflowId() {
    return $this->entity->lingotek['workflow_id'];
  }
  
  public function getProjectId() {
    return $this->entity->lingotek['project_id'];
  }
  
  public function getVaultId() {
    return $this->entity->lingotek['vault_id'];
  }
  
  public function getTitle() {
    if (!empty($this->info['entity keys']['label']) && !empty($this->entity->{$this->info['entity keys']['label']})) {
      return $this->entity->{$this->info['entity keys']['label']};
    }
    if ($this->entity_type == 'comment') {
      return $this->entity->subject;
    }
    LingotekLog::info('Did not find a label for @entity_type #!entity_id, using default label.',
        array('@entity_type' => $this->entity_type, '@entity_id' => $this->entity_id));
    return $this->entity_type . " #" . $this->entity_id;
  }
  
  public function getDescription() {
    return $this->getTitle();
  }
  
  public function getEntity() {
    return $this->entity;
  }
  
    /**
   * Return the Drupal Entity type
   *
   * @return string
   *   The entity type associated with this object
   */
  public function getEntityType() {
    return $this->entity_type;
  }

  /**
   * Return the node ID
   *
   * @return int
   *   The ID associated with this object
   */
  public function getId() {
    list($id, $vid, $bundle) = lingotek_entity_extract_ids($this->entity_type, $this->entity);
    return $id;
  }
  
  public function getSourceLocale() {
    return Lingotek::convertDrupal2Lingotek($this->entity->language);
  }
  
  public function getDocumentName() {
    return $this->getTitle();
    //return $this->getEntityType() . ' - ' . $this->getId();
  }

  public function getNote() {
    return $this->getTitle();
  }
  
  public function getUrl() {
    global $base_url;
    if ($this->entity_type == 'node' || $this->entity_type == 'comment') {
      $hack = (object) array('language' => ''); // this causes the url function to not prefix the url with the current language the user is viewing the site in
      return $base_url . "/lingotek/view/" . $this->getEntityType() . '/' . $this->getId() . '/{locale}';
    }
    return '';
  }
  
  public function preDownload($lingotek_locale, $completed) {
    if ($completed) {
      lingotek_keystore($this->getEntityType(), $this->getId(), 'target_sync_status_' . $lingotek_locale, LingotekSync::STATUS_READY); 
    }
  }
  
  public function postDownload($lingotek_locale, $completed) {
    $type = $this->getEntityType();
    $entity = $this->getEntity();
    $id = $this->getId();
    
    if ($type == 'node') {
      // clear any caching from entitycache module to allow the new translation to show immediately
      if (module_exists('entitycache')) {
        cache_clear_all($id, 'cache_entity_node');
      }
    }
  }

  public function setStatus($status) {
    $this->setMetadataValue('node_sync_status', $status);
    return $this;
  }

  /**
   * Set the entity's last error in the entity metadata table
   */
  public function setLastError($errors) {
    $this->setMetadataValue('last_sync_error', substr($errors, 0, 255));
    return $this;
  }

  public function setTargetsStatus($status, $lingotek_locale = 'all') {
    if ($lingotek_locale != 'all') {
      $this->setMetadataValue('target_sync_status_' . $lingotek_locale, $status);
    }
    else { // set status for all available targets
      foreach ($this->language_targets as $lt) {
        $this->setMetadataValue('target_sync_status_' . $lt, $status);
      }
    }
    return $this;
  }
}
