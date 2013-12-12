<?php

/**
 * @file
 * LingotekSync
 */

/**
 * A utility class for Lingotek Syncing.
 */
class LingotekSync {

  const STATUS_CURRENT = 'CURRENT';  // The node or target translation is current
  const STATUS_EDITED = 'EDITED';    // The node has been edited, but has not been uploaded to Lingotek
  const STATUS_PENDING = 'PENDING';  // The target translation is awaiting to receive updated content from Lingotek
  const STATUS_READY = 'READY';      // The target translation is complete and ready for download
  const STATUS_DISABLED = 'DISABLED';    // A disabled node should neither be uploaded nor downloaded by Lingotek
  const STATUS_TARGET = 'TARGET';    // A target node is being used to store a translation and should be ignored by Lingotek (used for node storage)

  const PROFILE_CUSTOM = 'CUSTOM';
  const PROFILE_DISABLED = 'DISABLED';

  public static function setNodeEnabled($nid, $enabled) {
    if($enabled) {
      if (lingotek_lingonode($nid, 'node_sync_status') == LingotekSync::STATUS_DISABLED) {
        lingotek_lingonode($nid, 'node_sync_status', LingotekSync::STATUS_EDITED);
        LingotekSync::setAllTargetStatus($nid, LingotekSync::STATUS_PENDING);
      }
    } else {
      lingotek_lingonode($nid, 'node_sync_status', LingotekSync::STATUS_DISABLED);
      LingotekSync::setAllTargetStatus($nid, LingotekSync::STATUS_DISABLED);
    }
  }
  
  public static function getTargetStatus($doc_id, $lingotek_locale) {
    $key = 'target_sync_status_' . $lingotek_locale;
    if ($chunk_id = LingotekConfigChunk::getIdByDocId($doc_id)) {
      return LingotekConfigChunk::getTargetStatusById($chunk_id, $lingotek_locale);
    }
    else {
      $node_id = self::getNodeIdFromDocId($doc_id);
      return lingotek_lingonode($node_id, $key);
    }
    LingotekLog::error('Did not find a node or chunk for Doc ID "@id"', array('@id' => $doc_id));
    return FALSE;
  }

  public static function getAllTargetStatusNotCurrent($nid) {
    $query = db_select('lingotek', 'l')
      ->fields('l', array('lingokey', 'lingovalue'))
      ->condition('lingokey', 'target_sync_status_%', 'LIKE')
      ->condition('lingovalue', 'CURRENT', '!=')
      ->condition('nid', $nid);
    $result = $query->execute()->fetchAll();
    return $result;
  }

  public static function setTargetStatus($node_id, $lingotek_locale, $status) {//lingotek_set_target_sync_status($node_id, $lingotek_locale, $node_status)
    $key = 'target_sync_status_' . $lingotek_locale;
    return lingotek_lingonode($node_id, $key, $status);
  }
  
  public static function setAllTargetStatus($nid, $status) {//lingotek_set_target_sync_status($node_id, $lingotek_locale, $node_status)
    $query = db_update('lingotek')
      ->condition('nid', $nid)
      ->condition('lingokey', 'target_sync_status%', 'LIKE')
      ->fields(array('lingovalue' => $status))
      ->execute();
  }

  public static function setNodeStatus($node_id, $status) {
    return lingotek_lingonode($node_id, 'node_sync_status', $status);
  }

  public static function getNodeStatus($node_id) {
    return lingotek_lingonode($node_id, 'node_sync_status');
  }

  public static function getSyncProjects() {
    $query = db_select('lingotek', 'l');
    $query->fields('l', array('lingovalue'));
    $query->condition('lingokey', 'project_id');
    $query->distinct();
    $result = $query->execute();
    $projects = $result->fetchCol();
    $default_project_id = variable_get('lingotek_project', NULL);
    if (!is_null($default_project_id) && !in_array($default_project_id, $projects)) {
      $projects[] = $default_project_id;
    }
    return $projects;
  }

