<?php

/**
 * @file
 * LingotekSync
 */

/**
 * A utility class for Lingotek Syncing.
 */
class LingotekSync {

  const STATUS_NONE = 'NONE';
  const STATUS_EDITED = 'EDITED';    // The node has been edited, but has not been uploaded to Lingotek
  const STATUS_CURRENT = 'CURRENT';  // The node or target translation is current
  const STATUS_ERROR = 'ERROR';
  const STATUS_PENDING = 'PENDING';  // The target translation is awaiting to receive updated content from Lingotek
  const STATUS_READY = 'READY';      // The target translation is complete and ready for download
  const STATUS_INTERIM = 'INTERIM'; // Part of the target translation is done and ready for download what has been done
  const STATUS_READY_INTERIM = 'READY_INTERIM';
  const STATUS_TARGET = 'TARGET';    // A target node is being used to store a translation (ignored for upload by Lingotek)
  const STATUS_UNTRACKED = 'UNTRACKED'; // A translation was discovered that is not currently managed by Lingotek
  const STATUS_TARGET_LOCALIZE = 'TARGET_LOCALIZE'; // A localization must be made of the source before uploading to Lingotek
  const STATUS_TARGET_EDITED = 'TARGET_EDITED'; // A localized version is ready for uploading to Lingotek
  const STATUS_NON_LINGOTEK = 'NON_LINGOTEK'; // Translations exist but the translation_agent_id != 3 (Not owned by Lingotek)
  const STATUS_DELETED = 'DELETED'; // Entity was translated but was afterward deleted

  const PROFILE_CUSTOM = 'CUSTOM';
  const PROFILE_DISABLED = 'DISABLED';
  const PROFILE_ENABLED = 'ENABLED';
  const PROFILE_CONFIG = 'CONFIG';
  const PROFILE_AUTOMATIC = 0;
  const PROFILE_MANUAL = 1;
  const PROFILE_INHERIT = 'INHERIT';
  const MARKED = 1;
  const NOT_MARKED = 0;
  const INVALID_XML_PRESENT = 1;
  const TRANSLATION_AGENT_ID_UNKNOWN = 1;
  const TRANSLATION_AGENT_ID_DRUPAL = 2;
  const TRANSLATION_AGENT_ID_LINGOTEK = 3;
  const MARKED_OFFSET = 1000000000;

  public static function getTargetStatus($doc_id, $lingotek_locale) {
    $key = 'target_sync_status_' . $lingotek_locale;
    if ($chunk_id = LingotekConfigSet::getIdByDocId($doc_id)) {
      return LingotekConfigSet::getTargetStatusById($chunk_id, $lingotek_locale);
    }
    else {
      list($entity_id, $entity_type) = self::getEntityIdFromDocId($doc_id);
      return lingotek_keystore($entity_type, $entity_id, $key);
    }
    LingotekLog::error('Did not find any local info for Lingotek Doc ID "@id"', array('@id' => $doc_id));
    return FALSE;
  }
  public static function getAllTargetStatusForEntity($entity_type, $entity_id, $lingotek_locale = NULL) {
    $dbkey = 'target_sync_status_';
    $query = db_select('lingotek_entity_metadata', 'l')
      ->fields('l', array('entity_key', 'value'))
      ->condition('entity_type', $entity_type)
      ->condition('entity_id', $entity_id);
    if ($lingotek_locale !== NULL) {
      $query->condition('entity_key', $dbkey . $lingotek_locale);
    }
    else {
      $query->condition('entity_key', $dbkey . '%', 'LIKE');
    }
    $result = $query->execute()->fetchAll();
    $targets = array();
    foreach ($result as $r_obj) {
      // get the locale of each result
      $locale = substr($r_obj->entity_key, strlen($dbkey));
      // assign the status for that locale
      $targets[$locale] = $r_obj->value;
    }
    return $targets;
  }

  public static function getTargetStatusOptions() {
    return array(
      'STATUS_NONE' => self::STATUS_NONE,
      'STATUS_CURRENT' => self::STATUS_CURRENT,
      'STATUS_EDITED' => self::STATUS_EDITED,
      'STATUS_ERROR' => self::STATUS_ERROR,
      'STATUS_PENDING' => self::STATUS_PENDING,
      'STATUS_READY' => self::STATUS_READY,
      'STATUS_READY_INTERIM' => self::STATUS_READY_INTERIM,
      'STATUS_INTERIM' => self::STATUS_INTERIM,
      'STATUS_TARGET' => self::STATUS_TARGET,
      'STATUS_UNTRACKED' => self::STATUS_UNTRACKED,
      'STATUS_TARGET_LOCALIZE' => self::STATUS_TARGET_LOCALIZE,
      'STATUS_TARGET_EDITED' => self::STATUS_TARGET_EDITED,
    );
  }

  public static function getAllTargetStatusNotCurrent($nid) {
    $query = db_select('lingotek_entity_metadata', 'l')
        ->fields('l', array('entity_key', 'value'))
      ->condition('entity_type', 'node')
      ->condition('entity_key', 'target_sync_status_%', 'LIKE')
      ->condition('value', 'CURRENT', '!=')
      ->condition('entity_id', $nid);
    $result = $query->execute()->fetchAll();
    return $result;
  }

  public static function setTargetStatus($entity_type, $entity_id, $lingotek_locale, $status, $update_on_dup = TRUE) {
    $key = 'target_sync_status_' . $lingotek_locale;
    return lingotek_keystore($entity_type, $entity_id, $key, $status, $update_on_dup);
  }

  public static function setAllTargetStatus($entity_type, $entity_id, $status) {
    if($entity_type === 'config'){
        $query = db_update('lingotek_config_metadata')
            ->condition('id', $entity_id, "=")
            ->condition('config_key', 'target_sync_status%', 'LIKE')
            ->fields(array('value' => $status, 'modified' => time()))
            ->execute();
      return;
    }
    $query = db_update('lingotek_entity_metadata')
        ->condition('entity_type', $entity_type)
        ->condition('entity_id', $entity_id)
        ->condition('entity_key', 'target_sync_status%', 'LIKE')
        ->fields(array('value' => $status, 'modified' => time()))
        ->execute();
    lingotek_cache_clear($entity_type, $entity_id);
  }

