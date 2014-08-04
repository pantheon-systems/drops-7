<?php

/**
 * @file
 * Call the Lingotek APIs
 */
$GLOBALS['_lingotek_client'] = new LingotekSession();

/*
 * This function is only used for node-based translations.
 * 
 * @param $node
 *  Node in the source language
 * @param $drupal_language_code
 *  The language which we are dealing with
 */

function lingotek_get_translated_node($node, $drupal_language_code) {
  $tset = translation_node_get_translations($node->nid);

  if ($node->tnid == 0) {
    $node->tnid = $node->nid;
    db_update('{node}')
        ->fields(array('tnid' => $node->nid))
      ->condition('nid', $node->nid)
      ->execute();
  }

  if (isset($tset[$drupal_language_code])) {
    $localized_node = node_load($tset[$drupal_language_code]->nid);
  }
  else {
    $localized_node = new stdClass();
    $localized_node->type = $node->type;
    node_object_prepare($localized_node);

    // Grandfather the lingotek settings (ie. profile, etc.) on the new node.
    if (!empty($node->lingotek)) {
      $localized_node->lingotek = $node->lingotek;
    }
    $localized_node->title = $node->title . ' (' . $drupal_language_code . ')';
    $localized_node->tnid = $node->tnid;
    $localized_node->language = $drupal_language_code;


    $localized_node->uid = $node->uid;
    $localized_node->name = $node->name;
    $localized_node->comment = $node->comment;
    $localized_node->promote = $node->promote;
    $localized_node->sticky = $node->sticky;
    $localized_node->status = $node->status;
    $localized_node->create_lingotek_document = FALSE;
    
    $lingotek_fields = variable_get('lingotek_enabled_fields');
    foreach ($lingotek_fields['node'][$localized_node->type] as $field_name) {
      $field = $node->$field_name;
      $f = array();
      if (isset($field[$node->language])) {
        foreach ($field[$node->language] as $key => $value) {
          if (isset($value['format']))
            $f[$drupal_language_code][$key]['format'] = $value['format'];
        }
      }
      
      $localized_node->$field_name = $f;
    }
        
    node_save($localized_node);
    lingotek_keystore('node', $localized_node->nid, 'node_sync_status', 'TARGET');
    // Child node should keep its parent node's profile, for rules integration.
    $parent_profile = lingotek_keystore('node', $node->nid, 'profile');
    if ($parent_profile !== FALSE) {
      lingotek_keystore('node', $localized_node->nid, 'profile', $parent_profile);
    }
  }

  return $localized_node;
}