  public static function setNodeAndTargetsStatus($node, $node_status, $targets_status) {
    // Set the Node to EDITED.
    self::setNodeStatus($node->nid, $node_status);

    $source_lingotek_locale = Lingotek::convertDrupal2Lingotek($node->language, TRUE);

    // Loop though each target language, and set that target to EDITED.
    $languages = Lingotek::availableLanguageTargets('lingotek_locale', FALSE, $source_lingotek_locale);
    foreach ($languages as $lingotek_locale) {
      self::setTargetStatus($node->nid, $lingotek_locale, $targets_status);
    }
  }

  // Add the node sync target language entries to the lingotek table.
  public static function insertTargetEntriesForAllNodes($lingotek_locale) {
    // select all nids where the node's source is the locale provided
    $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);
    $subquery = db_select('node', 'n')->fields('n', array('nid'));
    $subquery->condition('language', $drupal_language_code);

    $subquery2 = db_select('lingotek', 'l2')->fields('l2', array('nid'));
    $subquery2->condition('lingokey', 'target_sync_status_' . $lingotek_locale); //already has status

    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('lingokey', 'node_sync_status');
    $query->condition('nid', $subquery, 'NOT IN'); // exclude adding to nodes where this locale is the source
    $query->condition('nid', $subquery2, 'NOT IN'); // exclude nodes that already have this language as a target
    $result = $query->execute();