  public static function bulkSetAllTargetStatus($entity_type, $entity_ids, $status){
    if($entity_type === 'config'){
        $query = db_update('lingotek_config_metadata')
            ->condition('id', $entity_ids, "IN")
            ->condition('config_key', 'target_sync_status%', 'LIKE')
            ->fields(array('value' => $status, 'modified' => time()))
            ->execute();
      return;
    }
    $query = db_update('lingotek_entity_metadata')
        ->condition('entity_type', $entity_type)
        ->condition('entity_id', $entity_ids, "IN")
        ->condition('entity_key', 'target_sync_status%', 'LIKE')
        ->fields(array('value' => $status, 'modified' => time()))
        ->execute();
    foreach ($entity_ids as $eid) {
      lingotek_cache_clear($entity_type, $eid);
    }
  }

  public static function setUploadStatus($entity_type, $entity_id, $status) {
    return lingotek_keystore($entity_type, $entity_id, 'upload_status', $status);
  }

  public static function setAllUploadStatuses($entity_type, $entity_ids, $status) {
    foreach ($entity_ids as $entity_id) {
      $query = db_update('lingotek_entity_metadata')
        ->condition('entity_id', $entity_id)
        ->condition('entity_type', $entity_type)
        ->condition('entity_key', 'upload_status')
        ->fields(array('value' => $status, 'modified' => time()))
        ->execute();
    }
  }

  public static function setUploadStatuses($entity_type, $entity_ids, $status) {
    foreach($entity_ids as $entity_id) {
      return lingotek_keystore($entity_type, $entity_id, 'upload_status', $status);
    }
  }

  public static function getUploadStatus($entity_type, $entity_id) {
    $query = db_select('lingotek_entity_metadata', 'lem')
    ->fields('lem', array('value'))
    ->condition('lem.entity_id', $entity_id)
    ->condition('lem.entity_type', $entity_type)
    ->condition('lem.entity_key', 'upload_status');
    $upload_status = $query->execute()->fetchField();

    return $upload_status;
  }

  public static function getSyncProjects() {
    $query = db_select('lingotek_entity_metadata', 'l');
    $query->fields('l', array('value'));
    $query->condition('entity_type', 'node');
    $query->condition('entity_key', 'project_id');
    $query->distinct();
    $result = $query->execute();
    $projects = $result->fetchCol();
    $default_project_id = variable_get('lingotek_project', NULL);
    if (!is_null($default_project_id) && !in_array($default_project_id, $projects)) {
      $projects[] = $default_project_id;
    }
    return $projects;
  }

  public static function insertTargetEntriesForAllSets($lingotek_locale) {
    // insert/update a target language for all chunks
    $query = db_select('lingotek_config_metadata', 'meta')
        ->fields('meta', array('id'))
        ->groupBy('id');
    $ids = $query->execute()->fetchCol();

    foreach ($ids as $i) {
      $set = LingotekConfigSet::loadById($i);
      if (is_object($set)) {
        $set->setTargetsStatus(self::STATUS_PENDING, $lingotek_locale);
      }
    }
  }

  public static function insertTargetEntriesForAllEntities($lingotek_locale) {
    //this prevents a target from being added to the same source
    $locale = strtolower(str_replace("_", "-", $lingotek_locale));
    $node_source_check = db_select('node', 'n');
    $node_source_check->addField('n', 'nid');
    $or = db_or()->condition('n.tnid','0')->where('n.tnid = n.nid');
    $node_source_check->condition($or);
    $node_source_check->condition('n.language', $locale);

    $taxonomy_source_check = db_select('taxonomy_term_data', 't');
    $taxonomy_source_check->addField('t', 'tid');
    $taxonomy_source_check->condition('t.language', $locale);

    // insert/update a target language for all entities
    $query = db_select('lingotek_entity_metadata', 'meta')
        ->fields('meta', array('entity_id', 'entity_type'))
        ->condition('meta.entity_key', 'document_id')
        ->condition('entity_id', $node_source_check, "NOT IN")
        ->condition('entity_id', $taxonomy_source_check, "NOT IN");

    if (module_exists('comment')) {
      $comment_source_check = db_select('comment', 'c');
      $comment_source_check->addField('c', 'cid');
      $comment_source_check->condition('c.language', $locale);
      $query->condition('entity_id', $comment_source_check, "NOT IN");
    }

    if (module_exists('bean') && variable_get('lingotek_translate_beans')) {
      $bean_source_check = db_select('bean', 'b');
      $bean_source_check->addField('b', 'bid');
      $bean_source_check->condition('b.language', $locale);
      $query->condition('entity_id', $bean_source_check, "NOT IN");
    }

    if (module_exists('group') && variable_get('lingotek_translate_groups')) {
      $group_source_check = db_select('groups', 'g');
      $group_source_check->addField('g', 'gid');
      $group_source_check->condition('g.language', $locale);
      $query->condition('entity_id', $group_source_check, "NOT IN");
    }

    if (module_exists('file_entity') && variable_get('lingotek_translate_files')) {
      $file_source_check = db_select('file_managed', 'fm');
      $file_source_check->addField('fm', 'fid');
      $file_source_check->condition('fm.language', $locale);
      $query->condition('entity_id', $file_source_check, "NOT IN");
    }

    $entities = $query->execute()->fetchAll();

    foreach ($entities as $e) {
      self::setTargetStatus($e->entity_type, $e->entity_id, $lingotek_locale, self::STATUS_PENDING);
    }
  }

  public static function delete_entity_from_metadata($entity_type, $entity_id) {
    db_delete('lingotek_entity_metadata')
        ->condition('entity_type', $entity_type)
        ->condition('entity_id', $entity_id)
        ->execute();
    lingotek_cache_clear($entity_type, $entity_id);
  }

  public static function deleteTargetStatus($entity_type, $entity_id, $lingotek_locale) {
    $key = 'target_sync_status_' . $lingotek_locale;
    return lingotek_keystore_delete($entity_type, $entity_id, $key);
  }