function lingotek_process_entity_xml($xml, $entity, $entity_type, $drupal_language_code, $node_based_translation = FALSE, $url_alias_translation = 0) {
  list($id, $vid, $bundle) = lingotek_entity_extract_ids($entity_type, $entity);

  // Set $node_based if the entity type is a node, inherit otherwise.
  if ($entity_type == 'node') {
    $node_based_translation = lingotek_is_node_translation($entity) ? TRUE : FALSE;
  }
  // Must check for entity type and node-based translation, as the entity type
  // could be a field collection nested within a node-based node.
  $source_entity = $entity_type == 'node' && $node_based_translation ? lingotek_get_source_node($entity) : $entity;

  // Add revision_id to be the same as the entity_id if it was not found or if
  // it is a comment.
  if ($vid === NULL || $entity_type == 'comment') {
    $vid = $id;
  }
  
  $delta = 0;
  $last_tag = NULL;
  foreach ($xml as $tag => $content) {
    if ($tag == $last_tag) {
      $delta++;
    }
    else {
      $delta = 0;
    }
    if ($tag == 'url_alias' && $url_alias_translation == 1) {
      $target = check_plain($content);

      //URL Alias related to the page:
      $conditions = array('source' => 'node/' . $id);
      if ($entity->language != LANGUAGE_NONE) {
        $conditions['language'] = $entity->language;
      }
      $path = path_load($conditions);
      if ($path !== FALSE) {
        $conditions['language'] = $drupal_language_code;
        if ($path['alias'] != $target || $entity->language != $drupal_language_code) {
          $original = path_load($conditions);
          $conditions['alias'] = $target;
          if ($original === FALSE) {
            path_save($conditions);
          }
          else {
            path_delete($original);
            path_save($conditions);
          }
        }
      }
    }
    else { //this part of the xml is a field
      $drupal_field_name = $tag;
      $field = field_info_field($drupal_field_name);
      $source_field_data = field_get_items($entity_type, $source_entity, $drupal_field_name);
      $translatable_field = !empty($field['translatable']);
      $field_language = $node_based_translation && !$translatable_field ? LANGUAGE_NONE : $drupal_language_code;
      $target_key = 'value';
      $subfield_parts = explode('__', $tag);
      if (count($subfield_parts) == 2) {
        $drupal_field_name = $subfield_parts[0];
        $target_key = $subfield_parts[1];
      }
      if (isset($field)) {
        $curr_field_data = & $entity->$drupal_field_name;
        $index = 0;
        if (module_exists('link') && $field['type'] == 'link_field') {
          $target_key = array(
            'title',
            'url',
          );
        }
        if (!is_array($target_key)) {
          $target_key = array($target_key);
        }
        $insert_array = array(
          'entity_type' => $entity_type,
          'bundle' => $bundle,
          'entity_id' => $id,
          'revision_id' => $vid,
          'language' => $field_language,
          'delta' => $delta,
          'deleted' => '0',
        );
        $field_names = array('field_revision_' . $field['field_name'], 'field_data_' . $field['field_name']);

        if (module_exists('field_collection') && $field['type'] == 'field_collection') {
          $default = language_default();
          $default_language = $default->language;
          if (isset($curr_field_data[LANGUAGE_NONE][$delta]['value'])) {
            $field_collection_id = $curr_field_data[LANGUAGE_NONE][$delta]['value'];
          }
          elseif (isset($curr_field_data[$default_language][$delta]['value'])) {
            $field_collection_id = $curr_field_data[$language_default][$delta]['value'];
          }
          else {
            if (!$node_based_translation) {
              // The field-collection field must be empty.
              continue;
            }
            // If it does not exist and the profile is node-based, create a new FC.
            $field_collection_item = entity_create('field_collection_item', array('field_name' => $field['field_name']));
            $field_collection_item->setHostEntity($entity_type, $entity);
            $field_collection_item->save();
            $field_collection_id = $field_collection_item->item_id;
          }
          $field_collection_item = lingotek_entity_load_single('field_collection_item', $field_collection_id);
          if (!$field_collection_item) {
            // The field collection was removed, so disregard any info on it
            continue;
          }
          $field_collection_item->type = $field['field_name'];
          $field_collection_item->language = $entity->language;
          $field_collection_item->nid = $field_collection_item->item_id;
          $field_collection_item->vid = $field_collection_item->revision_id;

          // RECURSION FOR FIELD COLLECTIONS
          lingotek_process_entity_xml($content, $field_collection_item, 'field_collection_item', $drupal_language_code, $node_based_translation);
          $last_tag = $tag;
          continue;
        }
        else {
          foreach ($content as $text) {
            // Set all field-collection fields to translatable, if not node-based.
            if ($entity_type == 'field_collection_item' && !$node_based_translation && $field['translatable'] != 1) {
              $field['translatable'] = 1;
              field_update_field($field);
            }

            $is_link = in_array('url', $target_key);
            $without_title = (count($content) == 1);

            if ($is_link && $without_title) {
              $array_key = $target_key[1];
            }
            else {
              $array_key = $target_key[$index];
            }
            $db_field_name = $field['field_name'] . '_' . $array_key;
            $insert_array[$db_field_name] = lingotek_unfilter_placeholders(decode_entities($text));
            if ($db_field_name == 'title_field_value' && strlen($insert_array[$db_field_name]) > 254) {
              $insert_array[$db_field_name] = substr($insert_array[$db_field_name], 0, 254);
              $language_list = language_list('language');
              $langcode = $insert_array['language'];
              LingotekLog::info('The @lang (@langcode) title was truncated, since the translation exceeded the maximum of 255 characters.', array('@lang' => $language_list[$langcode]->name, '@langcode' => Lingotek::convertDrupal2Lingotek($langcode)));
            }

            // Assign the field format.
            $field_format = NULL;
            if (!empty($source_field_data[0]['format'])) {
              $field_format = $source_field_data[0]['format'];
            }
            elseif (!empty($curr_field_data[$entity->language][0]['format'])) {
              $field_format = $curr_field_data[$entity->language][0]['format'];
            }
            if (!empty($field_format)) {
              $format_db_field_name = $field['field_name'] . '_format';
              $insert_array[$format_db_field_name] = $field_format;
            }
            $index++;
          }
        }

        foreach ($field_names as $field_name) {
          // using drupal_write_record to avoid node_save - node_save overwrites publications unless called on both revised and published versions of the node (i.e. workbench_moderation)
          // UPDATE: This could perhaps be rewritten now that workbench_moderation is supported through the rules module
          try {
            drupal_write_record($field_name, $insert_array);
          } catch (PDOException $e) {
            $primary_keys = array(
              'entity_type',
              'entity_id',
              'revision_id',
              'deleted',
              'delta',
              'language',
            );
            drupal_write_record($field_name, $insert_array, $primary_keys);
          }
        }
      }

      cache_clear_all('field:' . $entity_type . ':' . $id, 'cache_field');

      //Set URL alias
      if ($tag == 'title_field' && $url_alias_translation == 2 && module_exists('pathauto') && $entity->language != LANGUAGE_NONE) {
        module_load_include('inc', 'pathauto');
        $uri = entity_uri('node', $entity);
        $entity_unchanged = entity_load_unchanged('node', $id);
        pathauto_create_alias('node', 'update', $uri['path'], array('node' => clone $entity_unchanged), $entity->type, $drupal_language_code);
      }
    }
    $last_tag = $tag;
  }
}