    while ($record = $result->fetchAssoc()) {
      $node_id = $record['nid'];
      // If the Node is CURRENT or PENDING, then we just need to pull down the new translation (because the source will have been uploaded), so set the Node and Target to PENDING.
      if ($record['lingovalue'] == self::STATUS_CURRENT) {
        self::setTargetStatus($node_id, $lingotek_locale, self::STATUS_PENDING);
      }
      else if ($record['lingovalue'] == self::STATUS_TARGET) { 
        
      }
      else if ($record['lingovalue'] == self::STATUS_DISABLED) { 
        self::setTargetStatus($node_id, $lingotek_locale, self::STATUS_DISABLED);
      }
      else { // Otherwise, set it to EDITED
        self::setNodeStatus($node_id, self::STATUS_EDITED);
        self::setTargetStatus($node_id, $lingotek_locale, self::STATUS_EDITED);
      }
    }
  }

  public static function resetTargetProgress($nid) {
    $query = db_select('lingotek', 'l')
      ->fields('l', array('lingokey'))
      ->condition('nid', $nid)
      ->condition('lingokey', 'target_sync_status_%', 'LIKE');
    $targets_raw = $query->execute()->fetchCol();
    foreach ($targets_raw as $target_raw) {
      $target = str_replace('target_sync_status_', '', $target_raw);
      $fields[] = array(
        'nid' => $nid,
        'lingokey' => 'target_sync_progress_' . $target,
        'lingovalue' => 0,
      );
      $fields[] = array(
        'nid' => $nid,
        'lingokey' => 'target_sync_last_progress_updated_' . $target,
        'lingovalue' => time(),
      );
    }
    $fields[] = array(
      'nid' => $nid,
      'lingokey' => 'translation_progress',
      'lingovalue' => 0,
    );

    $delete = db_delete('lingotek')
      ->condition('nid', $nid);
    $or = db_or();
      $or->condition('lingokey', 'target_sync_progress_%', 'LIKE');
      $or->condition('lingokey', 'target_sync_last_progress_updated_%', 'LIKE');
      $or->condition('lingokey', 'translation_progress');
    $delete->condition($or)
      ->execute();

    $insert = db_insert('lingotek')
      ->fields(array('nid', 'lingokey', 'lingovalue'));
    foreach ($fields as $field) {
      $insert->values($field);
    }
    $insert->execute();
  }
  
  public static function insertTargetEntriesForAllChunks($lingotek_locale) {
    // insert/update a target language for all chunks
    $query = db_select('lingotek_config_metadata', 'meta')
        ->fields('meta', array('id'))
        ->groupBy('id');
    $ids = $query->execute()->fetchCol();

    foreach ($ids as $i) {
      $chunk = LingotekConfigChunk::loadById($i);
      $chunk->setChunkTargetsStatus(self::STATUS_PENDING, $lingotek_locale);
    }
  }

  public static function insertTargetEntriesForAllDocs($lingotek_locale) {
    self::insertTargetEntriesForAllNodes($lingotek_locale);
    self::insertTargetEntriesForAllChunks($lingotek_locale);
  }

  // Remove the node sync target language entries from the lingotek table lingotek_delete_target_sync_status_for_all_nodes
  public static function deleteTargetEntriesForAllNodes($lingotek_locale) {
    $keys = array(
      'target_sync_status_' . $lingotek_locale,
      'target_sync_last_progress_updated_' . $lingotek_locale,
      'target_sync_progress_' . $lingotek_locale,
      'target_last_downloaded_' . $lingotek_locale,
    );
    db_delete('lingotek')->condition('lingokey', $keys, 'IN')->execute();
  }

  public static function deleteTargetEntriesForAllChunks($lingotek_locale) {
    $key = 'target_sync_status_' . $lingotek_locale;
    db_delete('lingotek_config_metadata')->condition('config_key', $key)->execute();
  }

  public static function deleteTargetEntriesForAllDocs($lingotek_locale) {
    self::deleteTargetEntriesForAllNodes($lingotek_locale);
    self::deleteTargetEntriesForAllChunks($lingotek_locale);
  }
  
  
  /**
   * getDocIdTargetsByStatus
   * 
   * @param status (e.g., LingotekSync::READY)
   * 
   * @return an array of associate arrays.  Each associate array will have a 'nid' (e.g., 5), 'locale' (e.g., 'de_DE'), and optionally 'doc_id' (e.g., 46677222-b5ec-47d5-880e-24632feffaf5)
   */
  public static function getTargetsByStatus($status, $include_doc_ids = FALSE) {
    $target_language_search = '%';
    $query = db_select('lingotek', 'l');
    $query->fields('l', array('nid', 'lingokey', 'lingovalue'));
    $query->condition('lingokey', 'target_sync_status_' . $target_language_search, 'LIKE');
    $query->condition('lingovalue', $status);
    $result = $query->execute();
    $records = $result->fetchAll(); //$result->fetchAllAssoc('nid');

     // build nid_doc_map (if needed)
    if ($include_doc_ids) {
      $nid_doc_map = array();
      foreach ($records as $record) {
        if (!key_exists($record->nid, $nid_doc_map)) {
          $doc_id = self::getDocIdFromNodeId($record->nid);
          $nid_doc_map[$record->nid] = $doc_id;
        }
      }
    }
    $targets = array();
    foreach ($records as $record) {
      $doc_target = array(
        'nid' => $record->nid,
        'doc_id' => $include_doc_ids ? $nid_doc_map[$record->nid] : NULL,
        'locale' => str_replace('target_sync_status_', '', $record->lingokey)
      );
      $targets[] = $doc_target;
    }

    return $targets;
  }

  public static function getDownloadableReport() {
    $document_ids = array_unique(array_merge(self::getDocIdsByStatus(self::STATUS_PENDING, FALSE), self::getDocIdsByStatus(self::STATUS_READY, FALSE)));

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
    $response = lingotek_get_and_update_target_progress($document_ids);

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

    /*
    if (isset($response->byDocumentIdAndTargetLocale)) {
      $status_by_doc = $response->byDocumentIdAndTargetLocale;
    }
    if (isset($response->byTargetLocale)) {
      $status_by_locale = $response->byTargetLocale;
    }
    if (isset($response->workflowCompletedByDocumentIdAndTargetLocale)) {
      $progress_report = $response->workflowCompletedByDocumentIdAndTargetLocale;
      foreach ($progress_report as $doc_id => $target_locales) {
        foreach ($target_locales as $lingotek_locale => $workflow_completed) {
          if (isset($status_by_doc->$doc_id) && isset($status_by_doc->$doc_id->$lingotek_locale)) {
            $status = $status_by_doc->$doc_id->$lingotek_locale;
          }
          $doc_target = array(
            'document_id' => $doc_id,
            'locale' => $lingotek_locale,
            'percent_complete' => $status,
          );

          $target_status = self::getTargetStatus($doc_id, $lingotek_locale);

          if ($workflow_completed && $target_status != self::STATUS_EDITED && $target_status != self::STATUS_DISABLED) {
            if ($target_status == self::STATUS_READY || $target_status == self::STATUS_PENDING) {
              $report['download_targets_workflow_complete'][] = $doc_target;
              $report['download_targets_workflow_complete_count']++;
            }
            else {
              // Target already downloaded
            }
          }
          elseif (!$workflow_completed) {
            $report['download_targets_workflow_incomplete'][] = $doc_target;
            $report['download_targets_workflow_incomplete_count']++;
          }
        }
      }
    }
    */
    return $report;
  }

  //lingotek_count_nodes
  public static function getNodeCountByStatus($status) {
    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('lingokey', 'node_sync_status');
    $query->condition('lingovalue', $status);
    $result = $query->countQuery()->execute()->fetchField();
    return $result;
  }

  //lingotek_count_chunks
  public static function getChunkCountByStatus($status) {
    $all_lids = count(self::getAllChunkLids());
    $dirty_lids = count(self::getDirtyChunkLids());
    $current_lids = $all_lids - $dirty_lids;
    $chunk_size = LINGOTEK_CONFIG_CHUNK_SIZE;
    $num_edited_docs = round(($all_lids - $dirty_lids) / $chunk_size);
    $num_total_docs = round($all_lids / $chunk_size);
    $num_pending_docs = count(self::getChunksWithPendingTranslations());
    $num_curr_docs = $num_total_docs - $num_edited_docs - $num_pending_docs;
    $num_curr_docs = ($num_curr_docs > 0 ? $num_curr_docs : 0);

    if ($status == self::STATUS_EDITED) {
      return $num_edited_docs;
    }
    elseif ($status == self::STATUS_PENDING) {
      return $num_pending_docs;
    }
    elseif ($status == self::STATUS_CURRENT) {
      return $num_curr_docs;
    }
    LingotekLog::error('Unknown config-chunk status: @status', array('@status' => $status));
    return 0;
  }

  //lingotek_count_total_source
  public static function getCountByStatus($status) {
    $count = 0;
    $count += self::getNodeCountByStatus($status);
    // (turned off reporting of config chunks, for now)
    /*
    if (variable_get('lingotek_translate_config', 0)) {
      $count += self::getChunkCountByStatus($status);
    }
     */
    return $count;
  }

  public static function getTargetNodeCountByStatus($status, $lingotek_locale) {
    $target_prefix = 'target_sync_status_';
    $target_key = $target_prefix . $lingotek_locale;

    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('lingokey', $target_key);
    $query->condition('lingovalue', $status);
    $result = $query->countQuery()->execute()->fetchAssoc();

    $count = 0;
    if (is_array($result)) {
      $count = array_shift($result);
    }

    // count nodes having this language as the source as current
    if ($status == LingotekSync::STATUS_CURRENT) {
      $nids = LingotekSync::getAllNodeIds();
      $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale, TRUE);
      $query = db_select('node', 'n');
      $query->join('lingotek', 'l', 'l.nid = n.nid
         AND l.lingokey = \'node_sync_status\'
         AND l.lingovalue != \'' . LingotekSync::STATUS_DISABLED .  '\'
           AND l.lingovalue != \'' . LingotekSync::STATUS_TARGET  .  '\'');
      $query->condition('n.language', $drupal_language_code);
      if (count($nids)) {
        $query->condition('n.nid', $nids, 'IN'); // nodes sent to lingotek
      }
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
    $subquery = db_select('lingotek', 'l1')
      ->fields('l1', array('nid'))
      ->condition('l1.lingokey', 'document_id')
      ->condition('l1.lingovalue', $document_ids, 'IN');
    $query = db_select('lingotek', 'l');
    $query->fields('l', array('nid'));
    $query->condition('l.lingokey', 'target_sync_status_%', 'LIKE');
    $query->condition('l.nid', $subquery, 'IN');
    $query->addExpression('COUNT(l.lingokey)', 'targets');

    $query->groupBy('l.nid');
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

  public static function getNonWorkbenchModerationNodeIds($edited_nodes) {
    $sub_query = db_select('workbench_moderation_node_history', 'wb') // get nids for unmoderated nodes
        ->fields('wb', array('nid'));
    $query = db_select('node_revision', 'nr')
        ->distinct(TRUE)
        ->fields('nr', array('nid'))
        ->condition('nid', $sub_query, 'NOT IN')
        ->condition('nid', $edited_nodes, 'IN');
    $no_wb_mod = $query->execute()->fetchCol(0);
    return $no_wb_mod;
  }

  protected static function getQueryCompletedConfigTranslations($drupal_codes) {
    // return a query object that contains all fully-translated/current strings
    // or ones that were not translated by Lingotek.
    // use the first addtl language as the query's base.
    $first_lang = array_shift($drupal_codes);
    $lingotek_id = LingotekConfigChunk::getLingotekTranslationAgentId();
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

  public static function getConfigChunk($chunk_id) {
    // return LingotekConfigChunk object containing all segments
    // for the given chunk id.
    return LingotekConfigChunk::loadById($chunk_id);
  }

  public static function getAllChunkLids() {
    // return the list of all lids
    $query = db_select('locales_source', 'ls')
        ->fields('ls', array('lid'));
    return $query->execute()->fetchCol();
  }

  public static function getDirtyChunkLids() {
    // return the list of all lids from the locale_source table *not* fully translated
    $source_language = language_default();
    if (!isset($source_language->lingotek_locale)) {
      $source_language->lingotek_locale = Lingotek::convertDrupal2Lingotek($source_language->language);
    }
    $lingotek_codes = Lingotek::availableLanguageTargetsWithoutSource($source_language->lingotek_locale);
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
    $query = db_select('locales_source', 'ls');
    $query->fields('ls', array('lid'))
        ->condition('ls.source', '', '!=')
        ->condition('ls.textgroup', LingotekConfigChunk::getTextgroupsForTranslation(), 'IN')
        ->condition('ls.lid', self::getQueryCompletedConfigTranslations($drupal_codes), 'NOT IN');
    return $query->execute()->fetchCol();
  }

  public static function getDirtyConfigChunks() {
    // return the set of chunk IDs, which are the chunks that contain
    // lids that are in need of some translation.  These IDs are calculated
    // as the segment ID of the first segment in the chunk, divided by
    // the configured chunk size.  So, segments 1 through [chunk size] would
    // be in chunk #1, etc.
    $lids = self::getDirtyChunkLids();
    $chunk_ids = array();
    foreach ($lids as $lid) {
      $id = LingotekConfigChunk::getIdBySegment($lid);
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
    // Handle nodes
    $edited_nodes = self::getNodeIdsByStatus(self::STATUS_EDITED, TRUE);
    $report = array(
      'upload_nids' => $edited_nodes,
      'upload_nids_count' => count($edited_nodes)
    );
    if (module_exists('entity_translation')) {
      $et_nodes = self::getETNodeIds();
      $report = array_merge($report, array(
        'upload_nids_et' => $et_nodes,
        'upload_nids_et_count' => count($et_nodes)
      ));
    }
    if (module_exists('workbench_moderation')) {
      $no_wb_nodes = empty($edited_nodes) ? array() : self::getNonWorkbenchModerationNodeIds($edited_nodes);
      $report = array_merge($report, array(
        'upload_nids_nowb' => $no_wb_nodes,
        'upload_nids_nowb_count' => count($no_wb_nodes)
      ));
    }
    // Handle configuration chunks
    if (variable_get('lingotek_translate_config')) {
      $config_chunks_to_update = self::getDirtyConfigChunks();
      $num_updates = count($config_chunks_to_update);
      $report = array_merge($report, array(
        'upload_config' => (array_keys($config_chunks_to_update)),
        'upload_config_count' => $num_updates,
      ));
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
    $query = db_select('lingotek', 'l');
    $query->fields('l', array('nid'));
    if($source) {
      $query->condition('lingokey', 'node_sync_status');
    } else {
      $query->condition('lingokey', 'target_sync_status_%', 'LIKE');
    }
    $query->condition('lingovalue', $status);
    $query->distinct();
    $result = $query->execute();
    $nids = $result->fetchCol();
    return $nids;
  }

  public static function getNodeIdsByStatusAndTarget($status, $target_language = '%') {
    $query = db_select('lingotek', 'l')
      ->distinct()
      ->fields('l', array('nid'))
      ->condition('lingokey', 'target_sync_status_' . $target_language, 'LIKE')
      ->condition('lingovalue', $status);
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getNodeIdsBySource($language) {
    $sub_query = db_select('lingotek', 'l')
        ->distinct()
        ->fields('l', array('nid'));
    $query = db_select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('n.language', $language)
        ->condition('n.nid', $sub_query, 'IN');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getNodeIdsByTarget($target_language) {
    $query = db_select('lingotek', 'l1')
        ->fields('l1', array('nid'))
        ->condition('lingokey', 'target_sync_status_' . $target_language);
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getDocIdsByStatus($status, $source) {
    $doc_ids = array();

    // retrieve document IDs from nodes
    $nids = self::getNodeIdsByStatus($status, $source);
    if (!empty($nids)) {
      $query = db_select('lingotek', 'l');
      $query->fields('l', array('lingovalue'));
      $query->condition('lingokey', 'document_id');
      $query->condition('nid', $nids);
      $result = $query->execute();
      $doc_ids = $result->fetchCol();
    }

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

  public static function disassociateAllNodes() {
    db_truncate('lingotek');
  }

  public static function disassociateAllEntities() {
    db_truncate('lingotek_entity_metadata')->execute();
  }

  public static function disassociateAllChunks() {
    db_truncate('lingotek_config_metadata')->execute();
  }

  public static function disassociateNodes($document_ids = array()) {
    $nids = self::getNodeIdsFromDocIds($document_ids);
    db_delete('lingotek')
      ->condition('nid', $nids, 'IN')
      ->execute();
  }

  public static function resetNodeInfoByDocId($lingotek_document_id) {
    $doc_ids = is_array($lingotek_document_id) ? $lingotek_document_id : array($lingotek_document_id);
    $count = 0;
    foreach ($doc_ids as $doc_id) {
      $node_id = LingotekSync::getNodeIdFromDocId($doc_id); // grab before node info is removed
      LingotekSync::removeNodeInfoByDocId($doc_id); //remove locally (regardless of success remotely)
      if ($node_id !== FALSE) {
        LingotekSync::setNodeStatus($node_id, LingotekSync::STATUS_EDITED);
        $count++;
      }
    }
    return $count;
  }

  public static function removeNodeInfoByNodeId($nid) {
    $query = db_delete('lingotek');
    $query->condition('nid', $nid);
    $query->condition('lingokey', 'profile', '<>');
    $result = $query->execute();
  }

  public static function removeNodeInfoByDocId($lingotek_document_id) {
    $doc_ids = is_array($lingotek_document_id) ? $lingotek_document_id : array($lingotek_document_id);
    $count = 0;
    foreach ($doc_ids as $doc_id) {
      $nid = self::getNodeIdFromDocId($doc_id);
      if ($nid) {
        self::removeNodeInfoByNodeId($nid);
        $count++;
      }
    }
    return $count;
  }

  public static function getAllLocalDocIds() {
    // node-related doc IDs
    $query = db_select('lingotek', 'l');
    $query->fields('l', array('lingovalue'));
    $query->condition('lingokey', 'document_id');
    $query->distinct();
    $result = $query->execute();
    $doc_ids = $result->fetchCol();

    // entity-related doc IDs
    $query = db_select('lingotek_entity_metadata', 'l');
    $query->fields('l', array('value'));
    $query->condition('entity_key', 'document_id');
    $query->distinct();
    $result = $query->execute();
    $doc_ids = array_merge($doc_ids, $result->fetchCol());

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
    $query = db_select('lingotek', 'l');
    $query->fields('l', array('nid'));
    //$query->condition('lingokey', 'document_id');
    $query->distinct('nid');
    $result = $query->execute();
    $nids = $result->fetchCol();
    return $nids;
  }

  public static function getAllDocIds() {
    $query = db_select('lingotek', 'l')
        ->fields('l', array('lingovalue'))
        ->condition('lingokey', 'document_id');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getDocIdsBySource($source_language) {
    $subquery = db_select('node', 'n')
        ->fields('n', array('nid'))
        ->condition('language', Lingotek::convertLingotek2Drupal($source_language));
    $query = db_select('lingotek', 'l')
        ->fields('l', array('lingovalue'))
        ->condition('lingokey', 'document_id')
        ->condition('nid', $subquery, 'IN');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getDocIdsByTarget($target_language) {
    $subquery = db_select('lingotek', 'l1')
        ->fields('l1', array('nid'))
        ->condition('lingokey', 'target_sync_status_' . $target_language);
    $query = db_select('lingotek', 'l2')
        ->fields('l2', array('lingovalue'))
        ->condition('lingokey', 'document_id')
        ->condition('nid', $subquery, 'IN');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  //lingotek_get_node_id_from_document_id
  public static function getNodeIdFromDocId($lingotek_document_id) {
    $found = FALSE;
    $key = 'document_id';

    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('lingokey', $key);
    $query->condition('lingovalue', $lingotek_document_id);
    $result = $query->execute();

    if ($record = $result->fetchAssoc()) {
      $found = $record['nid'];
    }

    return $found;
  }

  public static function getNodeIdsFromDocIds($lingotek_document_ids) {
    $nids = array();
    $query = db_select('lingotek', 'l')
        ->fields('l', array('nid'))
        ->condition('lingokey', 'document_id')
        ->condition('lingovalue', $lingotek_document_ids, 'IN');
    $result = $query->execute()->fetchCol();
    return $result;
  }

  public static function getDocIdFromNodeId($drupal_node_id) {
    $found = FALSE;

    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('nid', $drupal_node_id);
    $query->condition('lingokey', 'document_id');
    $result = $query->execute();

    if ($record = $result->fetchAssoc()) {
      $found = $record['lingovalue'];
    }

    return $found;
  }

  public static function getDocIdsFromNodeIds($drupal_node_ids, $associate = FALSE) {
    $query = db_select('lingotek', 'l')
        ->condition('nid', $drupal_node_ids, 'IN')
        ->condition('lingokey', 'document_id');
    $query->addField('l', 'lingovalue', 'doc_id');

    if ($associate) {
      $query->addField('l', 'nid');
      $result = $query->execute()->fetchAllAssoc('nid');
    }
    else {
      $result = $query->execute()->fetchCol();
    }

    return $result;
  }

  public static function getNodeIdSubsetByTargetStatusReady($nids, $lingotek_locale) {
    $query = db_select('lingotek', 'l')
        ->fields('l', array('nid'))
        ->condition('nid', $nids, 'IN')
        ->condition('lingokey', 'target_sync_status_' . $lingotek_locale)
        ->condition('lingovalue', LingotekSync::STATUS_READY);
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
    }
    return $success;
  }

}

?>
