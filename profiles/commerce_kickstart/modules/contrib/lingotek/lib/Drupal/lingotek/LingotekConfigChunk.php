<?php

/**
 * @file
 * Defines LingotekConfigChunk.
 */

/**
 * A class wrapper for Lingotek-specific behavior on ConfigChunks.
 */
class LingotekConfigChunk implements LingotekTranslatableEntity {
  /**
   * The Drupal entity type associated with this class
   */

  const DRUPAL_ENTITY_TYPE = 'config_chunk';
  const TAG_PREFIX = 'config_';
  const TAG_PREFIX_LENGTH = 7; // length of 'config_'

  /**
   * A Drupal config_chunk.
   *
   * @var object
   */

  protected $cid;

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

  /**
   * Constructor.
   *
   * This is private since we want consumers to instantiate via the factory methods.
   *
   * @param $chunk_id
   *   A Config Chunk ID.
   */
  private function __construct($chunk_id = NULL) {
    $this->cid = $chunk_id;
    $this->chunk_size = LINGOTEK_CONFIG_CHUNK_SIZE;
    $this->source_data = self::getAllSegments($this->cid);
    $this->source_meta = self::getChunkMeta($this->cid);
    $this->language = language_default();
    if (!isset($this->language->lingotek_locale)) { // if Drupal variable 'language_default' does not exist
      $this->language->lingotek_locale = Lingotek::convertDrupal2Lingotek($this->language->language);
    }
    $this->language_targets = Lingotek::getLanguagesWithoutSource($this->language->lingotek_locale);
    $this->min_lid = $this->getMinLid();
    $this->max_lid = $this->getMaxLid();
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
   * Return the status of the target locale for the given chunk_id
   *
   * @param int $chunk_id
   *   The id of the chunk to search for
   * @param string $target_locale
   *   The lingotek locale to search for
   *
   * @return string
   *   The status of the target locale for the given chunk_id
   */
  public static function getTargetStatusById($chunk_id, $target_locale) {
    $result = db_select('{lingotek_config_metadata}', 'meta')
        ->fields('meta', array('value'))
        ->condition('id', $chunk_id)
        ->condition('config_key', 'target_sync_status_' . $target_locale)
        ->execute();
    if ($result && $value = $result->fetchField()) {
      return $value;
    }
    LingotekLog::error('Did not find a target status for chunk ID "@id"', array('@id' => $chunk_id));
    return FALSE;
  }

  /**
   * Return the chunk ID for the current chunk
   *
   * @return int
   *   The ID of the current chunk, if it exists, otherwise NULL
   */
  public function getId() {
    return $this->cid;
  }

  /**
   * Return the title for the current chunk
   *
   * @return string
   *   The title of the current chunk
   */
  public function getTitle() {
    return 'Drupal configuration (' . $this->min_lid . '-' . $this->max_lid . ')';
  }

  /**
   * Return the description for the current chunk
   *
   * @return string
   *   The description of the current chunk
   */
  public function getDescription() {
    return 'Drupal configuration (' . $this->min_lid . '-' . $this->max_lid . ')';//'Drupal configuration strings (locales source table) between ' . $this->min_lid . ' and ' . $this->max_lid . '.';
  }

  /**
   * Return the chunk ID for a given segment from the locales source
   *
   * @param int
   *   the lid of a segment from the locales source
   *
   * @return int
   *   the ID of a chunk of configuration segments
   */
  public static function getIdBySegment($lid) {
    return intval($lid / LINGOTEK_CONFIG_CHUNK_SIZE) + 1;
  }

  /**
   * Return the segments by lid (from locales source) for a given chunk ID
   *
   * @param int
   *   the ID of a chunk of configuration segments
   *
   * @return array
   *   an array of lids from locales_source
   */
  public static function getSegmentIdsById($chunk_id) {
    $max_length = variable_get('lingotek_config_max_source_length', LINGOTEK_CONFIG_MAX_SOURCE_LENGTH);
    $include_misc_textgroups = FALSE;
    $textgroups_array = self::getTextgroupsForTranslation();
    if (in_array('misc', $textgroups_array)) {
      $include_misc_textgroups = TRUE;
    }
    $textgroups = "-1,'" . implode("','", $textgroups_array) . "'";
    $result = db_query(" SELECT ls.lid
                        FROM {locales_source} ls
                        WHERE ls.lid >= :minLid
                        AND ls.lid <= :maxLid
                        AND LENGTH(ls.source) < :maxLen
                        AND (ls.textgroup IN ($textgroups)
                        ".$include_misc?"OR ls.textgroup NOT IN ('default','blocks','taxonomy','menu','views','field'))":")",
      array(':minLid' => self::minLid($chunk_id),
            ':maxLid' => self::maxLid($chunk_id),
            ':maxLen' => $max_length,
        )
    );
    return $result->fetchCol();
  }
  