/*
 * Save a segment
 *
 * Helper method so that if a node has the tnid deferred until later, it will
 * get it from the database instead.
 *
 * @param $source_text
 *  Source text
 * @param $target_text
 *  Target text that should be saved in the segment
 * @param $target_language
 *  Target language as used by lingotek (locale_country)
 * @param $doc_id
 *  Document Id
 * @return
 *  boolean, TRUE if the api call was successful
 */

function lingotek_save_segment($source_text, $target_text, $target_language, $doc_id) {
  $param = array
    (
    "sourceText" => $source_text,
    "targetText" => $target_text,
    "targetLanguage" => $target_language,
    "documentId" => $doc_id,
    "overwrite" => 0,
  );
  $save_segment = LingotekApi::instance()->request("saveSegment", $param);
  return ($save_segment->results == "success");
}

#API ADDERS

/*
 * Apply the phase template to the Lingotek document
 *
 * This saves the chosen workflow to the Lingotek platform.
 *
 * @param $translation_target_id
 *  Translation Target Id (Id for the target language stored with the document associated with a node)
 * @param $phase_template_id
 *  Workflow Id to be added
 */

function lingotek_add_phase_template($translation_target_id, $phase_template_id) {
  $params = array('translationTargetId' => $translation_target_id, 'phaseTemplateId' => $phase_template_id);
  LingotekApi::instance()->request("applyPhaseTemplate", $params);
}

/*
 * Create a project and return it's id.
 * @param $name
 *  Project name being created
 */

function lingotek_add_project($name) {
  $output = LingotekApi::instance()->request('addProject', array('projectName' => $name));
  if ($output->results == "success") {
    variable_set('lingotek_project', $output->id);
    return $output->id;
  }
}

/*
 * Create a vault, and return it's id.
 * @param $name
 *  Vault name being created
 */

function lingotek_add_vault($name) {
  $output = LingotekApi::instance()->request('addTMVault', array('tmVaultName' => $name));
  if ($output->results == "success") {
    variable_set('lingotek_vault', $output->id);
    return $output->id;
  }
}

/**
 * Add the current vault to the current project.  It doesn't hurt to call this more than once.
 */
function lingotek_add_vault_to_project() {
  $vault_id = variable_get('lingotek_vault', '');
  $project_id = variable_get('lingotek_project', '');
  if ($vault_id != '' && $project_id != '') {
    $param = array(
      'project_id' => $project_id,
      'index_id' => $vault_id
    );
    LingotekApi::instance()->request('addProjectTMVault', $param);
  }
}

