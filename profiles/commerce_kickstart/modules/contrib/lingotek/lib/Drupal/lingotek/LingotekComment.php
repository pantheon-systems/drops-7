<?php

/**
 * @file
 * Defines LingotekComment.
 */

/**
 * A class wrapper for Lingotek-specific behavior on Comments.
 */
class LingotekComment implements LingotekTranslatableEntity {
  /**
   * The Drupal entity type associated with this class
   */
  const DRUPAL_ENTITY_TYPE = 'comment';

  /**
   * A Drupal comment.
   *
   * @var object
   */
  protected $comment;

  /**
   * A reference to the Lingotek API.
   *
   * @var LingotekApi
   */
  protected $api = NULL;

  /**
   * A static flag for content updates.
   */
  public static $content_update_in_progress = FALSE;

  /**
   * Constructor.
   *
   * This is private since we want consumers to instantiate via the factory methods.
   *
   * @param $document_id
   *   A Lingotek Document ID.
   */
  private function __construct($comment) {
    $this->comment = $comment;
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
   * Factory method for getting a loaded LingotekComment object.
   *
   * @param object $comment
   *   A Drupal comment.
   *
   * @return LingotekComment
   *   A loaded LingotekComment object.
   */
  public static function load($comment) {
    $comment = new LingotekComment($comment);
    $comment->setApi(LingotekApi::instance());

    return $comment;
  }

  /**
   * Factory method for getting a loaded LingotekComment object.
   *
   * @param int $comment_id
   *   A Drupal comment ID.
   *
   * @return mixed
   *   A loaded LingotekComment object if found, FALSE if the comment could not be loaded.
   */
  public static function loadById($comment_id) {
    $comment = FALSE;
    if ($comment = comment_load($comment_id)) {
      $comment = new LingotekComment($comment);
      $comment->setApi(LingotekApi::instance());
    }

    return $comment;
  }

  /**
   * Loads a LingotekComment by Lingotek Document ID.
   *
   * @param string $lingotek_document_id
   *   The Document ID whose corresponding comment should be loaded.
   * @param string $lingotek_language_code
   *   The language code associated with the Lingotek Document ID.
   * @param int $lingotek_project_id
   *   The Lingotek project ID associated with the Lingotek Document ID.
   *
   * @return mixed
   *   A LingotekComment object on success, FALSE on failure.
   */
  public static function loadByLingotekDocumentId($lingotek_document_id, $source_language_code, $lingotek_project_id) {
    $comment = FALSE;

    // Get all Comments in the system associated with the document ID.
    $query = db_select('lingotek_entity_metadata', 'meta')
      ->fields('meta', array('entity_id'))
      ->condition('entity_key', 'document_id')
      ->condition('entity_type', 'comment')
      ->condition('value', $lingotek_document_id);
    $results = $query->execute();

    $target_entity_ids = array();
    foreach ($results as $result) {
      $target_entity_ids[] = $result->entity_id;
    }

    // Get the results that are associated with the passed Lingotek project ID.
    // Lingotek Document IDs are not unique across projects.
    if (!empty($target_entity_ids)) {
      $in_project_results = db_select('lingotek_entity_metadata', 'meta')
        ->fields('meta', array('entity_id'))
        ->condition('entity_id', $target_entity_ids, 'IN')
        ->condition('entity_key', 'project_id')
        ->condition('entity_type', 'comment')
        ->condition('value', $lingotek_project_id)
        ->execute()
        ->fetchAll();

      if (count($in_project_results)) {
        $comment = self::loadById($in_project_results[0]->entity_id);
      }
    }
    return $comment;
  }


  /**
   * Event handler for updates to the comment's data.
   */
  public function contentUpdated() {

    $metadata = $this->metadata();
    if (empty($metadata['document_id'])) {
      $this->createLingotekDocument();
    }
    else {
      $update_result = $this->updateLingotekDocument();
    }

  }

  /**
   * Creates a Lingotek Document for this comment.
   *
   * @return bool
   *   TRUE if the document create operation was successful, FALSE on error.
   */
  protected function createLingotekDocument() {
    return ($this->api->addContentDocumentWithTargets($this)) ? TRUE : FALSE;
  }

  /**
   * Updates the existing Lingotek Documemnt for this comment.
   *
   * @return bool
   *   TRUE if the document create operation was successful, FALSE on error.
   */
  protected function updateLingotekDocument() {
    $result = $this->api->updateContentDocument($this);
    return ($result) ? TRUE : FALSE;
  }

  /**
   * Gets the local Lingotek metadata for this comment.
   *
   * @return array
   *   An array of key/value data for the current comment.
   */
  protected function metadata() {
    $metadata = array();

    $results = db_select('lingotek_entity_metadata', 'meta')
      ->fields('meta')
      ->condition('entity_id', $this->comment->cid)
      ->condition('entity_type', 'comment')
      ->execute();

    foreach ($results as $result) {
      $metadata[$result->entity_key] = $result->value;
    }

    return $metadata;
  }

  /**
   * Gets the contents of this item formatted as XML that can be sent to Lingotek.
   *
   * @return string
   *   The XML document representing the entity's translatable content.
   */
  public function documentLingotekXML() {
    $translatable = array();

    foreach ($this->comment as $key => $value) {
      $field = field_info_field($key);
      if (isset($field)) {
        array_push($translatable, $key);
      }
    }

    $content = '';
    foreach ($translatable as $field) {
      $language = $this->comment->language;
      if (!array_key_exists($language, $this->comment->$field)) {
        $language = LANGUAGE_NONE;
      }
      $text = $this->comment->$field;
      // Deal with not being initialized right, such as pre-existing titles.
      if (!array_key_exists($language, $this->comment->$field) || !array_key_exists(0, $text[$language])) {
        continue;
      }

      // We may split compound Drupal fields into several Lingotek fields.
      $target_keys = array(
        'value' => '', // Most text fields
        'summary' => 'summary' // "Long text with summary" fields have this sub-field value as well.
      );

      // Create fields from all target keys.
      foreach ($target_keys as $target_key => $element_suffix) {
        if (!empty($text[$language][0][$target_key])) {
          $element_name = $field;
          if (!empty($element_suffix)) {
            $element_name .= '__' . $element_suffix;
          }

          $current_field = '<' . $element_name . '>';

          foreach ($text[$language] as $key => $value) {
            // TODO: This isn't a very robust check for text fields.
            // Switch to using field metadata looking for known text field types?
            if (!array_key_exists('value', $value)) {
              continue;
            }

            $current_field .= '<element><![CDATA[' . $value[$target_key] . ']]></element>' . "\n";
          }

          $current_field .= '</' . $element_name . '>';
          $content .= $current_field . "\n";
        }
      }
    }

    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?><contents>$content</contents>";
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
    return db_select('lingotek_entity_metadata', 'meta')
      ->fields('meta', array('value'))
      ->condition('entity_key', $key)
      ->condition('entity_id', $this->comment->cid)
      ->condition('entity_type', self::DRUPAL_ENTITY_TYPE)
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
      db_insert('lingotek_entity_metadata')
        ->fields(array(
          'entity_id' => $this->comment->cid,
          'entity_type' => self::DRUPAL_ENTITY_TYPE,
          'entity_key' => $key,
          'value' => $value,
        ))
        ->execute();

    }
    else {
      db_update('lingotek_entity_metadata')
        ->fields(array(
          'value' => $value
        ))
        ->condition('entity_id', $this->comment->cid)
        ->condition('entity_type', self::DRUPAL_ENTITY_TYPE)
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
      db_delete('lingotek_entity_metadata')
        ->condition('entity_id', $this->comment->cid)
        ->condition('entity_type', self::DRUPAL_ENTITY_TYPE)
        ->condition('entity_key', $key, 'LIKE')
        ->execute();
    }
  }