  public function getSourceLocale() {
    return $this->language->lingotek_locale;
  }

  /**
   * Set all segments for a given chunk ID to CURRENT status
   *
   * @param int
   *   the ID of a chunk of configuration segments
   */
  public static function setSegmentStatusToCurrentById($chunk_id) {
    $result = db_update('locales_target')
        ->fields(array('i18n_status' => I18N_STRING_STATUS_CURRENT))
        ->condition('lid', self::minLid($chunk_id), '>=')
        ->condition('lid', self::maxLid($chunk_id), '<=')
        ->condition('translation_agent_id', self::getLingotekTranslationAgentId())
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
    db_merge('{locales_target}')
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
   * Return the chunk ID for a given Lingotek document ID, if it exists
   *
   * @param int
   *   the id of a lingotek document
   *
   * @return int
   *   the ID of a chunk of configuration segments
   */
  public static function getIdByDocId($doc_id) {
    $query = db_select('{lingotek_config_metadata}', 'meta');
    $query->fields('meta', array('id'));
    $query->condition('config_key', 'document_id');
    $query->condition('value', $doc_id);
    $result = $query->execute();
    if ($id = $result->fetchField()) {
      return $id;
    }
    return FALSE;
  }

  /**
   * Factory method for getting a loaded LingotekConfigChunk object.
   *
   * @param object $config_chunk
   *   A Drupal config_chunk.
   *
   * @return LingotekConfigChunk
   *   A loaded LingotekConfigChunk object.
   */
  public static function load($config_chunk) {
    // WTD: not sure how to build this yet, so just raise NYI for now...
    throw new Exception('Not yet implemented');
  }

  /**
   * Factory method for getting a loaded LingotekConfigChunk object.
   *
   * @param int $chunk_id
   *   A Drupal config chunk ID.
   *
   * @return mixed
   *   A loaded LingotekConfigChunk object if found, FALSE if the chunk could not be loaded.
   */
  public static function loadById($chunk_id) {
    $chunk = FALSE;
    // get any segments that should be associated with this chunk
    // if segments exist, return a LingotekConfigChunk instance
    // otherwise, return FALSE
    $chunk_segments = self::getAllSegments($chunk_id);

    if ($chunk_segments) {
      $chunk = new LingotekConfigChunk($chunk_id);
      $chunk->setApi(LingotekApi::instance());
    }

    return $chunk;
  }

  /**
   * Return all segments from the database that belong to a given chunk ID
   *
   * @param int $chunk_id
   *
   * @return array
   *   An array containing the translation sources from the locales_source table
   */
  protected static function getAllSegments($chunk_id) {
    $chunk_size = LINGOTEK_CONFIG_CHUNK_SIZE;
    $chunk_min = (intval($chunk_id) - 1) * intval($chunk_size) + 1;
    $chunk_max = (intval($chunk_id) - 1) * intval($chunk_size) + $chunk_size;
    $max_length = variable_get('lingotek_config_max_source_length', LINGOTEK_CONFIG_MAX_SOURCE_LENGTH);
    $textgroups_array = self::getTextgroupsForTranslation();
    $textgroups = "-1,'" . implode("','", $textgroups_array) . "'";

    $query = "SELECT ls.lid, ls.source
      FROM {locales_source} ls
      WHERE ls.lid >= :minLid
      AND ls.lid <= :maxLid
      AND LENGTH(ls.source) < :maxLen
      ";
    if (in_array('misc', $textgroups_array)) {
      $query .= "AND (ls.textgroup IN ($textgroups)
        OR ls.textgroup NOT IN ('default','taxonomy','blocks','menu','views','field'))
        ";
    }
    else {
      $query .= "AND ls.textgroup IN ($textgroups)";
    }

    $results = db_query($query, array(
      ':minLid' => $chunk_min,
      ':maxLid' => $chunk_max,
      ':maxLen' => $max_length,
        )
    );

    $response = array();
    while ($r = $results->fetchAssoc()) {
      $response[$r['lid']] = $r['source'];
    }
    // required to be in order ascending
    ksort($response, SORT_NUMERIC);
    return $response;
  }

