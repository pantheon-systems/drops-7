<?php

/**
 * @file
 * Call the Lingotek APIs
 */
$GLOBALS['_lingotek_client'] = new LingotekSession();

/*
 * Download the translations from the Lingotek platform
 *
 * This updates a node's content with translations for target languages
 *
 * @param $node
 *  Node being updated/synchronized
 * @param $lingotek_locale
 *  Target language to be updated/synchronized
 */

function lingotek_download_document(&$node, $lingotek_locale, $sync_success_status = LingotekSync::STATUS_CURRENT) {
  global $_lingotek_client, $_lingotek_locale;

  $document_id = lingotek_lingonode($node->nid, 'document_id');
  LingotekLog::trace('lingotek_download_document @doc_id (@target)', array('@doc_id' => $document_id, '@target' => $lingotek_locale));

  //CAREFUL of alternate values for $use_source, must be string 'TRUE' for api, not boolean
  $use_source = lingotek_lingonode($node->nid, 'use_source');
  if ($use_source === FALSE) {
    $use_source = variable_get('lingotek_use_source', TRUE);
  }
  if ($use_source == '1') {
    $use_source = 'TRUE';
  }

  $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale); //lingotek_lookup_language_by_locale($lingotek_locale)->language
  $params = array
    (
    'documentId' => $document_id,
    'targetLanguage' => $lingotek_locale,
    'useSource' => $use_source,
  );

  //Get the finished document
  $text = $_lingotek_client->download("downloadDocument", $params);
  try {
    $xml = new SimpleXMLElement($text);
  } catch (Exception $e) {
    LingotekLog::error("downloadDocument FAILED. Error: @error. Text: !xml.", array('!xml' => $text, '@error' => $e->getMessage()));
    return;
  }

  $url_alias_translation = lingotek_variable_get(lingotek_lingonode($node->nid, 'url_alias_translation'), 'lingotek_url_alias_translation', 1);

  foreach ($xml as $tag => $content) {
    if ($tag == 'menu_title') {
      //TODO handle menu translation
    }
    elseif ($tag == 'url_alias' && $url_alias_translation == 1) {
      $target = check_plain($content);

      //URL Alias related to the page:
      $conditions = array('source' => 'node/' . $node->nid);
      if ($node->language != LANGUAGE_NONE) {
        $conditions['language'] = $node->language;
      }
      $path = path_load($conditions);
      if ($path !== FALSE) {
        $conditions['language'] = $drupal_language_code;
        if ($path['alias'] != $target) {
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
    else {
      $drupal_field_name = $tag;
      $target_key = 'value';
      $subfield_parts = explode('__', $tag);
      if (count($subfield_parts) == 2) {
        $drupal_field_name = $subfield_parts[0];
        $target_key = $subfield_parts[1];
      }
      $field = field_info_field($drupal_field_name);
      if (isset($field) && array_key_exists('lingotek_translatable', $field) && $field['lingotek_translatable'] == 1) {
        $node_field = & $node->$drupal_field_name;
        $index = 0;
        foreach ($content as $text) {
          $node_field[$drupal_language_code][$index][$target_key] = decode_entities(lingotek_xml_decode($text));
          if (array_key_exists($node->language, $node_field)) {
            if (array_key_exists('format', $node_field[$node->language][0])) {
              $node_field[$drupal_language_code][$index]['format'] = $node_field[$node->language][0]['format'];
            }
          }
          $index++;
        }
      }

      //Set URL alias
      $url_alias_translation = lingotek_variable_get(lingotek_lingonode($node->nid, 'url_alias_translation'), 'lingotek_url_alias_translation', 1);
      if ($tag == 'title_field' && $url_alias_translation == 2 && module_exists('pathauto') && $node->language != LANGUAGE_NONE) {
        module_load_include('inc', 'pathauto');
        $uri = entity_uri('node', $node);
        pathauto_create_alias('node', 'update', $uri['path'], array('node' => clone $node), $node->type, $drupal_language_code);
      }
    }
  }

  //Fix for pathauto expecting the form:
  $node->path = path_load(array('source' => 'node/' . $node->nid, 'language' => $node->language));
  $node->path['alias'] = isset($node->path['alias']) ? $node->path['alias'] : '';
  $node->path['pathauto'] = 0;

  LingotekSync::setTargetStatus($node->nid, $lingotek_locale, $sync_success_status); //slightly pre-emptive, but certainly more cohesive
  $node->lingotek_upload_override = 0; // ensure that no upload to lingotek is triggered on node update (in lingotek_node_update)
  $node->skip_status_updates = 1; // ensure that the statuses are not set on node update (in lingotek_node_update)
  //Fix for workbench_moderation changing node state after translation download
  if (module_exists('workbench_moderation')) {
    if (!isset($node->workbench_moderation)) {
      LingotekLog::error("workbench_moderation variable does not exist inside node object. Error: @error.", array('@error' => $e->getMessage()));
    }
    else {
      $node->workbench_moderation['updating_live_revision'] = 1; // ensure that workflow state does not get updated
    }
  }

  node_save($node);
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
    if ($output) {
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
function lingotek_get_workbench_url($node, $lingotek_locale, $label = FALSE, $force = FALSE) {

  if ($lingotek_locale === FALSE) {
    return "";
  }

  $api = LingotekApi::instance();

  $link = array();

  $document_id = lingotek_lingonode($node->nid, 'document_id');
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
      $document_id = lingotek_lingonode($node->nid, 'document_id');
      if ($document_id && $workbench_url = $api->getWorkbenchLink($document_id, $phase_id)) {
        if ($label === FALSE) {
          $l = $workbench_url;
        }
        else {
          $path = $workbench_url;
          if ($label === TRUE) {
            $label = lingotek_get_phase_name($phase_id);
          }
          $l = l($label, '', array('attributes' => array(
              'onclick' => 'window.open(\'' . $path . '\'); return false;',
              'onmouseover' => 'jQuery("#node-' . $node->nid . '").addClass("highlight");',
              'onmouseout' => 'jQuery("#node-' . $node->nid . '").removeClass("highlight");',
              )));
        }
      }
    }

    $link[$lang] = array(
      'which_phase' => $which_phase,
      'link' => $l,
    );
  }

  return $link[$lingotek_locale]['link'];
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
  return $_lingotek_client->download("downloadDocumentAsXliff", $params);
}