/**
 * Analyze the Project
 */
function lingotek_analyze_project() {
  LingotekApi::instance()->request("analyzeProject", array('projectId' => variable_get('lingotek_project', -1)));
}

#GETTERS

/*
 * Get available URL alias methods
 */

function lingotek_get_url_alias_translations() {
  $methods = array();
  $methods[0] = t("Don't translate");
  $methods[1] = t("Translate the URL alias");
  $methods[2] = t("Use the translated page title");
  return $methods;
}

/*
 * Get the Lingotek user's cms key for the community they are currently logged in with
 */

function lingotek_get_cms_key() {
  global $_lingotek_client;

  $output = LingotekApi::instance()->request("getCMSKey");
  if ($output->results == "success") {
    variable_del('lingotek_password');
    return $output->cms;
  }
  else {
    return "";
  }
}

/*
 * Get the Lingotek user's current communities
 */

function lingotek_get_communities() {
  $options = array();

  if (!$_lingotek_client->canLogIn()) {
    return $options;
  }

  $list_communities = LingotekApi::instance()->request("listCommunities", array());
  if ($list_communities->results == "success") {
    foreach ($list_communities->communities as $community) {
      $options[$community->id] = $community->name;
    }
  }

  return $options;
}

/**
 * Get the target language objects for a Lingotek document associated with a node.
 *
 * @param int $document_id
 *   A Lingotek Document ID.
 * @param bool $flush_cache
 *   Whether or not to force a refresh from the server, as opposed to using cached data.
 *
 * @return array
 *   An array of translation target items.
 */
function lingotek_get_document_targets($document_id, $flush_cache = FALSE) {
  global $_lingotek_client;

  $targets = &drupal_static(__FUNCTION__);

  // Use static cache to ensure that we don't go to the server more than once per page for targets.
  if (isset($targets[$document_id])) {
    return $targets[$document_id];
  }

  $results = array();
  $cache_id = 'lingotek_targets_' . $document_id;
  $cache = cache_get($cache_id);
  if (lingotek_do_cache() && !$flush_cache && !empty($cache->data)) {
    LingotekLog::trace("lingotek_get_document_targets USING CACHE", array('document_id' => $document_id, 'flushCache' => $flush_cache));
    $results = $cache->data;
  }
  else {
    $output = LingotekApi::instance()->getDocument($document_id);
    if (!empty($output->translationTargets)) {
      foreach ($output->translationTargets as $target) {
        $results[$target->language] = $target;
      }
    }
    LingotekLog::trace("lingotek_get_document_targets GENERATING NEW CACHE DATA getDocument", array('document_id' => $document_id, 'flushCache' => $flush_cache));
    $targets[$document_id] = $results;
    if (!empty($results)) {
      cache_set($cache_id, $results, 'cache', time() + 900);
    }
  }

  return $results;
}

/**
 * Gets the phase name of the specified phase.
 *
 * This fetches a workflow step's name (known as a Phase in the Lingotek platform).
 *
 * @param int $phase_id
 *   A Lingotek phase ID.
 *
 * @return string
 *   Name for the workflow step (phase name).
 *
 * @todo Move the actual call to getPhase onto LingotekApi class.
 */
function lingotek_get_phase_name($phase_id) {
  $phases = &drupal_static(__FUNCTION__);

  $phase_name = '';

  if (!empty($phases[$phase_id])) {
    $phase_name = $phases[$phase_id]->name;
  }
  else {
    $params = array('phaseId' => $phase_id);
    $output = LingotekApi::instance()->request('getPhase', $params);

    if ($output->results == 'success') {
      $phases[$phase_id] = $output;
      $phase_name = $output->name;
    }
  }

  return $phase_name;
}

/*
 * Get available synchronization methods for keeping nodes up-to-date
 */

function lingotek_get_sync_methods() {
  $methods = array();
  $methods[0] = t("Never"); // Manual
  $methods[1] = t("Always"); // Automatic
  $methods[100] = t("100%");
  return $methods;
}