  /**
   * Return any metadata for the given chunk ID, if it exists
   *
   * @param int $chunk_id
   *
   * @return array
   *   An array containing anything for the chunk_id from table lingotek_config_metadata
   */
  protected static function getChunkMeta($chunk_id) {
    $query = db_select('{lingotek_config_metadata}', 'l');
    $query->fields('l', array('id', 'config_key', 'value'));
    $query->condition('l.id', $chunk_id);
    $result = $query->execute();
    $response = array();
    while ($record = $result->fetch()) {
      $response[$record->config_key] = $record->value;
    }
    return $response;
  }

  /**
   * Return lid lower limit for the given chunk ID
   *
   * @param int $chunk_id
   *
   * @return int
   *   the lower limit for the given chunk ID
   */
  public static function minLid($chunk_id) {
    return (($chunk_id - 1) * LINGOTEK_CONFIG_CHUNK_SIZE) + 1;
  }

  /**
   * Return lid upper limit for the given chunk ID
   *
   * @param int $chunk_id
   *
   * @return int
   *   the upper limit for the given chunk ID
   */
  public static function maxLid($chunk_id) {
    return $chunk_id * LINGOTEK_CONFIG_CHUNK_SIZE;
  }

  protected function getMinLid() {
    if ($this->chunk_id) {
      return self::minLid($this->chunk_id);
    }
    return (intval(key($this->source_data) / $this->chunk_size) * $this->chunk_size) + 1;
  }

  protected function getMaxLid() {
    if ($this->chunk_id) {
      return self::maxLid($this->chunk_id);
    }
    return (intval(key($this->source_data) / $this->chunk_size) * $this->chunk_size) + $this->chunk_size;
  }

  /**
   * Loads a LingotekConfigChunk by Lingotek Document ID.
   *
   * @param string $lingotek_document_id
   *   The Document ID whose corresponding chunk should be loaded.
   * @param string $lingotek_language_code
   *   The language code associated with the Lingotek Document ID.
   * @param int $lingotek_project_id
   *   The Lingotek project ID associated with the Lingotek Document ID.
   *
   * @return mixed
   *   A LingotekConfigChunk object on success, FALSE on failure.
   */
  public static function loadByLingotekDocumentId($lingotek_document_id) {
    $chunk = FALSE;

    // Get the Chunk entries in the system associated with the document ID.
    $query = db_select('{lingotek_config_metadata}', 'meta')
        ->fields('meta', array('id'))
        ->condition('config_key', 'document_id')
        ->condition('value', $lingotek_document_id)
        ->execute();
    $id = $query->fetchField();

    if ($id) {
      $chunk = self::loadById($id);
    }
    
    return $chunk;
  }

  /**
   * Event handler for updates to the config chunk's data.
   */
  public function contentUpdated() {

    $metadata = $this->metadata();
    if (empty($metadata['document_id'])) {
      $this->createLingotekDocument();
    }
    else {
      $update_result = $this->updateLingotekDocument();
    }

    // Synchronize the local content with the translations from Lingotek.
    // We instruct the users to configure config chunk translation with a
    // single-phase machine translation-only Workflow, so the updated content
    // should be available right after our create/update document calls from above.
    // If it isn't, Lingotek will call us back via LINGOTEK_NOTIFICATIONS_URL
    // when machine translation for the item has finished.
    if (!self::$content_update_in_progress) {
      // Only update the local content if the Document is 100% complete
      // according to Lingotek.

      $document_id = $this->getMetadataValue('document_id');
      if ($document_id) {
        $document = $this->api->getDocument($document_id);
        if (!empty($document->percentComplete) && $document->percentComplete == 100) {
          $this->updateLocalContent();
        }
      }
      else {
        LingotekLog::error('Unable to retrieve Lingotek Document ID for config chunk @id', array('@id' => $this->cid));
      }
    }
  }