  /**
   * Updates the local content with data from a Lingotek Document.
   *
   * @return bool
   *   TRUE if the content updates succeeded, FALSE otherwise.
   */
  public function updateLocalContent() {
    $success = TRUE;
    $metadata = $this->metadata();
    $document_id = $metadata['document_id'];

    if (empty($document_id)) {
      LingotekLog::error('Unable to refresh local contents for comment @cid. Could not find Lingotek Document ID.',
        array('@cid' => $this->comment->cid));
      return FALSE;
    }
  
    $api = LingotekApi::instance();
    $document = $api->getDocument($document_id);
    
    foreach ($document->translationTargets as $target) {
        $this->updateLocalContentByTarget($target->language);
    }

    return $success;
  }
  
  /**
   * Updates the local content of $target_code with data from a Lingotek Document
   *
   * @param string $lingotek_locale
   *   The code for the language that needs to be updated.
   * @return bool
   *   TRUE if the content updates succeeded, FALSE otherwise.
   */
  public function updateLocalContentByTarget($lingotek_locale) {
    $metadata = $this->metadata();
    $document_id = $metadata['document_id'];
    
    if (empty($document_id)) {
      LingotekLog::error('Unable to refresh local contents for comment @cid. Could not find Lingotek Document ID.',
        array('@cid' => $this->comment->cid));
      return FALSE;
    }
    
    $api = LingotekApi::instance();
    $document_xml = $api->downloadDocument($document_id, $lingotek_locale);
    
    $target_language = Lingotek::convertLingotek2Drupal($lingotek_locale);
    foreach ($document_xml as $drupal_field_name => $content) {

      // Figure out which subkey of the field data we're targeting.
      // "value" for standard text fields, or some other key for
      // compound text fields (text with summary, for example).
      $target_key = 'value';
      $subfield_parts = explode('__', $drupal_field_name);
      if (count($subfield_parts) == 2) {
        $drupal_field_name = $subfield_parts[0];
        $target_key = $subfield_parts[1];
      }

      $field = field_info_field($drupal_field_name);
      if (!empty($field)) {
        $comment_field = &$this->comment->$drupal_field_name;
        $index = 0;
        foreach ($content as $text) {
          $comment_field[$target_language][$index][$target_key] = decode_entities($text);

          // Copy filter format from source language field.
          if (!empty($comment_field[$this->comment->language][0]['format'])) {
            $comment_field[$target_language][$index]['format'] = $comment_field[$this->comment->language][0]['format'];
          }
          $index++;
        }
      }
    }

    $comment_node = LingotekNode::loadById($this->comment->nid);
    $comment_fields = array_keys(field_info_instances('comment', 'comment_node_' . $comment_node->type));
    foreach ($comment_fields as $field) {
      // Copy any untranslated fields from the default language into this target.
      if (isset($this->comment->{$field}[$this->comment->language]) && !isset($this->comment->{$field}[$target_language])) {
        $this->comment->{$field}[$target_language] = $this->comment->{$field}[$this->comment->language];
      }

      // Ensure that all fields get their LANGUAGE_NONE field data populated with the
      // comment's default language data, to support toggling off of comment translation
      // at some point in the future.
      if (!empty($this->comment->{$field}[$this->comment->language])) {
        $this->comment->{$field}[LANGUAGE_NONE] = $this->comment->{$field}[$this->comment->language];
      }
    }
    
    // This avoids an infitinite loop when hooks resulting from comment_save() are invoked.
    self::$content_update_in_progress = TRUE;
    comment_save($this->comment);
    self::$content_update_in_progress = FALSE;
    $this->comment = comment_load($this->comment->cid);
    
    return TRUE;
  }  