  // Remove the node sync target language entries from the lingotek table lingotek_delete_target_sync_status_for_all_nodes
  public static function deleteTargetEntriesForAllEntities($lingotek_locale) {
    $keys = array(
      'target_sync_status_' . $lingotek_locale,
    );
    db_delete('lingotek_entity_metadata')->condition('entity_key', $keys, 'IN')->execute();
  }

  public static function deleteTargetEntriesForAllChunks($lingotek_locale) {
    $key = 'target_sync_status_' . $lingotek_locale;
    db_delete('lingotek_config_metadata')->condition('config_key', $key)->execute();
  }

  public static function deleteTargetEntriesForAllDocs($lingotek_locale) {
    self::deleteTargetEntriesForAllEntities($lingotek_locale);
    self::deleteTargetEntriesForAllChunks($lingotek_locale);
    lingotek_cache_clear();
  }


  /**
   * getDocIdTargetsByStatus
   *
   * @param status (e.g., LingotekSync::READY)
   *
   * @return an array of associate arrays.  Each associate array will have a 'nid' (e.g., 5), 'locale' (e.g., 'de_DE'), and optionally 'doc_id' (e.g., 46677222-b5ec-47d5-880e-24632feffaf5)
   */
  public static function getTargetsByStatuses($entity_type, $statuses=array(), $include_doc_ids = FALSE) {
    $target_language_search = '%';
    $query = db_select('lingotek_entity_metadata', 'l');
    $query->fields('l', array('entity_id', 'entity_key', 'value'));
    $query->condition('entity_type', $entity_type);
    $query->condition('entity_key', 'target_sync_status_' . $target_language_search, 'LIKE');
    $or = db_or();
    foreach($statuses as $status){
      $or->condition('value', $status);
    }
    $query->condition($or);
    $result = $query->execute();
    $records = $result->fetchAll(); //$result->fetchAllAssoc('nid');

     // build nid_doc_map (if needed)
    if ($include_doc_ids) {
      $nid_doc_map = array();
      foreach ($records as $record) {
        if (!key_exists($record->entity_id, $nid_doc_map)) {
          $doc_id = self::getDocIdFrom($record->entity_id);
          $nid_doc_map[$record->entity_id] = $doc_id;
        }
      }
    }
    $targets = array();
    foreach ($records as $record) {
      $doc_target = array(
        'id' => $record->entity_id,
        'doc_id' => $include_doc_ids ? $nid_doc_map[$record->entity_id] : NULL,
        'locale' => str_replace('target_sync_status_', '', $record->entity_key)
      );
      $targets[] = $doc_target;
    }

    return $targets;
  }

  public static function getDownloadableReport() {
    $document_ids = array_unique(array_merge(self::getConfigDocIdsByStatus(self::STATUS_PENDING), self::getConfigDocIdsByStatus(self::STATUS_READY)));

    $report = array(
      'download_targets_workflow_complete' => array(), // workflow complete and ready for download
      'download_targets_workflow_complete_count' => 0,
      'download_workflow_complete_targets' => array(),
      'download_targets_workflow_incomplete' => array(), // not workflow complete (but download if wanted)
      'download_targets_workflow_incomplete_count' => 0,
      'download_workflow_incomplete_targets' => array(),
    );
    if (empty($document_ids))
      return $report; // if no documents are PENDING, then no need to make the API call.
    $response = lingotek_update_config_progress($document_ids);

    $locales = lingotek_get_target_locales();
    foreach ($document_ids as $document_id) {
      if (!$document_id) {
        continue;
      }
      foreach ($locales as $locale) {
        if (isset($response->byDocumentIdAndTargetLocale->$document_id->$locale)) {
          $target_status = self::getTargetStatus($document_id, $locale);
          $doc_target = array(
            'document_id' => $document_id,
            'locale' => $locale,
            'percent_complete' => $response->byDocumentIdAndTargetLocale->$document_id->$locale,
          );
          if ($target_status == self::STATUS_READY) {
            $report['download_targets_workflow_complete'][] = $doc_target;
            $report['download_targets_workflow_complete_count']++;
          }
          elseif ($target_status == self::STATUS_PENDING) {
            $report['download_targets_workflow_incomplete'][] = $doc_target;
            $report['download_targets_workflow_incomplete_count']++;
          }
        }
      }
    }
    return $report;
  }

   /**
   * Sums the values of the arrays be there keys (PHP 4, PHP 5)
   * array array_sum_values ( array array1 [, array array2 [, array ...]] )
   */
  public static function arraySumValues() {
    $return = array();
    $intArgs = func_num_args();
    $arrArgs = func_get_args();
    if ($intArgs < 1) {
      trigger_error('Warning: Wrong parameter count for arraySumValues()', E_USER_WARNING);
    }

    foreach ($arrArgs as $arrItem) {
      if (!is_array($arrItem)) {
        trigger_error('Warning: Wrong parameter values for arraySumValues()', E_USER_WARNING);
      }
      foreach ($arrItem as $k => $v) {
        if (!key_exists($k, $return)) {
          $return[$k] = 0;
        }
        $return[$k] += $v;
      }
    }
    return $return;

    $sumArray = array();
    foreach ($myArray as $k => $subArray) {
      foreach ($subArray as $id => $value) {
        $sumArray[$id]+=$value;
      }
    }
    return $sumArray;
  }

  public static function getSourceCounts($lingotek_locale) {
    $total = 0;
    $managed_entities = lingotek_managed_entity_types();
    $response = array();
    $response['types'] = array();
    foreach(array_keys($managed_entities) as $entity_type){
      $entity_type_count = self::getEntitySourceCount($lingotek_locale, $entity_type);
      $response['types'][$entity_type] = $entity_type_count;
      $total += $entity_type_count;
    }
    $response['total'] = $total;
    return $response;
  }