/*
 * Get the translation target
 *
 * This fetches an target language object for a specific document.
 *
 * @param $translation_target_id
 *  Id for the target language object
 * @return
 *  Object representing a target language for a specific document in the lingotek platform
 */

function lingotek_get_translation_target($translation_target_id) {
  $params = array('translationTargetId' => $translation_target_id);
  $output = LingotekApi::instance()->request("getTranslationTarget", $params);
  if ($output->results == "success") {
    return $output;
  }
}

/**
 * Get the url to open the Lingotek Workbench.
 *
 * This fetches a link.
 *
 * @param object $node
 *   A Drupal node.
 * @param $lingotek_locale
 *   A target language.
 * @param mixed $label
 *   The label to use as text for the link. Possible values are
 *   TRUE, FALSE, or a string to use as a the custom label for the link.
 * @param bool $force
 *   Force the link to use the label of the first returned workflow phase for the target Document.
 *
 * @return string
 *   Either a link pointing the the url, or the url itself if $label is FALSE
 */
function lingotek_get_workbench_url($document_id, $lingotek_locale, $label = FALSE, $force = FALSE) {

  if ($lingotek_locale === FALSE) {
    return "";
  }

  $api = LingotekApi::instance();

  $targets = lingotek_get_document_targets($document_id, TRUE); //Make sure we get the current phases for the links and not out of date ones (so caches don't combine)

  if (count($targets) == 0) {
    return '';
  }

  foreach ($targets as $lang => $translation_target) {
    if ($lang != $lingotek_locale) {
      continue;
    }

    $target = $api->getTranslationTarget($translation_target->id);
    $phases = ($target) ? $target->phases : array();

    return lingotek_get_workbench_url_by_phases($document_id, $phases, $label, $force);
  }

  LingotekLog::error('lingotek_get_workbench_url - Specified language target not found', $document_id);

  return '';
}

/**
 * Get the url to open the Lingotek Workbench.
 *
 * This fetches a link.
 *
 * @param object $node
 *   A Drupal node.
 * @param $lingotek_locale
 *   A target language.
 * @param mixed $label
 *   The label to use as text for the link. Possible values are
 *   TRUE, FALSE, or a string to use as a the custom label for the link.
 * @param bool $force
 *   Force the link to use the label of the first returned workflow phase for the target Document.
 *
 * @return string
 *   Either a link pointing the the url, or the url itself if $label is FALSE
 */
function lingotek_get_workbench_url_by_phases($document_id, $phases, $label = FALSE, $force = FALSE) {

  $phase_id = -1;
  $which_phase = 0;
  foreach ($phases as $phase) {
    if (!$phase->isMarkedComplete || $force) {
      $phase_id = $phase->id;
      break;
    }
    $which_phase++;
  }

  // All phases are complete, use last phase as current.
  if (!empty($phases) && $phase_id == -1) {
    $last_phase = end($phases);
    $phase_id = $last_phase->id;
  }

  $l = '';
  if ($phase_id != -1) {
    if ($document_id && $workbench_url = LingotekApi::instance()->getWorkbenchLink($document_id, $phase_id)) {
      if ($label === FALSE) {
        $l = $workbench_url;
      }
      else {
        $path = $workbench_url;
        if ($label === TRUE) {
          $label = lingotek_get_phase_name($phase_id);
        }
        list($nid, $entity_type) = LingotekSync::getEntityIdFromDocId($document_id);
        $l = l($label, '', array('attributes' => array(
            'onclick' => 'window.open(\'' . $path . '\'); return false;',
            'onmouseover' => 'jQuery("#node-' . $nid . '").addClass("lingotek-highlight");',
            'onmouseout' => 'jQuery("#node-' . $nid . '").removeClass("lingotek-highlight");',
        )));
      }
    }
  }

  return $l;
}

/*
 * Get the xliff information of the node
 *
 * This fetches an xliff representation of the source document.
 *
 * @param $doc_id
 *  Document id that associates the node to the Lingotek platform
 * @return
 *  xml text of the xliff
 */

function lingotek_get_xliff($doc_id) {
  global $_lingotek_client;

  $xliff_text = "";
  $params = array('documentId' => $doc_id);
  return $_lingotek_client->downloadTriggered("downloadDocumentAsXliff", $params);
}