  /**
   * Gets the Lingotek document ID for this entity.
   *
   * @return mixed
   *   The integer document ID if the entity is associated with a
   *   Lingotek document. FALSE otherwise.
   */
  public function lingotekDocumentId() {
    return $this->getMetadataValue('document_id');
  }

  /**
   * Return the Drupal Entity type
   *
   * @return string
   *   The entity type associated with this object
   */
  public function getEntityType() {
    return self::DRUPAL_ENTITY_TYPE;
  }

  /**
   * Return the comment ID
   *
   * @return int
   *   The ID associated with this object
   */
  public function getId() {
    return $this->comment->cid;
  }

  /**
   * Magic get for access to node and node properties.
   */
  public function __get($property_name) {
    $property = NULL;

    if ($property === self::DRUPAL_ENTITY_TYPE) {
      $property = $this->comment;
    }
    elseif (isset($this->comment->$property_name)) {
      $property = $this->comment->$property_name;
    }

    return $property;
  }
  
  public function getWorkflowId() {
    return variable_get('lingotek_translate_comments_workflow_id', '');
  }
  
  public function getProjectId() {
    return variable_get('lingotek_project', '');
  }
  
  public function getVaultId() {
    return variable_get('lingotek_vault', '');
  }
  
  public function getTitle() {
    return $this->comment->title;
  }
  
  public function getDescription() {
    return $this->comment->title;
  }
  
  public function getSourceLocale() {
    return Lingotek::convertDrupal2Lingotek($this->comment->language);
  }
  
}