  /**
   * Creates a Lingotek Document for this config chunk.
   *
   * @return bool
   *   TRUE if the document create operation was successful, FALSE on error.
   */
  protected function createLingotekDocument() {
    return ($this->api->addContentDocumentWithTargets($this)) ? TRUE : FALSE;
  }

  /**
   * Updates the existing Lingotek Documemnt for this config chunk.
   *
   * @return bool
   *   TRUE if the document create operation was successful, FALSE on error.
   */
  protected function updateLingotekDocument() {
    $result = $this->api->updateContentDocument($this);
    return ($result) ? TRUE : FALSE;
  }

  /**
   * Gets the local Lingotek metadata for this config chunk.
   *
   * @return array
   *   An array of key/value data for the current config chunk.
   */
  protected function metadata() {
    $metadata = array();

    $results = db_select('{lingotek_config_metadata}', 'meta')
        ->fields('meta')
        ->condition('id', $this->cid)
        ->execute();

    foreach ($results as $result) {
      $metadata[$result->config_key] = $result->value;
    }

    return $metadata;
  }

  /**
   * Return true if current chunk already has an assigned Lingotek Document ID
   *
   * @return boolean TRUE/FALSE
   */
  public function hasLingotekDocId() {
    $has_id = array_key_exists('document_id', $this->source_meta);
    if ($has_id && ($this->source_meta['document_id'] > 0)) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Get the chunk's target translation status for the given locale
   */
  public function getTargetStatus($lingotek_locale) {
    $result = db_select('{lingotek_config_metadata}', 'meta')
        ->fields('meta', array('value'))
        ->condition('id', $this->cid)
        ->condition('config_key', 'target_sync_status_' . $lingotek_locale)
        ->execute();
    return $result->value;
  }

  /**
   * Set the chunk's document ID in the config metadata table
   */
  public function setDocumentId($doc_id) {
    $this->setMetadataValue('document_id', $doc_id);
    return $this;
  }

  /**
   * Set the chunk's project ID in the config metadata table
   */
  public function setProjectId($project_id) {
    $this->setMetadataValue('project_id', $project_id);
    return $this;
  }

  /**
   * Set the chunk's status in the config metadata table
   */
  public function setStatus($status) {
    $this->setMetadataValue('chunk_sync_status', $status);
    return $this;
  }

  /**
   * Set the chunk's last error in the config metadata table
   */
  public function setLastError($errors) {
    $this->setMetadataValue('last_sync_error', substr($errors, 0, 255));
    return $this;
  }

  /**
   * Set the chunk's target status(es) in the config metadata table
   */
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

  /**
   * Gets the contents of this item formatted as XML to be sent to Lingotek.
   *
   * @return string
   *   The XML document representing the entity's translatable content.
   */
  public function documentLingotekXML() {
    $translatable = array();

    // for now, assume all strings in locales_source table for the given chunk are translatable
    if (TRUE) {
      $translatable = array_keys($this->source_data);
    }

    $content = '';
    foreach ($translatable as $field) {
      $text = lingotek_filter_placeholders($this->source_data[$field], TRUE);

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
    return db_select('{lingotek_config_metadata}', 'meta')
            ->fields('meta', array('value'))
            ->condition('config_key', $key)
            ->condition('id', $this->cid)
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
      db_insert('{lingotek_config_metadata}')
          ->fields(array(
            'id' => $this->cid,
            'config_key' => $key,
            'value' => $value,
            'created' => $timestamp,
            'modified' => $timestamp
          ))
          ->execute();
    }
    else {
      db_update('{lingotek_config_metadata}')
          ->fields(array(
            'value' => $value,
            'modified' => $timestamp
          ))
          ->condition('id', $this->cid)
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
      db_delete('{lingotek_config_metadata}')
          ->condition('id', $this->cid)
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
      LingotekLog::error('Unable to refresh local contents for config chunk @cid. Could not find Lingotek Document ID.', array('@cid' => $this->cid));
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
      LingotekLog::error('Unable to refresh local contents for config chunk @cid. Could not find Lingotek Document ID.', array('@cid' => $this->cid));
      return FALSE;
    }

    $api = LingotekApi::instance();
    $document_xml = $api->downloadDocument($document_id, $lingotek_locale);
    $target_language = Lingotek::convertLingotek2Drupal($lingotek_locale);

    /* FAST VERSION (git history for slow version) */
    // 1. save the dirty targets associated with given language
    $dirty_lids = self::getDirtyLidsByChunkIdAndLanguage($this->cid, $target_language);
    // 2. delete all segment targets associated with given language
    self::deleteSegmentTranslationsByChunkIdAndLanguage($this->cid, $target_language);
    // 3. insert all segments for the given language
    self::saveSegmentTranslations($document_xml, $target_language);
    // 4. return the dirty targets' statuses
    self::restoreDirtyLids($dirty_lids);
    /* END FAST */

    // set chunk status to current
    $this->setStatus(LingotekSync::STATUS_CURRENT);
    $this->setTargetsStatus(LingotekSync::STATUS_CURRENT, $lingotek_locale);

    return TRUE;
  }

  /**
   * Return all target segments by ID marked to be updated
   *
   * This is a preparatory step before resetting the locales targets for a given
   * chunk.
   *
   * @param int
   *    the ID of the chunk under which to search
   * @param string
   *    the language code for which to get the segments that need updating
   */
  public static function getDirtyLidsByChunkIdAndLanguage($chunk_id, $language) {
    $result = db_select('{locales_target}', 'lt')
        ->fields('lt', array('lid'))
        ->condition('lid', self::minLid($chunk_id), '>=')
        ->condition('lid', self::maxLid($chunk_id), '<=')
        ->condition('language', $language)
        ->condition('i18n_status', I18N_STRING_STATUS_CURRENT, '!=')
        ->condition('translation_agent_id', self::getLingotekTranslationAgentId())
        ->execute();
    return $result->fetchCol();
  }

  /**
   * Mark as dirty all target segments passed, in the locales targets
   *
   * @param array
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
   * Delete all target segments for a given chunk
   *
   * @param int
   *    the ID of the chunk for which to delete target segments
   * @param string
   *    the language code for which to delete target segments
   */
  public static function deleteSegmentTranslationsByChunkIdAndLanguage($chunk_id, $target_language) {
    db_delete('{locales_target}')
        ->condition('language', $target_language)
        ->condition('lid', self::minLid($chunk_id), '>=')
        ->condition('lid', self::maxLid($chunk_id), '<=')
        ->condition('translation_agent_id', self::getLingotekTranslationAgentId())
        ->execute();
  }

  /**
   * Get lingotek translation agent ID
   */
  public static function getLingotekTranslationAgentId() {
    $result = db_select('{lingotek_translation_agent}', 'lta')
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
    $result = db_select('{locales_target}', 'lt')
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
   *    the SimpleXMLElement object containing the translations to be saved
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
   * Magic get for access to chunk and chunk properties.
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
    return variable_get('lingotek_translate_config_workflow_id', '');
  }
  
  public function getProjectId() {
    return variable_get('lingotek_project', '');
  }
  
   public function getVaultId() {
    return variable_get('lingotek_vault', '');
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
    if (variable_get('lingotek_translate_config_misc', 0)) {
      $textgroups[] = 'misc';
    }
    return $textgroups;
  }
  
  public static function getLidBySource($source_string) {
    return db_select('{locales_source}', 's')
            ->fields('s', array('lid'))
        ->condition('s.source', $source_string)
        ->execute()
        ->fetchField();
  }


/*
 * Return all document IDs related to config translation
 */

  public static function getAllDocumentIds() {
  $result = db_select('{lingotek_config_metadata}', 'c')
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
  }

  public function postDownload($lingotek_locale, $completed) {
  }
}