  public static function getEntitySourceCount($lingotek_locale, $entity_type = NULL) {
    $managed_entities = lingotek_managed_entity_types();
    $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);
    $q = array();
    $total_count = 0;
    foreach ($managed_entities as $m_entity_type => $properties) {
      if (!is_null($entity_type) && $entity_type != $m_entity_type) {
        continue;
      }
      $entity_base_table = $properties['base table'];
      $query = db_select($entity_base_table, 't')->condition('t.language', $drupal_language_code);

      // exclude translation sets (only for nodes)
      if ($entity_base_table == 'node') {
        $tnid_query = db_or();
        $tnid_query->condition('t.tnid', 0);
        $tnid_query->where('t.tnid = t.nid');
        $query->condition($tnid_query);
      }

      // Exclude translation sets for menu_links
      if ($entity_base_table == 'menu_links') {
        $min_query = db_select('menu_links', 'ml')
          ->condition('ml.i18n_tsid', 0, '!=')
          ->groupBy('i18n_tsid');
        $min_query->addExpression('MIN(mlid)', 'minimum');

        $ml_or = db_or();
        $ml_or->condition('t.i18n_tsid', 0);
        $ml_or->condition('t.mlid', $min_query, 'IN');

        $query->condition('t.language', LANGUAGE_NONE, '!=');
        $query->condition($ml_or);
      }

      // exclude disabled entities (including those that have disabled bundles)
      $disabled_profile = LingotekProfile::loadById(LingotekSync::PROFILE_DISABLED);
      $disabled_entities = $disabled_profile->getEntities($entity_type);
      if (count($disabled_entities)) {
        $disabled_entity_ids = array();
        array_walk($disabled_entities, function($a) use (&$disabled_entity_ids) {
          $disabled_entity_ids[] = $a['id'];
        });
        $enabled_entity_ids = lingotek_get_enabled_entities_by_type($entity_type);
        if (count($disabled_entity_ids) < count($enabled_entity_ids)) {
          $query->condition($properties['entity keys']['id'], array_merge(array(-1), $disabled_entity_ids), "NOT IN"); //exclude disabled entities
        }
        else {
          $query->condition($properties['entity keys']['id'], array_merge(array(-1), $enabled_entity_ids), "IN"); //include only eabled entities
        }
      }

      $count = $query->countQuery()->execute()->fetchField();
      $total_count += $count;
    }
    return $total_count;
  }

  public static function getCountsByStatus($status, $lingotek_locale) {
    $total = 0;
    $managed_entities = lingotek_managed_entity_types();
    $response = array();
    $response['types'] = array();
    foreach(array_keys($managed_entities) as $entity_type){
      $entity_type_count = self::getEntityTargetCountByStatus($status, $lingotek_locale, $entity_type);
      $response['types'][$entity_type] = $entity_type_count;
      $total += $entity_type_count;
    }
    $response['total'] = $total;
    return $response;
  }

  /**
   * Get a count of translation targets by entity and status.
   *
   * @param mixed $status
   *   a string or array of strings containing the desired status(es) to count
   * @param string $lingotek_locale
   *   the desired locale to count
   * @param string $entity_type
   *   the desired entity type to count
   * @return array
   */
  public static function getEntityTargetCountByStatus($status, $lingotek_locale, $entity_type = NULL) {
    if (!is_array($status)) {
      $status = array(-1, $status);
    }
    $managed_entities = lingotek_managed_entity_types();
    $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);
    $target_prefix = 'target_sync_status_';
    $target_key = $target_prefix . $lingotek_locale;
    $q = array();
    $total_count = 0;
    foreach($managed_entities as $m_entity_type => $properties){
      if(!is_null($entity_type) && $entity_type != $m_entity_type){
        continue;
      }
      $entity_base_table = $properties['base table'];
      $query = db_select($entity_base_table, 't');
      $query->leftJoin('lingotek_entity_metadata', 'l', 'l.entity_id = '.$properties['entity keys']['id'].
       ' AND l.entity_type = \''.$m_entity_type.'\''.
       ' AND l.entity_key = \''.$target_key.'\' '
      );

      // exclude translation sets (only for nodes)
      if ($entity_base_table == 'node') {
        $tnid_query = db_or();
        $tnid_query->condition('t.tnid', 0);
        $tnid_query->where('t.tnid = t.nid');
        $query->condition($tnid_query);
      }

      // exclude disabled nodes (including those that have disabled bundles)
      $disabled_profile = LingotekProfile::loadById(LingotekSync::PROFILE_DISABLED);
      $disabled_entities = $disabled_profile->getEntities($entity_base_table);
      if (!empty($disabled_entities)) {
        $disabled_entity_ids = array();
        array_walk($disabled_entities, function($a) use (&$disabled_entity_ids) {
          $disabled_entity_ids[] = $a['id'];
        });
        $query->condition("t.".$properties['entity keys']['id'], $disabled_entity_ids, "NOT IN"); //exclude disabled entities
      }

      $query->condition('l.value', $status, 'IN');

      // exclude orphaned targets (targets whose source language has been deleted)
      if (db_field_exists($entity_base_table, 'language')) {
        $query->condition('t.language', '', '!=');
      }

      $count = $query->countQuery()->execute()->fetchField();
      $total_count += $count;
    }
    return $total_count;
  }

  public static function getTargetNodeCountByStatus($status, $lingotek_locale) {
    $target_prefix = 'target_sync_status_';
    $target_key = $target_prefix . $lingotek_locale;

    $query = db_select('lingotek_entity_metadata', 'l')->fields('l');
    $query->condition('entity_type', 'node');
    $query->condition('entity_key', $target_key);
    $query->condition('value', $status);
    $result = $query->countQuery()->execute()->fetchAssoc();

    $count = 0;
    if (is_array($result)) {
      $count = array_shift($result);
    }

    // count nodes having this language as the source as current
    if ($status == LingotekSync::STATUS_CURRENT) {
      $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale, TRUE);
      $query = db_select('node', 'n');
      $query->leftJoin('lingotek_entity_metadata', 'l', 'l.entity_id = n.nid
        AND l.entity_type = \'node\'
        AND l.entity_key = \'profile\'
           AND l.value != \'DISABLED\'');
      $query->condition('n.language', $drupal_language_code);
      $query->addExpression('COUNT(*)', 'cnt');
      $result = $query->execute()->fetchField();
      $count += $result;
    }
    return $count;
  }

  public static function getTargetChunkCountByStatus($status, $lingotek_locale) {
    $target_prefix = 'target_sync_status_';
    $target_key = $target_prefix . $lingotek_locale;

    $query = db_select('lingotek_config_metadata', 'l')->fields('l');
    $query->condition('value', $status);
    $query->condition('config_key', $target_key);

    $count = 0;
    $result = $query->countQuery()->execute()->fetchAssoc();
    if (is_array($result)) {
      $count = array_shift($result);
    }
    return $count;
  }

  //lingotek_count_all_targets
  public static function getTargetCountByStatus($status, $lingotek_locale) {

    $count = 0;

    // get the count of nodes
    $count += self::getTargetNodeCountByStatus($status, $lingotek_locale);

    // get the count of config chunks (turned off for now)
    /*
    if (variable_get('lingotek_translate_config', 0)) {
      $count += self::getTargetChunkCountByStatus($status, $lingotek_locale);
    }
     */
    return $count;
  }

  public static function getTargetCountByDocumentIds($document_ids) {
    if (empty($document_ids)) {
      return;
    }
    if (!is_array($document_ids)) {
      $document_ids = array($document_ids);
    }
    $subquery = db_select('lingotek_entity_metadata', 'l1')
        ->fields('l1', array('entity_id'))
      ->condition('l1.entity_type', 'node')
      ->condition('l1.entity_key', 'document_id')
      ->condition('l1.value', $document_ids, 'IN');
    $query = db_select('lingotek_entity_metadata', 'l');
    $query->addField('l', 'entity_id', 'nid');
    $query->condition('l.entity_type', 'node');
    $query->condition('l.entity_key', 'target_sync_status_%', 'LIKE');
    $query->condition('l.entity_id', $subquery, 'IN');
    $query->addExpression('COUNT(l.entity_key)', 'targets');

    $query->groupBy('l.entity_id');
    $result = $query->execute()->fetchAllAssoc('nid');
    return $result;
  }

  public static function getETNodeIds() { // get nids for entity_translation nodes that are not lingotek pushed
    $types = lingotek_translatable_node_types(); // get all translatable node types
    $et_content_types = array();
    foreach ($types as $type) {
      if (lingotek_managed_by_entity_translation($type)) { // test if lingotek_managed_by_entity_translation
        $et_content_types[] = $type;
      }
    }
    if (empty($et_content_types))
      return array();

    $nodes = entity_load('node', FALSE, array('type' => $et_content_types)); // select nodes with et types
    $et_node_ids = array();
    foreach ($nodes as $node) {
      if (!lingotek_node_pushed($node)) {
        $et_node_ids[] = $node->nid;
      }
    }
    return $et_node_ids;
  }

  protected static function getQueryCompletedConfigTranslations($drupal_codes) {
    // return a query object that contains all fully-translated/current strings
    // or ones that were not translated by Lingotek.
    // use the first addtl language as the query's base.
    $first_lang = array_shift($drupal_codes);
    $lingotek_id = LingotekConfigSet::getLingotekTranslationAgentId();
    $primary_or = db_or()
        ->condition('lt0.i18n_status', 0)
        ->condition('lt0.translation_agent_id', $lingotek_id, '!=');
    $query = db_select('locales_target', "lt0")
        ->fields('lt0', array('lid'))
        ->condition('lt0.language', $first_lang)
        ->condition($primary_or);
    $addtl_joins = 0;
    foreach ($drupal_codes as $new_join) {
      // join a new instance of locales_target for each target language
      // where an entry for the language exists for the given lid and
      // it is "current" (ie. i18n_status field is set to 0)
      $addtl_joins++;
      $ja = "lt$addtl_joins"; // join alias
      $join_str = "$ja.lid = lt0.lid and $ja.language = '$new_join' and ($ja.i18n_status = 0 or $ja.translation_agent_id != $lingotek_id)";
      $query->join('locales_target', $ja, $join_str);
    }
    return $query;
  }

  public static function getConfigSet($set_id) {
    // return LingotekConfigSet object containing all segments
    // for the given set id.
    return LingotekConfigSet::loadById($set_id);
  }

  public static function getDirtySetLids() {
    // return the list of all lids from the locale_source table *not* fully translated
    $source_language = language_default();
    if (!isset($source_language->lingotek_locale)) {
      $source_language->lingotek_locale = Lingotek::convertDrupal2Lingotek($source_language->language);
    }
    $lingotek_codes = Lingotek::getLanguagesWithoutSource($source_language->lingotek_locale);
    if (!count($lingotek_codes)) {
      LingotekLog::error('No languages configured for this Lingotek account.', array());
      return array();
    }
    // get the drupal language for each associated lingotek locale
    $drupal_codes = array();
    foreach ($lingotek_codes as $lc) {
      $drupal_codes[] = Lingotek::convertLingotek2Drupal($lc);
    }
    // get the list of all segments that need updating
    // that belong to the textgroups the user wants translated
    $textgroups = array_merge(array(-1), LingotekConfigSet::getTextgroupsForTranslation());
    $max_length = variable_get('lingotek_config_max_source_length', LINGOTEK_CONFIG_MAX_SOURCE_LENGTH);
    $query = db_select('locales_source', 'ls');
    $query->fields('ls', array('lid'))
        ->condition('ls.source', '', '!=')
        ->condition('ls.lid', self::getQueryCompletedConfigTranslations($drupal_codes), 'NOT IN')
        ->where('length(ls.source) < ' . (int) $max_length);
    if (in_array('misc', $textgroups)) {
      $or = db_or();
      $or->condition('ls.textgroup', $textgroups, 'IN');
      $or->where("ls.textgroup NOT IN ('default','menu','taxonomy','views','blocks','field')");
      $query->condition($or);
    }
    else {
      $query->condition('ls.textgroup', $textgroups, 'IN');
    }
    return $query->execute()->fetchCol();
  }

  public static function getDirtyConfigChunks() {
    // return the set of chunk IDs, which are the chunks that contain
    // lids that are in need of some translation.  These IDs are calculated
    // as the segment ID of the first segment in the chunk, divided by
    // the configured chunk size.  So, segments 1 through [chunk size] would
    // be in chunk #1, etc.
    $lids = self::getDirtySetLids();
    $chunk_ids = array();
    foreach ($lids as $lid) {
      $id = LingotekConfigSet::getSetId($lid);
      if (array_key_exists($id, $chunk_ids)) {
        $chunk_ids[$id]++;
      }
      else {
        $chunk_ids[$id] = 1;
      }
    }
    $chunk_ids = self::pruneChunksWithPendingTranslations($chunk_ids);

    return $chunk_ids;
  }

  protected static function getChunksWithPendingTranslations() {
    // get the list of chunks with pending translations
    $result = db_select('lingotek_config_metadata', 'meta')
        ->fields('meta', array('id', 'id'))
        ->condition('config_key', 'target_sync_status_%', 'LIKE')
        ->condition('value', array(self::STATUS_PENDING, self::STATUS_READY), 'IN')
        ->distinct()
        ->execute();
    return $result->fetchAllKeyed();
  }

  protected static function pruneChunksWithPendingTranslations($chunk_ids) {
    // return the chunk_ids not in the pending set
    $final_chunks = array_diff_key($chunk_ids, self::getChunksWithPendingTranslations());
    return $final_chunks;
  }

  public static function getUploadableReport() {
    // Handle configuration chunks
    $report = array();
    if (variable_get('lingotek_translate_config')) {
      $config_chunks_to_update = self::getDirtyConfigChunks();
      $num_updates = count($config_chunks_to_update);
      $report = array(
        'upload_config' => (array_keys($config_chunks_to_update)),
        'upload_config_count' => $num_updates,
        );
    }
    return $report;
  }

  public static function getReport() {
    $report = array_merge(
        self::getUploadableReport(), self::getDownloadableReport()
    );
    return $report;
  }

  public static function getNodeIdsByStatus($status, $source) {
    $query = db_select('lingotek_entity_metadata', 'l');
    $query->condition('entity_type', 'node');
    $query->addField('l', 'entity_id', 'nid');
    if($source) {
      $query->condition('entity_key', 'upload_status');
    } else {
      $query->condition('entity_key', 'target_sync_status_%', 'LIKE');
    }
    $query->condition('value', $status);
    $query->distinct();
    $result = $query->execute();
    $nids = $result->fetchCol();
    return $nids;
  }

  public static function getEntityIdsToUpload($entity_type, $entity_ids = null) {
    $info = entity_get_info($entity_type);
    $id_key = $info['entity keys']['id'];
    $query = db_select($info['base table'], 'base');
    $query->addField('base', $id_key);
    $query->leftJoin('lingotek_entity_metadata', 'upload', 'upload.entity_id = base.' . $id_key . ' and upload.entity_type =\'' . $entity_type . '\' and upload.entity_key = \'upload_status\'');

    $query2 = db_select('lingotek_entity_metadata', 'lem');
    $query2->distinct();
    $query2->addField('lem', 'entity_id');
    $query2->condition('entity_key', 'profile');
    $query2->condition('entity_type', $entity_type);
    $query2->condition('value', 'DISABLED');

    $query->condition('upload.entity_id', $query2, 'NOT IN');

    if ($entity_type == 'node') {
      // Exclude any target nodes created using node-based translation.
      $tnid_query = db_or();
      $tnid_query->condition('base.tnid', 0);
      $tnid_query->where('base.tnid = base.nid');
      $query->condition($tnid_query);
    }

    $or = db_or();
    $or->condition('upload.value', LingotekSync::STATUS_EDITED);
    $or->isNull('upload.value');
    $query->condition($or);
    if($entity_ids !== null){
      $query->condition('upload.entity_id', $entity_ids, 'IN');
    }
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getConfigSetIdsToUpload() {
    $query = db_select('lingotek_config_metadata', 'lcm')
        ->fields('lcm', array('id'))
        ->condition('config_key', 'upload_status')
        ->condition('value', LingotekSync::STATUS_EDITED);
    $set_ids = $query->execute()->fetchCol();
    return $set_ids;
  }

  /*
   * Get the entities, by ID, by target status
   * @param $entity_type
   *   a string of the entity type
   * @param $status
   *   a string containing the desired status
   * @param $locales
   *   (optional) an array containing the locales to check
   *
   * @return an array of IDs
   */
  public static function getEntityIdsByTargetStatus($entity_type, $status, $locales = NULL) {
    if ($locales === NULL) {
      $verb = 'LIKE';
      $target_locales = 'target_sync_status_%';
    }
    else {
      $verb = 'IN';
      $target_locales = array();
      foreach ($locales as $l) {
        $target_locales[] = 'target_sync_status_' . $l;
      }
    }
    $query = db_select('lingotek_entity_metadata', 'l')
        ->distinct()
      ->condition('entity_type', $entity_type)
      ->condition('entity_key', $target_locales, $verb)
      ->condition('value', $status);
    $query->addField('l', 'entity_id', 'nid');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getEntityIdsByStatuses($entity_type, $statuses = array(), $locales = NULL) {
    if ($locales === NULL) {
      $verb = 'LIKE';
      $target_locales = 'target_sync_status_%';
    }
    else {
      $verb = 'IN';
      $target_locales = array();
      foreach ($locales as $l) {
        $target_locales[] = 'target_sync_status_' . $l;
      }
    }
    $or = db_or();
    foreach($statuses as $status){
      $or->condition('value', $status);
    }
    $query = db_select('lingotek_entity_metadata', 'l')
        ->distinct()
      ->condition('entity_type', $entity_type)
      ->condition('entity_key', $target_locales, $verb)
      ->condition($or)
      ->condition('value', LingotekSync::PROFILE_DISABLED, '<>');
    $query->addField('l', 'entity_id', 'nid');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getEntityIdsByProfileStatus($entity_type, $status) {
    $query = db_select('lingotek_entity_metadata', 'l')
        ->distinct()
      ->condition('entity_type', $entity_type)
      ->condition('entity_key', 'profile')
      ->condition('value', $status);
    $query->addField('l', 'entity_id', 'nid');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getProfileByEntityId($entity_type, $entity_id) {
    $query = db_select('lingotek_entity_metadata', 'lem')
      ->fields('lem', array('value'))
      ->condition('lem.entity_id', $entity_id)
      ->condition('lem.entity_type', $entity_type)
      ->condition('lem.entity_key', 'profile');
    $profile = $query->execute()->fetchField();

    return $profile;
  }

  public static function getConfigDocIdsByStatus($status) {
    $doc_ids = array();

    if (variable_get('lingotek_translate_config', 0)) {
      // retrieve document IDs from config chunks
      $cids = self::getChunkIdsByStatus($status);
      if (!empty($cids)) {
        $query = db_select('lingotek_config_metadata', 'meta');
        $query->fields('meta', array('value'));
        $query->condition('config_key', 'document_id');
        $query->condition('id', $cids);
        $result = $query->execute();
        $doc_ids = array_merge($doc_ids, $result->fetchCol());
      }
    }

    return $doc_ids;
  }

  public static function getSetIdsFromLids($lids) {
    if (empty($lids)) {
      return $lids;
    }
    $query = db_select('lingotek_config_map', 'l');
    $query->addField('l', 'set_id');
    $query->condition('lid', $lids, 'IN');
    $sids = $query->execute()->fetchCol();

    return $sids;
  }

  public static function getConfigDocIdsFromSetIds($sids) {
    if (empty($sids)) {
      return $sids;
    }
    $query = db_select('lingotek_config_metadata', 'l');
    $query->addField('l', 'value');
    $query->condition('id', $sids, 'IN');
    $query->condition('config_key', 'document_id');
    $doc_ids = $query->execute()->fetchCol();

    return $doc_ids;
  }

  public static function updateConfigSetWorkflow($set_id, $workflow_id){
    $insertRecord = array(
        "value" => $workflow_id,
        "created" => time(),
        "modified" => time(),
        "id" => $set_id,
        "config_key" => 'workflow_id'
    );
    $updateRecord = array(
        "value" => $workflow_id,
        "modified" => time()
    );
    db_merge('lingotek_config_metadata')
            ->key(array('id' => $set_id, 'config_key' => 'workflow_id'))
            ->insertFields($insertRecord)
            ->updateFields($updateRecord)
            ->execute();
//    drupal_write_record('lingotek_config_metadata', $record);
  }

  public static function getWorkflowIdFromConfigSet($sid){
    $query = db_select('lingotek_config_metadata', 'lcm');
    $query->addField('lcm', 'value');
    $query->condition('id', $sid, '=');
    return $query->execute()->fetchCol();
  }

  public static function getWorkflowIdFromEntityId($id) {
    $query = db_select('lingotek_entity_metadata', 'lem');
    $query->addField('lem', 'value');
    $query->condition('entity_key', 'workflow_id', '=');
    $query->condition('entity_id', $id, '=');

    $result = $query->execute()->fetchCol();

    return $result[0];
  }

  public static function getChunkIdsByStatus($status) {
    $query = db_select('lingotek_config_metadata', 'meta');
    $query->fields('meta', array('id'));
    $query->condition('config_key', 'target_sync_status_%', 'LIKE');
    $query->condition('value', $status);
    $query->distinct();
    $result = $query->execute();
    $cids = $result->fetchCol();
    return $cids;
  }

  public static function disassociateAllEntities() {
    db_truncate('lingotek_entity_metadata')->execute();
    lingotek_cache_clear();
  }

  public static function disassociateAllSets() {
    db_truncate('lingotek_config_metadata')->execute();
  }

  public static function disassociateEntities($document_ids = array()) {
    $eids = self::getNodeIdsFromDocIds($document_ids);
    db_delete('lingotek_entity_metadata')
        ->condition('entity_type', 'node')
        ->condition('entity_id', $eids, 'IN')
        ->execute();
    lingotek_cache_clear('node');
  }

  public static function getAllLocalDocIds() {
    // entity-related doc IDs
    $query = db_select('lingotek_entity_metadata', 'l');
    $query->fields('l', array('value'));
    $query->condition('entity_key', 'document_id');
    $query->distinct();
    $result = $query->execute();
    $doc_ids = $result->fetchCol();

    // config-related doc IDs
    $query = db_select('lingotek_config_metadata', 'l')
        ->fields('l', array('value'))
        ->condition('config_key', 'document_id')
        ->distinct();
    $result = $query->execute();
    $doc_ids = array_merge($doc_ids, $result->fetchCol());

    return $doc_ids;
  }

  public static function getAllNodeIds() { // This query is broken - it also gets things without doc ids
    // all node ids having document_ids in lingotek table
    $query = db_select('lingotek_entity_metadata', 'l');
    $query->addField('l', 'entity_id');
    //$query->condition('lingokey', 'document_id');
    $query->distinct('entity_id');
    $result = $query->execute();
    $nids = $result->fetchCol();
    return $nids;
  }

  public static function getEntityIdFromDocId($lingotek_document_id, $entity_type = NULL) {
    $key = 'document_id';

    $query = db_select('lingotek_entity_metadata', 'l')->fields('l');
    if ($entity_type) {
      $query->condition('entity_type', $entity_type);
    }
    $query->condition('entity_key', $key);
    $query->condition('value', $lingotek_document_id);
    $result = $query->execute();

    $found = FALSE;
    $type = FALSE;

    if ($record = $result->fetchAssoc()) {
      $found = $record['entity_id'];
      $type = $record['entity_type'];
    }

    return array($found, $type);
  }

  public static function getNodeIdFromDocId($lingotek_document_id) {
    list($id, $type) = LingotekSync::getEntityIdFromDocId($lingotek_document_id);
    return array($id, $type);
  }

  public static function getNodeIdsFromDocIds($lingotek_document_ids) {
    $nids = array();
    $query = db_select('lingotek_entity_metadata', 'l')
        ->fields('l', array('nid'))
        ->condition('entity_type', 'node')
        ->condition('entity_key', $key)
        ->condition('value', $lingotek_document_ids, 'IN');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getDocIdFromNodeId($drupal_node_id) {
    return getDocIdFromEntityId('node', $drupal_node_id);
  }

  public static function getDocIdFromEntityId($entity_type, $entity_id) {
    $found = FALSE;

    $query = db_select('lingotek_entity_metadata', 'l')->fields('l');
    $query->condition('entity_type', $entity_type);
    $query->condition('entity_id', $entity_id);
    $query->condition('entity_key', 'document_id');
    $result = $query->execute();

    if ($record = $result->fetchAssoc()) {
      $found = $record['value'];
    }

    return $found;
  }

  public static function getDocIdsFromEntityIds($entity_type, $entity_ids, $associate = FALSE) {

    $query = db_select('lingotek_entity_metadata', 'l');
    $query->addField('l', 'value', 'doc_id');
    $query->condition('entity_type', $entity_type);
    $query->condition('entity_id', $entity_ids, 'IN');
    $query->condition('entity_key', 'document_id');

    if ($associate) {
      $query->addField('l', 'entity_id', 'nid');
      $result = $query->execute()->fetchAllAssoc('nid');
    }
    else {
      $result = $query->execute()->fetchCol();
    }

    return $result;
  }

  public static function getEntityIdSubsetByTargetStatusReady($entity_type, $nids, $lingotek_locale) {
    $query = db_select('lingotek_entity_metadata', 'l')
        ->fields('l', array('entity_id'))
        ->condition('entity_type', $entity_type)
        ->condition('entity_id', $nids, 'IN')
        ->condition('entity_key', 'target_sync_status_' . $lingotek_locale)
        ->condition('value', LingotekSync::STATUS_READY);
    $result = $query->execute()->fetchCol();

    return $result;
  }

  public static function updateNotifyUrl() {
    $security_token = md5(time());
    $new_url = lingotek_notify_url_generate($security_token);
    $api = LingotekApi::instance();
    $integration_method_id = variable_get('lingotek_integration_method', '');

    if (!strlen($integration_method_id)) { // request integration id when not already set, attempt to detect
      $params = array(
        'regex' => ".*"
      );
      $response = $api->request('searchOutboundIntegrationUrls', $params);
      if (isset($response->results) && $response->results) {
        global $base_url;
        $integration_methods = $response->integrationMethods;
        foreach ($integration_methods as $integration_method) {
          if (strpos($integration_method->url, $base_url) !== FALSE) {
            $integration_method_id = $integration_method->id; // prefer integration with matching base_url
          }
        }
        if (!strlen($integration_method_id)) {
          reset($integration_methods); // just in case the internal pointer is not pointing to the first element
          $integration_method = current($integration_methods); // grab the first element in the list
          $integration_method_id = $integration_method->id; // use the first url found (if no matching url was found previously)
        }
        variable_set('lingotek_integration_method', $integration_method_id);
      }
    }
    $parameters = array(
      'id' => $integration_method_id,
      'url' => $new_url
    );
    $response = $api->request('updateOutboundIntegrationUrl', $parameters);
    $success = isset($response->results) ? $response->results : FALSE;
    if ($success) {
      variable_set('lingotek_notify_url', $new_url);
      variable_set('lingotek_notify_security_token', $security_token);
      if (strpos($new_url,'localhost') !== false) {
        $success = 'localhost_url';
      }
    }
    return $success;
  }

  public static function getMenuLinkTargetStatus($mlid, $lingotek_locale) {
    $key = 'target_sync_status_' . $lingotek_locale;
    $status = lingotek_keystore('menu_link', $mlid, $key);

    if ($status) {
      return $status;
    }
    else {
      LingotekLog::error('Did not find any targets for menu link "@id"', array('@id' => $mlid));
      return FALSE;
    }
  }

  public static function getDocumentId($entity_type, $entity_id) {
    $doc_id = db_select('lingotek_entity_metadata', 'lem')
      ->fields('lem', array('value'))
      ->condition('lem.entity_type', $entity_type)
      ->condition('lem.entity_id', $entity_id)
      ->condition('lem.entity_key', 'document_id')
      ->execute()
      ->fetch(PDO::FETCH_ASSOC);

    if ($doc_id) {
      return $doc_id;
    }
    else {
      return FALSE;
    }
  }

  public static function setLastSyncError($entity_type, $entity_id, $error) {
    lingotek_keystore($entity_type, $entity_id, 'last_sync_error', $error);
  }

  public static function getLastSyncError($entity_type, $entity_id) {
    $error = '';
    $query = db_select('lingotek_entity_metadata', 'lem')
    ->fields('lem', array('value'))
    ->condition('lem.entity_id', $entity_id)
    ->condition('lem.entity_type', $entity_type)
    ->condition('lem.entity_key', 'last_sync_error');
    $last_sync_error = $query->execute()->fetchField();

    return $last_sync_error;
  }

  public static function deleteLastSyncError($entity_type, $entity_id) {
    lingotek_keystore_delete($entity_type, $entity_id, 'last_sync_error');
  }

  public static function updateLingotekTranslationAgentId($lids, $agent_id) {
    $query = db_update('locales_target')
      ->condition('lid', $lids, "IN")
      ->fields(array('translation_agent_id' => $agent_id))
      ->execute();
  }

  /**
   * Sets a Lingotek metadata value for this item.
   * @param int $lid
   *   The lid for the config item.
   * @param int $marked_offset
   *   Integer space reserved for marked value
   */
  public static function setConfigMarkedValue($lid, $marked_offset) {
    $insertRecord = array(
      "value" => $lid,
      "created" => time(),
      "modified" => time(),
      "id" => $marked_offset,
      "config_key" => 'marked'
    );
    $updateRecord = array(
      "value" => $lid,
      "modified" => time()
    );
    db_merge('lingotek_config_metadata')
      ->key(array('id' => $marked_offset, 'config_key' => 'marked'))
      ->insertFields($insertRecord)
      ->updateFields($updateRecord)
      ->execute();
  }

  /**
   * Deletes a Lingotek metadata value for this item.
   * @param int $lid
   *   The lid for the config item.
   * @param string $marked_offset
   *   Integer space reserved for marked value.
   */
  public static function deleteConfigMarkedValue($lid, $marked_offset) {
    db_delete('lingotek_config_metadata')
      ->condition('id', $marked_offset)
      ->condition('config_key', 'marked')
      ->condition('value', $lid)
      ->execute();
  }
}
