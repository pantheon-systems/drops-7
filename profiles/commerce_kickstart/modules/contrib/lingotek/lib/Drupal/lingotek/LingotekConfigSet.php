<?php

/**
 * @file
 * Defines LingotekConfigSet.
 */

/**
 * A class wrapper for Lingotek-specific behavior on ConfigSets.
 */
class LingotekConfigSet implements LingotekTranslatableEntity {
  /**
   * The Drupal entity type associated with this class
   */

  const DRUPAL_ENTITY_TYPE = 'config_set';
  const TAG_PREFIX = 'config_';
  const TAG_PREFIX_LENGTH = 7; // length of 'config_'

  /**
   * The title of the document
   */
  protected $title = NULL;

  /**
   * A Drupal config_set.
   *
   * @var object
   */

  protected $sid;

  /**
   * Array for storing source and target translation strings
   */
  protected $source_data;
  protected $target_data;

  /**
   * A reference to the Lingotek API.
   *
   * @var LingotekApi
   */
  protected $api = NULL;

  /**
   * A static flag for content updates.
   */
  protected static $content_update_in_progress = FALSE;
  protected $workflow_id = null;
  /**
   * Constructor.
   *
   * This is private since we want consumers to instantiate via the factory methods.
   *
   * @param $set_id
   *   A Config Set ID.
   */
  private function __construct($set_id = NULL) {
    $this->sid = $set_id;
    $this->set_size = LINGOTEK_CONFIG_SET_SIZE;
    $this->source_data = self::getAllSegments($this->sid);
    $this->source_meta = self::getSetMeta($this->sid);
    $this->language = language_default();
    // INT-791 Respecting the i18n_string_source_language setting
    $i18n_language = variable_get('i18n_string_source_language', language_default()->language);
    $this->language->language = $i18n_language;
    $this->language->lingotek_locale = Lingotek::convertDrupal2Lingotek($this->language->language);
    $this->language_targets = Lingotek::getLanguagesWithoutSource($this->language->lingotek_locale);
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
   * Return the status of the target locale for the given set_id
   *
   * @param int $set_id
   *   The id of the set to search for
   * @param string $target_locale
   *   The lingotek locale to search for
   *
   * @return string
   *   The status of the target locale for the given set_id
   */
  public static function getTargetStatusById($set_id, $target_locale) {
    $result = db_select('lingotek_config_metadata', 'meta')
        ->fields('meta', array('value'))
        ->condition('id', $set_id)
        ->condition('config_key', 'target_sync_status_' . $target_locale)
        ->execute();
    if ($result && $value = $result->fetchField()) {
      return $value;
    }
    LingotekLog::error('Did not find a target status for set ID "@id"', array('@id' => $set_id));
    return FALSE;
  }

  /**
   * Return the set ID for the current set
   *
   * @return int
   *   The ID of the current set, if it exists, otherwise NULL
   */
  public function getId() {
    return $this->sid;
  }

  /**
   * Return the title for the current set
   *
   * @return string
   *   The title of the current set
   */
  public function getTitle() {
    if ($this->title) {
      return $this->title;
    }
    $this->title = self::getTitleBySetId($this->sid);
    return $this->title;
  }

  /**
   * Set the display name for the document in the TMS
   */
  public function setTitle($title) {
    $this->title = $title;
  }

  /**
   * Return the title for the given set id
   *
   * @return string
   *   The title of the current set
   */
  public static function getTitleBySetId($set_id) {
    $textgroup = db_select('lingotek_config_metadata', 'l')
        ->fields('l', array('value'))
        ->condition('id', $set_id)
        ->condition('config_key', 'textgroup')
        ->execute()
        ->fetchField();

    $all_from_group = db_select('lingotek_config_metadata', 'l')
        ->fields('l', array('id'))
        ->condition('config_key', 'textgroup')
        ->condition('value', $textgroup)
        ->orderBy('id')
        ->execute()
        ->fetchCol();
    $num_in_group = array_search($set_id, $all_from_group);
    $textgroup = ($textgroup == 'default') ? 'Built-in Interface' : $textgroup;
    return ucfirst($textgroup) . ' ' . (1 + $num_in_group);
  }

  /**
   * Return the description for the current set
   *
   * @return string
   *   The description of the current set
   */
  public function getDescription() {
    return $this->getTitle();
  }

  /**
   * Return the set ID for a given segment from the locales source
   *
   * @param int
   *   the lid of a segment from the locales source
   *
   * @return int
   *   the ID of a set of configuration segments
   */
  public static function getSetId($lid, $assign = TRUE) {
    // Check if the lid already has a set:
    $existing_sid = db_select('lingotek_config_map', 'l')
        ->fields('l', array('set_id'))
        ->condition('lid', $lid)
        ->execute()
        ->fetchField();
    // If not, assign one to it
    if (!$existing_sid && $assign) {
      $new_sid = self::assignSetId($lid);
      return $new_sid;
    }
    return $existing_sid;
  }

  public static function bulkGetSetId($lid_map){
    $set_ids = array();
    foreach($lid_map as $textgroup => $lids){
      $open_set_id = self::getOpenSet($textgroup);
      if ($open_set_id === FALSE) {
        $open_set_id = self::createSet($textgroup);
      }

      $query = db_select('lingotek_config_map', 'lcm');
      $query->addField('lcm', 'set_id');
      $query->addField('lcm', 'lid');
      $query->condition('lid', $lids, "IN");
      $result = $query->execute()->fetchAllAssoc('lid');

      $insert = db_insert('lingotek_config_map');
      $insert->fields(array('lid','set_id'));

      $count = 0;
      foreach($lids as $lid){
        if($count === LINGOTEK_CONFIG_SET_SIZE){
          $open_set_id = self::createSet($textgroup);
          $count = 0;
        }
        if(!isset($result[$lid])){
          $insert->values(array('lid'=>$lid,'set_id'=>$open_set_id));
          $set_ids[$open_set_id] = $open_set_id;
        }
        else {
          $set_ids[$result[$lid]->set_id] = $result[$lid]->set_id;
        }
        $count++;
      }
      $insert->execute();
    }
    return $set_ids;
  }

  protected static function assignSetId($lid) {
    // get the $lid's textgroup
    $textgroup = db_select('locales_source', 'l')
        ->fields('l', array('textgroup'))
        ->condition('lid', $lid)
        ->execute()
        ->fetchField();

    $open_set_id = self::getOpenSet($textgroup);
    if ($open_set_id === FALSE) {
      $open_set_id = self::createSet($textgroup);
    }
    // assign lid to that set
    db_merge('lingotek_config_map')
        ->key(array('lid' => $lid))
        ->fields(array(
          'lid' => $lid,
          'set_id' => $open_set_id,
        ))
        ->execute();
    return $open_set_id;
  }

  protected static function getOpenSet($textgroup) {
    $full_sets = self::getFullSets();

    $query = db_select('lingotek_config_metadata', 'l')
        ->fields('l', array('id'))
        ->condition('config_key', 'textgroup')
        ->condition('value', $textgroup);
    if (!empty($full_sets)) {
      $query->condition('id', $full_sets, 'NOT IN');
    }
    $query->orderBy('id');
    $result = $query->execute();
    $set_ids = $result->fetchCol();
    $open_set_id = FALSE;
    foreach ($set_ids as $key => $set_id) {
      if (!self::hasMaxChars($set_id)) {
        $open_set_id = $set_ids[$key];
        break;
      }
    }
    return $open_set_id;
  }

  protected static function getFullSets() {
    $query = db_query('SELECT set_id, COUNT(*) c FROM {lingotek_config_map} GROUP BY set_id HAVING c >= :max_size', array(':max_size' => LINGOTEK_CONFIG_SET_SIZE));

    $full_sets = $query->fetchCol();
    return $full_sets;
  }

  protected static function hasMaxChars($set_id) {
    $lids = self::getLidsFromSets($set_id);
    if (!empty($lids)) {
      $query = db_select('locales_source', 'ls')
          ->fields('ls', array('source'))
          ->condition('lid', $lids, 'IN')
          ->execute();
      $strings_array = $query->fetchCol();
      $strings = implode('', $strings_array);
      if (strlen($strings) > LINGOTEK_CONFIG_MAX_SOURCE_LENGTH) {
        return TRUE;
      }
    }
    return FALSE;
  }

  protected static function createSet($textgroup) {
    $timestamp = time();
    $next_id = self::getNextSetId();
    db_insert('lingotek_config_metadata')
        ->fields(array(
          'id' => $next_id,
          'config_key' => 'textgroup',
          'value' => $textgroup,
          'created' => $timestamp,
          'modified' => $timestamp
        ))
        ->execute();

    return $next_id;
  }

  protected static function getNextSetId() {
    $query = db_query('SELECT max(id) FROM lingotek_config_metadata WHERE id < :max_size', array(':max_size' => LingotekSync::MARKED_OFFSET));
    $max_set_id = $query->fetchField();

    if ($max_set_id) {
      return (int) $max_set_id + 1;
    }
    return 1;
  }

  public static function getDocId($set_id) {
    $doc_id = db_select('lingotek_config_metadata', 'l')
        ->fields('l', array('value'))
        ->condition('id', $set_id)
        ->condition('config_key', 'document_id')
        ->execute()
        ->fetchField();
    return $doc_id;
  }

  public static function getAllConfigDocIds() {
    $doc_ids = db_select('lingotek_config_metadata', 'l')
        ->fields('l', array('value'))
        ->condition('config_key', 'document_id')
        ->execute()
        ->fetchCol();
    return $doc_ids;
  }

  public static function getAllUnsetWorkflowConfigDocIds() {
    $setWorkflowSetIds = db_select('lingotek_config_metadata', 'lcm')
        ->fields('lcm', array('id'))
        ->condition('config_key', 'workflow_id');
    $doc_ids = db_select('lingotek_config_metadata', 'l')
        ->fields('l', array('value'))
        ->condition('config_key', 'document_id')
        ->condition('id', $setWorkflowSetIds, 'NOT IN')
        ->execute()
        ->fetchCol();
    return $doc_ids;
  }

  public static function deleteConfigSetWorkflowIds(){
    db_delete('lingotek_config_metadata')
        ->condition('config_key', 'workflow_id', '=')
        ->execute();
  }

  public function getSourceLocale() {
    return $this->language->lingotek_locale;
  }

  /**
   * Set all segments for a given set ID to CURRENT status
   *
   * @param int
   *   the ID of a set of configuration segments
   */
  public static function setSegmentStatusToCurrentById($set_id) {
    $lids = self::getLidsFromSets($set_id);
    $result = db_update('locales_target')
        ->fields(array('i18n_status' => I18N_STRING_STATUS_CURRENT))
        ->condition('lid', $lids, 'IN')
        ->execute();
  }

  /**
   * Save segment's translation for the given target
   *
   * @param int
   *   the lid of the segment being translated
   * @param string
   *   the 2-digit language code for the target language
   * @param string
   *   the translated content to be saved
   */
  public static function saveSegmentTranslation($lid, $target_language, $content) {
    // insert/update translations, overwriting everything that is there
    // except for the i18n_status field, which should preserve its
    // currently-set flags, and the plid and plural fields which just
    // take default values for now.
    db_merge('locales_target')
        ->key(array('lid' => $lid, 'language' => $target_language,))
        ->fields(array(
          'lid' => $lid,
          'translation' => $content,
          'language' => $target_language,
        ))
        ->execute();
  }

  /**
    /**
   * Return the set ID for a given Lingotek document ID, if it exists
   *
   * @param int
   *   the id of a lingotek document
   *
   * @return int
   *   the ID of a set of configuration segments
   */
  public static function getIdByDocId($doc_id) {
    $query = db_select('lingotek_config_metadata', 'meta');
    $query->fields('meta', array('id'));
    $query->condition('config_key', 'document_id');
    $query->condition('value', $doc_id);
    $set_id = $query->execute()->fetchField();
    if ($set_id !== FALSE) {
      return $set_id;
    }
    return FALSE;
  }

  /**
   * Factory method for getting a loaded LingotekConfigSet object.
   *
   * @param object $config_set
   *   A Drupal config_set.
   *
   * @return LingotekConfigSet
   *   A loaded LingotekConfigSet object.
   */
  public static function load($config_set) {
    // WTD: not sure how to build this yet, so just raise NYI for now...
    throw new Exception('Not yet implemented');
  }

  /**
   * Factory method for getting a loaded LingotekConfigSet object.
   *
   * @param int $set_id
   *   A Drupal config set ID.
   *
   * @return mixed
   *   A loaded LingotekConfigSet object if found, FALSE if the set could not be loaded.
   */
  public static function loadById($set_id) {
    $set = FALSE;
    // get any segments that should be associated with this set
    // if segments exist, return a LingotekConfigSet instance
    // otherwise, return FALSE
    $set_segments = self::getLidsFromSets($set_id);
    if ($set_segments) {
      $set = new LingotekConfigSet($set_id);
      $set->setApi(LingotekApi::instance());
    }

    return $set;
  }

  /**
   * Return all segments from the database that belong to a given set ID
   *
   * @param int $set_id
   *
   * @return array
   *   An array containing the translation sources from the locales_source table
   */
  protected static function getAllSegments($set_id) {
    $max_length = variable_get('lingotek_config_max_source_length', LINGOTEK_CONFIG_MAX_SOURCE_LENGTH); //is this just to make sure there are no enormous config items or is there another reason? How often does this come into play?

    $lids = self::getLidsFromSets($set_id);
    if (empty($lids)) {
      return $lids;
    }

    $results = db_select('locales_source', 'ls')
        ->fields('ls', array('lid', 'source'))
        ->condition('lid', $lids, 'IN')
        ->orderBy('lid')
        ->execute();

    $response = array();
    while ($r = $results->fetchAssoc()) {
      if(strlen($r['source']) < $max_length) {
        $response[$r['lid']] = $r['source'];
      }
      else {
        LingotekLog::warning("Config item @id was not sent to Lingotek for translation because it exceeds the max length of @max_length characters.", array('@id' => $r['lid'], '@max_length' => $max_length));
        // Remove it from the set in the config_map table so it doesn't get marked as uploaded or translated.
        self::disassociateSegments($r['lid']);
      }
    }

    return $response;
  }

  public static function getLidsFromSets($set_ids) {
    $set_ids = is_array($set_ids) ? $set_ids : array($set_ids);
    if (empty($set_ids)) {
      return array();
    }
    $lids = db_select('lingotek_config_map', 'lcm')
        ->fields('lcm', array('lid'))
        ->condition('set_id', $set_ids, 'IN')
        ->execute()
        ->fetchCol();
    return $lids;
  }

  protected static function getLidsForTextgroup($textgroup) {
    $query = db_select('locales_source', 'ls')
        ->fields('ls', array('lid'))
        ->condition('textgroup', $textgroup)
        ->execute();

    $lids = $query->fetchCol();
    return $lids;
  }

  public static function getLidsByStatus($status) {
    $target_language_search = '%';
    $query = db_select('lingotek_config_metadata', 'l');
    $query->fields('l', array('id'));
    $query->condition('config_key', 'target_sync_status_' . $target_language_search, 'LIKE');
    $query->condition('value', $status);
    $result = $query->execute();
    $set_ids = $result->fetchCol(); //$result->fetchAllAssoc('nid');

    $lids = self::getLidsFromSets($set_ids);
    return $lids;
  }
  
  public static function getSetIdsByStatus($status, $lids = null) {
    $query = db_select('lingotek_config_metadata', 'l');
    if($lids !== null) {
      $query->join('lingotek_config_map', 'lc', 'l.id = lc.set_id');
      $query->condition('lc.lid', $lids, 'IN');
    }
    $query->fields('l', array('id'));
    $query->condition('l.config_key', 'target_sync_status_%', 'LIKE');
    $query->condition('l.value', $status);
    $query->distinct();
    $result = $query->execute();
    $set_ids = $result->fetchCol();
    return $set_ids;
  }

  /**
   * Return any metadata for the given set ID, if it exists
   *
   * @param int $set_id
   *
   * @return array
   *   An array containing anything for the set_id from table lingotek_config_metadata
   */
  protected static function getSetMeta($set_id) {
    $query = db_select('lingotek_config_metadata', 'l');
    $query->fields('l', array('id', 'config_key', 'value'));
    $query->condition('l.id', $set_id);
    $result = $query->execute();
    $response = array();
    while ($record = $result->fetch()) {
      $response[$record->config_key] = $record->value;
    }
    return $response;
  }

  /**
   * Loads a LingotekConfigSet by Lingotek Document ID.
   *
   * @param string $lingotek_document_id
   *   The Document ID whose corresponding set should be loaded.
   * @param string $lingotek_language_code
   *   The language code associated with the Lingotek Document ID.
   * @param int $lingotek_project_id
   *   The Lingotek project ID associated with the Lingotek Document ID.
   *
   * @return mixed
   *   A LingotekConfigSet object on success, FALSE on failure.
   */
  public static function loadByLingotekDocumentId($lingotek_document_id) {
    $set = FALSE;

    // Get the set entries in the system associated with the document ID.
    $query = db_select('lingotek_config_metadata', 'meta')
        ->fields('meta', array('id'))
        ->condition('config_key', 'document_id')
        ->condition('value', $lingotek_document_id)
        ->execute();
    $set_id = $query->fetchField();

    // this returns a 0 for the first id then the if shows false
    if (isset($set_id)) {
      $set = self::loadById($set_id);
    }

    return $set;
  }

  /**
   * Gets the local Lingotek metadata for this config set.
   *
   * @return array
   *   An array of key/value data for the current config set.
   */
  protected function metadata() {
    $metadata = array();

    $results = db_select('lingotek_config_metadata', 'meta')
        ->fields('meta')
        ->condition('id', $this->sid)
        ->execute();

    foreach ($results as $result) {
      $metadata[$result->config_key] = $result->value;
    }

    return $metadata;
  }

  /**
   * Return true if current set already has an assigned Lingotek Document ID
   *
   * @return boolean TRUE/FALSE
   */
  public function hasLingotekDocId() {
    $has_id = array_key_exists('document_id', $this->source_meta);
    if ($has_id && (strlen($this->source_meta['document_id']) > 0)) {
      return $this->source_meta['document_id'];
    }
    return FALSE;
  }

  /**
   * Get the set's target translation status for the given locale
   */
  public function getTargetStatus($lingotek_locale) {
    $result = db_select('lingotek_config_metadata', 'meta')
        ->fields('meta', array('value'))
        ->condition('id', $this->sid)
        ->condition('config_key', 'target_sync_status_' . $lingotek_locale)
        ->execute();
    return $result->value;
  }

  /**
   * Assign the set's document ID in the config metadata table
   */
  public function setDocumentId($doc_id) {
    $this->setMetadataValue('document_id', $doc_id);
    return $this;
  }

  /**
   * Assign the set's project ID in the config metadata table
   */
  public function setProjectId($project_id) {
    $this->setMetadataValue('project_id', $project_id);
    return $this;
  }

  /**
   * Assign the set's status in the config metadata table
   */
  public function setStatus($status) {
    $this->setMetadataValue('upload_status', $status);
    return $this;
  }

  /**
   * Assign the set's last sync error in the config metadata table
   */
  public function setLastError($errors) {
    $this->setMetadataValue('last_sync_error', substr($errors, 0, 255));
    return $this;
  }

  /**
   * Assign the set's last upload_error in the config metadata table
   */
  public function setUploadError($errors) {
    $this->setMetadataValue('upload_error', substr($errors, 0, 255));
    return $this;
  }

  /**
   * Delete the set's last error in the config metadata table
   */
  public function deleteLastError() {
    $this->deleteMetadataValue('last_sync_error');
    return $this;
  }

  /**
   * Delete the set's last upload_error in the config metadata table
   */
  public function deleteUploadError() {
    $this->deleteMetadataValue('upload_error');
    return $this;
  }

  /**
   * Assign the set's target status(es) in the config metadata table
   */
  public function setTargetsStatus($status, $lingotek_locale = NULL) {
    if (is_array($lingotek_locale)) {
      foreach ($lingotek_locale as $ll) {
        $this->setMetadataValue('target_sync_status_' . $ll, $status);
      }
    }
    elseif (is_string($lingotek_locale) && !empty($lingotek_locale)) {
      $this->setMetadataValue('target_sync_status_' . $lingotek_locale, $status);
    }
    else { // set status for all available targets
      foreach ($this->language_targets as $lt) {
        $this->setMetadataValue('target_sync_status_' . $lt, $status);
      }
    }
    return $this;
  }

  /**
   * Gets the contents of this item formatted as XML to be sent to Lingotek.
   *
   * @return string
   *   The XML document representing the entity's translatable content.
   */
  public function documentLingotekXML() {
    $translatable = array();

    // for now, assume all strings in locales_source table for the given set are translatable
    if (TRUE) {
      $translatable = array_keys($this->source_data);
    }

    $content = '';
    foreach ($translatable as $field) {
      $field_data = $this->source_data[$field];
      if (variable_get('lingotek_config_encode_ampersand', FALSE)) {
        $field_data = str_replace("&", "&amp;", $field_data);
      }
      $text = lingotek_filter_placeholders($field_data, TRUE);

      if ($text) {
        $current_field = '<' . self::TAG_PREFIX . $field . '>';
        $current_field .= '<element><![CDATA[' . $text . ']]></element>' . "\n";
        $current_field .= '</' . self::TAG_PREFIX . $field . '>';
        $content .= $current_field . "\n";
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
    return db_select('lingotek_config_metadata', 'meta')
            ->fields('meta', array('value'))
            ->condition('config_key', $key)
            ->condition('id', $this->sid)
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
    $timestamp = time();
    if (!isset($metadata[$key])) {
      db_insert('lingotek_config_metadata')
          ->fields(array(
            'id' => $this->sid,
            'config_key' => $key,
            'value' => $value,
            'created' => $timestamp,
            'modified' => $timestamp
          ))
          ->execute();
    }
    else {
      db_update('lingotek_config_metadata')
          ->fields(array(
            'value' => $value,
            'modified' => $timestamp
          ))
          ->condition('id', $this->sid)
          ->condition('config_key', $key)
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
      db_delete('lingotek_config_metadata')
          ->condition('id', $this->sid)
        ->condition('config_key', $key, 'LIKE')
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
    $metadata = $this->metadata();
    $document_id = $metadata['document_id'];

    if (empty($document_id)) {
      LingotekLog::error('Unable to refresh local contents for config set @sid. Could not find Lingotek Document ID.', array('@sid' => $this->sid));
      return FALSE;
    }

    $api = LingotekApi::instance();
    $document = $api->getDocument($document_id);

    foreach ($document->translationTargets as $target) {
      $this->downloadTriggered($target->lingotek_locale);
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
    $metadata = $this->metadata();
    $document_id = $metadata['document_id'];

    if (empty($document_id)) {
      LingotekLog::error('Unable to refresh local contents for config set @sid. Could not find Lingotek Document ID.', array('@sid' => $this->sid));
      return FALSE;
    }

    $api = LingotekApi::instance();
    $document_xml = $api->downloadDocument($document_id, $lingotek_locale);
    $target_language = Lingotek::convertLingotek2Drupal($lingotek_locale);

    /* FAST VERSION (git history for slow version) */
    // 1. save the dirty targets associated with given language
    $dirty_lids = self::getDirtyLidsBySetIdAndLanguage($this->sid, $target_language);
    // 2. delete all segment targets associated with given language
    self::deleteSegmentTranslationsBySetIdAndLanguage($this->sid, $target_language);
    // 3. insert all segments for the given language
    self::saveSegmentTranslations($document_xml, $target_language);
    // 4. return the dirty targets' statuses
    self::restoreDirtyLids($dirty_lids);
    /* END FAST */

    // assign set status to current
    $this->setStatus(LingotekSync::STATUS_CURRENT);
    $this->setTargetsStatus(LingotekSync::STATUS_CURRENT, $lingotek_locale);
    self::markSetsCurrent($this->sid);

    return TRUE;
  }

  /**
   * Return all target segments by ID marked to be updated
   *
   * This is a preparatory step before resetting the locales targets for a given
   * set.
   *
   * @param int
   *    the ID of the set under which to search
   * @param string
   *    the language code for which to get the segments that need updating
   */
  public static function getDirtyLidsBySetIdAndLanguage($set_id, $language) {
    $lids = self::getLidsFromSets($set_id);
    $result = db_select('locales_target', 'lt')
        ->fields('lt', array('lid'))
        ->condition('lid', $lids, 'IN')
        ->condition('language', $language)
        ->condition('i18n_status', I18N_STRING_STATUS_CURRENT, '!=')
        ->condition('translation_agent_id', self::getLingotekTranslationAgentId())
        ->execute();
    return $result->fetchCol();
  }

  public static function getEditedLidsInSets($set_ids) {
    $set_ids = is_array($set_ids) ? $set_ids : array($set_ids);
    $query = db_select('lingotek_config_map', 'lcm')
        ->fields('lcm', array('lid'))
        ->condition('set_id', $set_ids, 'IN')
        ->condition('current', 1);
    $query->join('locales_target', 'lt', 'lt.lid = lcm.lid');
    $query->condition('i18n_status', 1);
    $result = $query->execute()->fetchCol();
    $edited_lids = array_unique($result);
    return $edited_lids;
  }

  /**
   * Mark as dirty all target segments passed, in the locales targets
   *
   * @param array $dirty_lids
   *    the list of segments that need updating
   */
  public static function restoreDirtyLids($dirty_lids) {
    if ($dirty_lids) {
      db_update('locales_target')
          ->fields(array('i18n_status' => 1))
          ->condition('lid', $dirty_lids, 'IN')
          ->execute();
    }
  }

  /**
   * Return bool if a specific lid is current
   *
   * @param int $lid
   *    a single lid
   */
  public static function isLidCurrent($lid) {
    $query = db_select('lingotek_config_map', 'lcm')
        ->fields('lcm', array('current'))
        ->condition('lid', $lid)
        ->execute();
    $current = $query->fetchField();
    return $current;
  }

  /**
   * Mark all lids passed as current or not current, in the lingotek_config_map table
   *
   * @param array $lids
   *    the list of lids that are current
   */
  public static function markLidsNotCurrent($lids) {
    $query = db_update('lingotek_config_map')
        ->fields(array('current' => 0));
    if ($lids != 'all') {
      $query->condition('lid', $lids, 'IN');
    }
    $query->execute();
  }

  /**
   * Mark all sets passed as current, in the lingotek_config_map table
   *
   * @param array $set_ids
   *    the list of lids that are current
   */
  protected static function markSetsCurrent($set_ids) {
    $query = db_update('lingotek_config_map')
        ->fields(array('current' => 1));
    if ($set_ids != 'all') {
      $set_ids = is_array($set_ids) ? $set_ids : array($set_ids);
      $query->condition('set_id', $set_ids, 'IN');
    }
    $query->execute();
  }

  /**
   * Get all lids marked as current or not, in the lingotek_config_map table
   *
   * @param int $current
   *    1 to get lids of all current segments, 0 to get lids for segments that are not current
   * @param array $lids
   *    a subset of lids to check, defaults to look for all current segments
   */
  public static function getLidsToUpdate($current = 0, $lids = 'all') {
    $textgroups = array_merge(array(-1), LingotekConfigSet::getTextgroupsForTranslation());

    $query = db_select('lingotek_config_map', 'lcm')
        ->fields('lcm', array('lid'));
    if ($lids !== 'all') {
      $query->condition('lcm.lid', $lids, 'IN');
    }
    $query->join('locales_source', 'ls', "lcm.lid = ls.lid");
    $query->addField('ls', 'textgroup');
    $query->condition('ls.textgroup', $textgroups, 'IN');

    $query->join('locales_target', 'lt', "lcm.lid = lt.lid");
    $or = db_or();
    $or->condition('lcm.current', $current);
    $or->condition('lt.i18n_status', 1);
    $query->condition($or);
    
    $results = $query->execute();
    $lids = array();
    foreach($results as $result){
      $lids[$result->textgroup][$result->lid] = $result->lid; 
    }
    return $lids;
  }
  /**
   * Check a given list for lids that have never been uploaded
   * @param type $control_list
   *  The list of lids to search through
   * @return type
   */
  public static function findNeverUploadedLids($control_list = NULL){
    if($control_list !== NULL && !empty($control_list)){
        $query = db_select('locales_source','ls');
        $query->leftJoin('locales_target','lt','ls.lid = lt.lid');
        $query->isNull("lt.lid");
        $query->addField('ls', 'lid');
        $query->addField('ls',"textgroup");
        $query->condition('ls.lid',$control_list, 'IN');
        $never_uploaded_lids = $query->execute();
        $textgroup_lid = array();
        foreach($never_uploaded_lids as $lid){
          $textgroup_lid[$lid->textgroup][$lid->lid] = $lid->lid; 
        }
        return $textgroup_lid;
    }
    return array();
  }
  /**
   * Delete all target segments for a given set
   *
   * @param int
   *    the ID of the set for which to delete target segments
   * @param string
   *    the language code for which to delete target segments
   */
  public static function deleteSegmentTranslationsBySetIdAndLanguage($set_id, $target_language) {
    $lids = self::getLidsFromSets($set_id);
    db_delete('locales_target')
        ->condition('language', $target_language)
        ->condition('lid', $lids, 'IN')
        ->condition('translation_agent_id', self::getLingotekTranslationAgentId())
        ->execute();
  }

  public static function deleteSegmentTranslations($lids) {
    $lids = is_array($lids) ? $lids : array($lids);
    db_delete('locales_target')
        ->condition('lid', $lids, 'IN')
        ->execute();

    db_delete('lingotek_config_map')
        ->condition('lid', $lids, 'IN')
        ->execute();
  }

  public static function disassociateSegments($lids) {
    $lids = is_array($lids) ? $lids : array($lids);
    db_delete('lingotek_config_map')
        ->condition('lid', $lids, 'IN')
        ->execute();
  }

  public static function deleteConfigSetMetadataBySetId($set_ids) {
    db_delete('lingotek_config_metadata')
      ->condition('id', $set_ids)
      ->execute();
  }

  public static function deleteConfigSetMapDataBySetId($set_ids) {
    db_delete('lingotek_config_map')
      ->condition('set_id', $set_ids)
      ->execute();
  }

  public static function removeEmptyConfigSets($set_ids) {
    foreach($set_ids as $set_id) {
      $set_members = db_select('lingotek_config_map', 'lcm')
        ->fields('lcm', array('lid'))
        ->condition('set_id', $set_ids, 'IN')
        ->execute()
        ->fetchAll();

      if (empty($set_members)) { // The set is empty, so delete it.
        db_delete('lingotek_config_metadata')
          ->condition('id', $set_id)
          ->execute();
      }
    }
  }

  /**
   * Get lingotek translation agent ID
   */
  public static function getLingotekTranslationAgentId() {
    $result = db_select('lingotek_translation_agent', 'lta')
        ->fields('lta', array('id'))
        ->condition('name', 'Lingotek')
        ->execute();
    return $result->fetchField();
  }

  /**
   * Get all locales target entries that were not created by Lingotek
   */
  protected static function getNonLingotekLocalesTargets($document_xml, $target_language) {
    $lids = array(-1); // seed lids for proper query handling on empty case
    foreach ($document_xml as $drupal_field_name => $xml_obj) {
      $lids[] = self::getLidFromTag($drupal_field_name);
    }
    $result = db_select('locales_target', 'lt')
        ->fields('lt', array('lid'))
        ->condition('lid', $lids, 'IN')
        ->condition('language', $target_language)
        ->condition('translation_agent_id', self::getLingotekTranslationAgentId(), '!=')
        ->execute();
    return $result->fetchCol();
  }

  /**
   * Save segment target translations for the given language
   *
   * @param obj
   *    the LingotekXMLElement object containing the translations to be saved
   * @param string
   *    the language code under which to save the translations
   */
  public static function saveSegmentTranslations($document_xml, $target_language) {
    $non_lingotek_locales_targets = self::getNonLingotekLocalesTargets($document_xml, $target_language);
    $plural_mapping = variable_get('lingotek_config_plural_mapping', array());
    $rows = array();
    $sql = 'INSERT INTO {locales_target} (lid, translation, language, plid, plural, translation_agent_id) VALUES ';
    $subsql = '';
    $icount = 0;
    $lingotek_agent = self::getLingotekTranslationAgentId();
    foreach ($document_xml as $drupal_field_name => $xml_obj) {
      $lid = self::getLidFromTag($drupal_field_name);
      if (!in_array($lid, $non_lingotek_locales_targets)) {
        $content = (string) $xml_obj->element;
        $content = lingotek_unfilter_placeholders(decode_entities($content));
        $plural_lid = array_key_exists($lid, $plural_mapping);
        $rows += array(
          ":l_$icount" => $lid,
          ":c_$icount" => $content,
          ":lang_$icount" => $target_language,
          ":plid_$icount" => ($plural_lid ? $plural_mapping[$lid]['plid'] : 0),
          ":plural_$icount" => ($plural_lid ? $plural_mapping[$lid]['plural'] : 0),
          ":agent_$icount" => $lingotek_agent,
          );
        $subsql .= "( :l_$icount, :c_$icount, :lang_$icount, :plid_$icount, :plural_$icount, :agent_$icount),";
        $icount++;
      }
    }
    if (!empty($rows)) {
      $subsql = rtrim($subsql, ',');
      db_query($sql . $subsql, $rows);
    }
  }

  /**
   * Get the Lingotek document ID for this entity.
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
   * Magic get for access to set and set properties.
   */
  public function __get($property_name) {
    $property = NULL;

    if ($property === self::DRUPAL_ENTITY_TYPE) {
      $property = $this;
    }
    elseif (isset($this->$property_name)) {
      $property = $this->$property_name;
    }

    return $property;
  }

  /**
   * Return the lid for locales source/target tables from the XML tag name
   */
  protected static function getLidFromTag($tag) {
    // for now, remove the 'config_' as quickly as possible
    return substr($tag, self::TAG_PREFIX_LENGTH);
  }

  public function getWorkflowId() {
    if($this->workflow_id !== null){
      $workflow_id = $this->workflow_id;
    }
    else {
      $profiles = variable_get('lingotek_profiles');
      $config_profile = $profiles[LingotekSync::PROFILE_CONFIG];
      $workflow_id = array_key_exists('workflow_id', $config_profile) ? $config_profile['workflow_id'] : variable_get('lingotek_translate_config_workflow_id', '');
    }
    return $workflow_id;
  }
  public function setWorkflowId($workflow_id) {
    $this->workflow_id = $workflow_id;
  }
  public function getProjectId() {
    $profiles = variable_get('lingotek_profiles');
    $config_profile = $profiles[LingotekSync::PROFILE_CONFIG];
    $project_id = array_key_exists('project_id', $config_profile) ? $config_profile['project_id'] : variable_get('lingotek_project', '');
    return $project_id;
  }

   public function getVaultId() {
    $profiles = variable_get('lingotek_profiles');
    $config_profile = $profiles[LingotekSync::PROFILE_CONFIG];
    $vault_id = array_key_exists('vault_id', $config_profile) ? $config_profile['vault_id'] : variable_get('lingotek_vault', '');
    return $vault_id;
  }

  /**
   * Return all textgroups from locales_source for which translation is desired
   */
  public static function getTextgroupsForTranslation() {
    $textgroups = array();
    if (variable_get('lingotek_translate_config_builtins', 0)) {
      $textgroups[] = 'default';
    }
    if (variable_get('lingotek_translate_config_menus', 0)) {
      $textgroups[] = 'menu';
    }
    if (variable_get('lingotek_translate_config_taxonomies', 0)) {
      $textgroups[] = 'taxonomy';
    }
    if (variable_get('lingotek_translate_config_blocks', 0)) {
      $textgroups[] = 'blocks';
    }
    if (variable_get('lingotek_translate_config_views', 0)) {
      $textgroups[] = 'views';
    }
    if (variable_get('lingotek_translate_config_fields', 0)) {
      $textgroups[] = 'field';
    }
    if (variable_get('lingotek_translate_config_webform', 0)) {
      $textgroups[] = 'webform';
    }
    if (variable_get('lingotek_translate_config_misc', 0)) {
      $textgroups[] = 'misc';
    }
    return $textgroups;
  }

  public static function getLidBySource($source_string) {
    return db_select('locales_source', 's')
            ->fields('s', array('lid'))
        ->condition('s.source', $source_string)
        ->execute()
        ->fetchField();
  }

/*
 * Return all document IDs related to config translation
 */

  public static function getAllDocumentIds() {
  $result = db_select('lingotek_config_metadata', 'c')
        ->fields('c', array('value'))
      ->condition('c.config_key', 'document_id')
      ->execute();
  return $result->fetchCol();
}


  public function getDocumentName() {
    return $this->getTitle();
  }

  public function getUrl() {
    return '';
  }

  public function getNote() {
    return '';
  }

  public function preDownload($lingotek_locale, $completed) {
    // If auto download is turned off, you need to uncomment these lines and set status to READY.
    /* if ($completed) {
      $this->setTargetsStatus(LingotekSync::STATUS_READY, $lingotek_locale);
      // The following lines mark the whole set as ready rather than just the changed items.
      $lids = array_keys(self::getAllSegments($set_id));
      self::markLidsNotCurrent($lids)
    } */
  }

  public function postDownload($lingotek_locale, $completed) {
  }
}
