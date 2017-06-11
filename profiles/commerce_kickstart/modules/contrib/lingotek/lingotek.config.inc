<?php

/**
 * @file
 * Config grid form elements
 */

// All the stuff for the new config menu goes in here
function lingotek_config_filter_inline_submit($form, $form_state) {
  $entity_type = $_SESSION['grid_entity_type'];
  unset($_SESSION['grid_filters'][$entity_type]['textgroup']);
  $_SESSION['grid_filters'][$entity_type]['textgroup'] = $form_state['values']['search_type'];
  if (!empty($form_state['values']['search'])) {
    $_SESSION['grid_filters'][$entity_type]['search'] = $form_state['values']['search'];
  }
  else {
    unset($_SESSION['grid_filters'][$entity_type]['search']);
  }

  if (isset($form_state['values']['limit_select'])) {
    $_SESSION['limit_select'] = $form_state['values']['limit_select'];
  }
}

function lingotek_config_header() {
  $header = array(// Define the tentative source header
    'marked' => array('data' => 'Marked'),
    'lid' => array('data' => t('ID'), 'field' => 'lid', 'width' => '2%'),
    'config_set_id' => array('data' => t('Config Set ID'), 'field' => 'config_set_id', 'width' => '9%'),
    'set_name' => array('data' => t('Config Set Name'), 'field' => 'set_id', 'width' => '15%'),
    'source' => array('data' => t('Source'), 'field' => 'source', 'width' => '25%'),
    'source_uploaded' => array('data' => t('Source Uploaded')),
    'translations' => array('data' => t('Translations'), 'field' => 'current_target_count'),
    'location' => array('data' => t('Location'), 'field' => 'location'),
    'context' => array('data' => t('Context'), 'field' => 'context'),
    'doc_id' => array('data' => t('Doc ID'), 'field' => 'document_id', 'display' => 'none'),
    'textgroup' => array('data' => t('Textgroup'), 'field' => 'textgroup'),
    'workflow' => array('data' => t('Workflow'), 'field' => 'workflow_id'),
  );

  return $header;
}
function lingotek_row_status_query() {
  $query1 = lingotek_config_start_query(null, 'not taxonomy');
  lingotek_config_add_query_filters($query1);
  $query2 = lingotek_config_start_query(null, 'taxonomy name');
  lingotek_config_add_query_filters($query2);
  $query3 = lingotek_config_start_query(null, 'taxonomy desc');
  lingotek_config_add_query_filters($query3);
  $query1->union($query2, 'UNION');
  $query1->union($query3, 'UNION');
  $query = db_select($query1, 'query_union')->fields('query_union');
  $limit = isset($_SESSION['limit_select']) ? $_SESSION['limit_select'] : 10;
  $table_data_raw = $query
      ->extend('PagerDefault')
      ->extend('TableSort')
      ->limit($limit)
      ->execute()
      ->fetchAllAssoc('lid');

  return lingotek_bulk_grid_parse_config_data($table_data_raw, null, TRUE);
}
function lingotek_config_get_rows($entity_type, $form, &$form_state, $count_only = FALSE) {
  $columns = isset($form_state['values']['columns']) ? $form_state['values']['columns'] : array();
  $header = lingotek_config_header();
  $form_state['values']['grid_header'] = lingotek_bulk_grid_refine_source_header($header, $columns);
  $query1 = lingotek_config_start_query($form_state, 'not taxonomy');
  lingotek_config_add_query_filters($query1);
  $query2 = lingotek_config_start_query($form_state, 'taxonomy name');
  lingotek_config_add_query_filters($query2);
  $query3 = lingotek_config_start_query($form_state, 'taxonomy desc');
  lingotek_config_add_query_filters($query3);
  $query1->union($query2, 'UNION');
  $query1->union($query3, 'UNION');

  if ($count_only) {
    $result = $query1->execute();
    return $result->rowCount();
  }

  $query = db_select($query1, 'query_union')->fields('query_union');
  $limit = isset($_SESSION['limit_select']) ? $_SESSION['limit_select'] : 10;
  $table_data_raw = $query
      ->extend('PagerDefault')
      ->extend('TableSort')
      ->limit($limit)
      ->orderByHeader($form_state['values']['grid_header'])
      ->execute()
      ->fetchAllAssoc('lid');
  $table_data = lingotek_bulk_grid_parse_config_data($table_data_raw, $entity_type);
  if(isset($_SESSION['grid_filters'][$entity_type])){
    $all_filtered_results = $query
      ->extend('PagerDefault')
      ->limit(500000)
      ->execute();
    $all_filtered_lids = array();
    foreach($all_filtered_results as $filtered_result){
      $all_filtered_lids[$filtered_result->lid] = $filtered_result->lid;
    }
    $_SESSION['grid_filters'][$entity_type]['filtered_config_lids'] = $all_filtered_lids;
  }
  return $table_data;
}
function lingotek_config_get_status($lids){
  $metadata_query = db_select('lingotek_config_map', 'lcmap');
  $metadata_query->join('lingotek_config_metadata', 'lcmeta', 'lcmeta.id = lcmap.set_id');
  $metadata_query->condition('lcmeta.config_key','%target_sync_status%', 'LIKE');
  $metadata_query->condition('lcmap.lid', $lids, 'IN');
  $metadata_query->addField('lcmap', 'lid');
  $metadata_query->addField('lcmeta', 'config_key');
  $metadata_query->addField('lcmeta', 'value');
  $metadata_results = $metadata_query->execute();

  $locales_target_query = db_select('locales_target', 'lt');
  $locales_target_query->condition('lt.i18n_status','1');
  $locales_target_query->condition('lt.lid', $lids, 'IN');
  $locales_target_query->addField('lt', 'lid');
  $locales_target_query->addField('lt', 'i18n_status');
  $locales_target_query->addField('lt', 'language');
  $lt_results = $locales_target_query->execute();
  $lt_array = array();
  foreach($lt_results as $lt_result){
    $lt_array[$lt_result->lid][$lt_result->language] = $lt_result;
  }

  return array('metadata' => $metadata_results, 'dirty_status' => $lt_array);
}
function lingotek_config_add_existing_translations($lids, $lang_codes) {
  $query = db_select('locales_target', 'lt')
      ->fields('lt', array('lid', 'language'));
  $query->condition('lid', $lids, 'IN');
  $result = $query->execute()->fetchAll();

  $existing_translations = array();
  foreach ($result as $key => $value) {
    $lang = Lingotek::convertDrupal2Lingotek($value->language);
    // This check is performed in lang_icons anyway so you might remove it
    if ($lang && array_key_exists($lang, $lang_codes)) {
      $existing_translations[$value->lid][] = Lingotek::convertDrupal2Lingotek($value->language);
    }
  }
  return $existing_translations;
}

function lingotek_config_start_query($form_state, $union_part_desc) {
  $query = db_select('locales_source', 's');

  // Gets sync statuses for each language.
  $query->addExpression("(SELECT COUNT(*) FROM {locales_target} WHERE i18n_status = 0 AND lid = (SELECT lid FROM {lingotek_config_map} WHERE lid = s.lid AND current = 1))", 'current_target_count');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='PENDING')", 'pending');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='READY')", 'ready');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='CURRENT')", 'current');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='UNTRACKED')", 'ready_interim');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='UNTRACKED')", 'interim');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='EDITED')", 'edited');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='UNTRACKED')", 'untracked');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='ERROR')", 'error');
  $query->addExpression("(SELECT GROUP_CONCAT(SUBSTRING(config_key, 20, 10)) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key LIKE 'target_sync_status_%' AND value='NONE')", 'none');
  $query->addExpression("(SELECT GROUP_CONCAT(language) FROM {locales_target} WHERE lid = s.lid AND i18n_status = 1)", 'source_edited');
  $query->addExpression("(SELECT set_id FROM {lingotek_config_map} lcm WHERE lcm.lid = s.lid)", 'set_id');
  $query->addExpression("(SELECT value FROM {lingotek_config_metadata} lcm WHERE lcm.id = (SELECT set_id FROM lingotek_config_map WHERE lid = s.lid) AND config_key = 'upload_error')", 'upload_error');
  $query->addExpression("(SELECT value FROM {lingotek_config_metadata} lcm WHERE lcm.value = s.lid AND config_key = 'marked')", 'marked');
  $query->addExpression("(SELECT set_id FROM {lingotek_config_map} lcm WHERE lcm.lid = s.lid)", 'config_set_id');

  // Lingotek Document ID
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key = 'document_id')", 'document_id');
  //Config Workflow ID
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key = 'workflow_id')", 'workflow_id');
  // Entity Upload Status
  $query->addExpression("(SELECT GROUP_CONCAT(value) FROM {lingotek_config_metadata} WHERE id = (SELECT set_id FROM {lingotek_config_map} WHERE lid = s.lid) AND config_key = 'upload_status' AND value <> '" . LingotekSync::STATUS_TARGET . "' AND value <> '" . LingotekSync::STATUS_DELETED . "')", 'upload_status');
  $query->fields('s', array('source', 'location', 'context', 'lid', 'textgroup'));
  // Only show items in textgroups that are selected for translation on Config Settings page.
  $translatable_textgroups = LingotekConfigSet::getTextgroupsForTranslation() ? : array('no textgroups selected');

  if (in_array('misc', $translatable_textgroups)) {
    $all_textgroup_categories = array('blocks', 'default', 'menu', 'taxonomy', 'views', 'field');
    $qor = db_or();
    $qor->condition('s.textgroup', $translatable_textgroups, 'IN');
    $qor->condition('s.textgroup', $all_textgroup_categories, 'NOT IN');
    $query->condition($qor);
  }
  else {
    $query->condition('s.textgroup', $translatable_textgroups, 'IN');
  }

  // Hide taxonomy terms that use translate for i18n_mode.
  $translate_terms_subquery = db_select('taxonomy_vocabulary', 'tv');
  $translate_terms_subquery->fields('tv', array('vid'));
  $translate_terms_subquery->condition('tv.i18n_mode', LINGOTEK_TAXONOMY_TRANSLATE_VALUE);
  $translate_ids = $translate_terms_subquery->execute()->fetchCol();

  // Hide taxonomy terms that have custom fields.
  $advanced_vids = array_keys(lingotek_get_advanced_vocabularies());

  if ($union_part_desc == 'taxonomy name') {
    $advanced_vid_expression = empty($advanced_vids) ? '' : ' AND term_name.vid NOT IN (' . implode(',', $advanced_vids) . ')';
    $query->leftJoin('taxonomy_term_data', 'term_name', 'SUBSTRING_INDEX(SUBSTRING_INDEX(s.context , \':\', 2 ),\':\',-1) = term_name.tid AND term_name.name = s.source AND term_name.vid NOT IN (' . implode(',', array_merge(array(-1), $translate_ids)) . ')' . $advanced_vid_expression);
    $query->addExpression('term_name.tid', 'tid_from_name');//DO NOT do addField here, it puts the column name in the wrong place and messes with the union
  }
  else {
    $query->addExpression('NULL', 'tid_from_name');
  }

  if ($union_part_desc == 'taxonomy desc') {
    $advanced_vid_expression = empty($advanced_vids) ? '' : ' AND term_description.vid NOT IN (' . implode(',', $advanced_vids) . ')';
    $query->leftJoin('taxonomy_term_data', 'term_description', 'SUBSTRING_INDEX(SUBSTRING_INDEX(s.context , \':\', 2 ),\':\',-1) = term_description.tid AND term_description.description = s.source AND term_description.vid NOT IN (' . implode(',', array_merge(array(-1), $translate_ids)) . ')' . $advanced_vid_expression);
    $query->addExpression('term_description.tid', 'tid_from_description');//DO NOT do addField here, it puts the column name in the wrong place and messes with the union
  }
  else {
    $query->addExpression('NULL', 'tid_from_description');
  }

  // For all taxonomy terms, only show if it has a name or description.
  if ($union_part_desc == 'not taxonomy') {
    $query->condition('s.textgroup', 'taxonomy', '<>');
  }
  elseif ($union_part_desc == 'taxonomy name') {
    $query->isNotNull('term_name.tid');
  }
  elseif ($union_part_desc = 'taxonomy desc') {
    $query->isNotNull('term_description.tid');
  }

  return $query;
}

function lingotek_config_add_query_filters($query) {
  $entity_type = $_SESSION['grid_entity_type'];
  if (!isset($_SESSION['grid_filters'][$entity_type])) {
    return;
  }
  $filters = $_SESSION['grid_filters'][$entity_type];
  if (isset($filters['search']) && strlen($filters['search'])) {
    $query->where("LOWER(CONVERT(source USING utf8)) LIKE :source_string", array(':source_string' => '%' . strtolower($filters['search'] . '%')));
  }

  if (isset($filters['textgroup'])) {
    if ($filters['textgroup'] == 'interface') {
      $query->condition('textgroup', 'default');
    }
    elseif ($filters['textgroup'] == 'menu') {
      $query->condition('textgroup', 'menu');
    }
    elseif ($filters['textgroup'] == 'taxonomy') {
      $query->condition('textgroup', 'taxonomy');
    }
    elseif ($filters['textgroup'] == 'blocks') {
      $query->condition('textgroup', 'blocks');
    }
    elseif ($filters['textgroup'] == 'field') {
      $query->condition('textgroup', 'field');
    }
    elseif ($filters['textgroup'] == 'views') {
      $query->condition('textgroup', 'views');
    }
    elseif ($filters['textgroup'] == 'webform') {
      $query->condition('textgroup', 'webform');
    }
    elseif ($filters['textgroup'] == 'misc') {
      $query->condition('textgroup', array('default', 'menu', 'taxonomy', 'blocks', 'field', 'views'), 'NOT IN');
    }
  }

  // Module Filter
  if (isset($filters['location']) && $filters['location'] != '') {
    $query->condition('s.location', '%' . $filters['location'] . '%', 'LIKE');
  }

  lingotek_filter_by_document_id($query, $filters);

  // Upload Status
  if (isset($filters['upload_status']) && $filters['upload_status'] != 'all') {
    $query->leftJoin('locales_target', 'lt', 'lt.lid = s.lid');
    if ($filters['upload_status'] == 'never') {
      $query->isNull('lt.lid');
    }
    elseif ($filters['upload_status'] == LingotekSync::STATUS_EDITED) {
      // The edited status won't show in lingotek_config_metadata table so we need to check the i18n_status (1 == edited).
      $query->condition('lt.i18n_status', '1');
    }
    else {
      $query->havingCondition('upload_status', $filters['upload_status']);
      // Do not include items with a source marked as edited in locales_target (1 == edited).
      $query->condition('lt.i18n_status', '0');
    }
  }

  // Marked status
  if (isset($filters['marked_status']) && $filters['marked_status'] != 'all') {
    // Marked
    if($filters['marked_status'] == 'marked') {
      $marked_query = db_select('lingotek_config_metadata','lcm');
      $marked_query->distinct();
      $marked_query->fields('lcm',array('value'));
      $marked_query->condition('config_key', 'marked');

      $query->condition('s.lid', $marked_query, 'IN');
    }
    // Unmarked
    else if($filters['marked_status'] == 'unmarked'){
      $marked_query = db_select('lingotek_config_metadata','lcm');
      $marked_query->distinct();
      $marked_query->fields('lcm',array('value'));
      $marked_query->condition('config_key', 'marked');

      $query->condition('s.lid', $marked_query, 'NOT IN');
    }
  }

  // Entity ID
  if (isset($filters['lid']) && $filters['lid'] != '') {
    $query->condition('s.lid', $filters['lid']);
  }

  // Config set ID
  if (isset($filters['config_set_id']) && $filters['config_set_id'] != '') {
    $config_set_query = db_select('lingotek_config_map', 'lcm')
      ->fields('lcm', array('lid'))
      ->condition('set_id', $filters['config_set_id']);

    $query->condition('s.lid', $config_set_query, 'IN');
  }
}

/**
 * Gets record of all Non-Lingotek translations (translation_agent_id != 3)
 */
function lingotek_get_locked_config_translations($lids = NULL) {
  $query = db_select('locales_target', 'lt')
    ->fields('lt', array('lid'));
  $query->addExpression('GROUP_CONCAT(lt.language)', 'language_codes');
  $query->condition('lt.translation_agent_id', LingotekConfigSet::getLingotekTranslationAgentId(), '!=');
  if (is_array($lids) && !empty($lids)) {
    $query->condition('lt.lid', $lids, 'IN');
  }
  $query->groupBy('lid');

  return $query->execute()->fetchAllAssoc('lid');
}

function lingotek_bulk_grid_parse_config_data($table_data_raw, $entity_type, $status_query = FALSE) {
  $lingotek_languages = Lingotek::getLanguages();

  $lids = array_keys($table_data_raw);
  $existing_translations = !empty($lids) ? lingotek_config_add_existing_translations($lids, $lingotek_languages) : array();

  $global_profile = lingotek_get_global_profile();
  $row_status = array();
  $table_data = array();
  if($status_query === FALSE) {
    $non_lingotek_config_translations = lingotek_get_locked_config_translations($lids);
    $api = LingotekApi::instance();
    $workflows = $api->listWorkflows();
    $languages = language_list();
  }

  $i18n_language = variable_get('i18n_string_source_language', language_default()->language);

  foreach ($table_data_raw as $row) {
    $lid = $row->{'lid'};
    $config_set_id = isset($row->{'config_set_id'}) ? $row->{'config_set_id'} : 'N/A';
    $row->lingotek = $global_profile;
    $row->lingotek['profile'] = 1;
    $row->language = $i18n_language;

    $locale_statuses = lingotek_build_locales_statuses($row, FALSE);
    $item_is_current = LingotekConfigSet::isLidCurrent($lid);
    $item_has_translations = array_key_exists($lid, $existing_translations);
    $source_was_edited = $row->source_edited && $item_is_current !== FALSE ? explode(',', $row->source_edited) : array();

    $item_belongs_to_set = !empty($locale_statuses);

    if ($source_was_edited) {
      $row->upload_status = LingotekSync::STATUS_EDITED;
      foreach ($source_was_edited as $key => $lang) {
        $locale = Lingotek::convertDrupal2Lingotek($lang);
        $locale_statuses[$locale] = 'edited';
      }
    }

    $not_uploaded_to_lingotek = !$item_has_translations && $item_is_current;
    ksort($locale_statuses);
    if ($item_belongs_to_set) {
      if ($not_uploaded_to_lingotek) {
        unset($locale_statuses);
        $locale_statuses = array();
        $row->upload_status = NULL;
      }
    }
    elseif ($item_has_translations) { // No translations in locales + translations in locales_target = Untracked.
      foreach ($existing_translations[$lid] as $existing_locale) {
        $locale_statuses[$existing_locale] = 'untracked';
      }
    }
    if($status_query === TRUE) {
      $row_status[$lid] = $locale_statuses;
      if (isset($row->status)) {
        $row_status[$lid]['source_status'] = $row->upload_status;
      }
      continue;
    }

    $escaped_source = htmlentities($row->{'source'});
    $source_text = lingotek_truncate_grid_text($escaped_source, 55);

    $translation_icons = lingotek_lang_icons($entity_type, $languages, $lid, NULL, $locale_statuses, FALSE, $row->language, FALSE, FALSE, $non_lingotek_config_translations); // show translation statuses
    $target_icons_str = implode('', array_values($translation_icons));
    $source_uploaded = lingotek_render_source($entity_type, $row, $languages, 'language');
    $set_id = LingotekConfigSet::getSetId($lid, FALSE);
    $marked_html = lingotek_render_marked_html($row, 'config', $lid);
    $set_name = $set_id !== FALSE ? LingotekConfigSet::getTitleBySetId($set_id) : t('N/A');
    $config_profile = LingotekProfile::loadById(LingotekSync::PROFILE_CONFIG);
    $stored_workflow_id = $config_profile->getWorkflow();
    if(isset($row->{'workflow_id'}) && array_key_exists($row->{'workflow_id'}, $workflows)) {
      $workflow = $workflows[$row->{'workflow_id'}];
    }
    else if(array_key_exists($stored_workflow_id, $workflows)) {
      $workflow = $workflows[$stored_workflow_id];
    }
    else {
      $workflow = '';
    }

    $data = array(
      'marked' => $marked_html,
      'config_set_id' => $config_set_id,
      'lid' => $lid ? : t('??'),
      'source' => '<a class="tooltips">' . $source_text . '<span>' . $escaped_source . '</span></a>',
      'source_uploaded' => $source_uploaded,
      'location' => $row->{'location'},
      'context' => $row->{'context'},
      'textgroup' => $row->{'textgroup'} == 'default' ? 'built-in interface' : $row->{'textgroup'},
      'translations' => $target_icons_str,
      'doc_id' => $row->document_id ? $row->document_id : t('N/A'),
      'set_name' => '<span class="set_name">' . $set_name . '</span>',
      'workflow' => $workflow
    );

    $table_data[$lid] = $data;
  }
  if($status_query === TRUE) {
    return $row_status;
  }
  return $table_data;
}

/**
 * Submit function for The Grid's actions
 * The action corresponds to the key of the option selected
 * Often redirects to batch operations or to other pages entirely
 */
function lingotek_config_action_submit($form, $form_state) {
  $lids = array();

  if (isset($form_state['clicked_button']) && $form_state['clicked_button']['#name'] == 'submit_actions') { // If submitting an action
    foreach ($form_state['values']['the_grid'] as $value) {
      if ($value != 0) {
        $lids[] = $value;
      }
    }

    if (isset($form_state['values']['select_actions'])) { // If an action was selected (which it would be, I don't know if this could ever NOT occur with normal use)
      $action = $form_state['values']['select_actions']; // Get the action
      if (count($lids) <= 0) { // Select a node
        drupal_set_message(t('You must select at least one node to @action.', array('@action' => $action)), 'warning'); // Or pay the price
      }
      elseif ($action == 'upload') { // If uploading
        lingotek_config_upload_selected($lids);
      }
      elseif (substr($action, 0, 8) == 'download') { // If downloading all targets
        lingotek_config_download_selected($action, $lids);
      }
      elseif ($action == 'sync') { // If syncing the progress
        lingotek_config_update_selected($lids); // Run batch operations to get the progress report from Lingotek
      }
      elseif ($action == 'delete' || $action == 'reset') {
        // ajax ctools modal employed (see lingotek_bulk_grid_form() and lingotek.bulk_grid.js)
      }
      elseif ($action == 'marked') {
        lingotek_config_mark_items($lids);
      }
      elseif ($action == 'unmarked') {
        lingotek_config_unmark_items($lids);
      }
    }
  }
}

function lingotek_config_mark_items($lids) {
  $batch = array(
    'title' => t('Updating Marked Config Items'),
    'finished' => 'lingotek_update_marked_items_finished'
  );

  $operations = lingotek_get_marked_batch_elements('config', $lids);
  $batch['operations'] = $operations;
  $redirect = current_path();

  batch_set($batch);
  batch_process($redirect);
}

function lingotek_config_unmark_items($lids) {
  $batch = array(
    'title' => t('Updating Unmarked Config Items'),
    'finished' => 'lingotek_update_unmarked_items_finished'
  );

  $operations = lingotek_get_unmarked_batch_elements('config', $lids);
  $batch['operations'] = $operations;
  $redirect = current_path();

  batch_set($batch);
  batch_process($redirect);
}

function lingotek_config_upload_selected($lid_map) {
  //allows for passing in an array of lids, not as efficient as passing a textgroup-lid map
  //in some cases
  if(is_array($lid_map) && isset($lid_map[0]) && !is_array($lid_map[0])){
    $lids = $lid_map;
    $query = db_select('locales_source', 'ls');
    $query->addField('ls', 'textgroup');
    $query->addField('ls', 'lid');
    $query->condition('lid', $lids, 'IN');
    $results = $query->execute();
    $lid_map = array();
    foreach($results as $result){
      $lid_map[$result->textgroup][$result->lid] = $result->lid;
    }
  }
  $unique_set_ids = LingotekConfigSet::bulkGetSetId($lid_map);
  $not_current_lids = !empty($unique_set_ids) ? LingotekConfigSet::getEditedLidsInSets($unique_set_ids) : array();

  if (empty($not_current_lids) && empty($lid_map)) {
    drupal_set_message(t('No config items have been edited and need to be uploaded at this time. To translate a config item for the first time, check the corresponding box and upload.'), 'status', FALSE);
    drupal_goto('admin/settings/lingotek/manage/config');
  }

  foreach($lid_map as $textgroup) {
    LingotekConfigSet::markLidsNotCurrent($textgroup);
  }
  if(!empty($not_current_lids)) {
    LingotekConfigSet::markLidsNotCurrent($not_current_lids);
  }

  // Update the translation_agent_id in the locales_target table to be owned by Lingotek
  LingotekSync::updateLingotekTranslationAgentId($lid_map, LingotekSync::TRANSLATION_AGENT_ID_LINGOTEK);

  $batch = array(
    'title' => t('Uploading Content To Lingotek'),
    'finished' => 'lingotek_sync_upload_config_finished'
  );
  $operations = lingotek_get_sync_upload_config_batch_elements($unique_set_ids);
  $batch['operations'] = $operations;
  $redirect = 'admin/settings/lingotek/manage/config';

  batch_set($batch);
  batch_process($redirect); // Run batch operations to upload all of the selected nodes to Lingotek
}

function lingotek_config_download_selected($action, $ids, $set_ids_passed = false) {
  $set_ids = $set_ids_passed ? $ids : array_unique(LingotekSync::getSetIdsFromLids($ids));
  if (empty($set_ids)) {
    drupal_set_message(t('No translations to download at this time. Make sure configuration items have been uploaded to Lingotek before trying to get translations for them.'), 'status', FALSE);
    drupal_goto('admin/settings/lingotek/manage/config');
  }
  $document_ids = LingotekSync::getConfigDocIdsFromSetIds($set_ids);
  $available_targets = Lingotek::getLanguages();
  unset($available_targets['en_US']); // all locales_source is assumed by i18n modules to be English

  list($verb, $target_locale) = explode('_', $action, 2);
  if ($verb != 'download') {
    throw LingotekException("Config download verb mismatch: expected 'download', received '$verb'");
  }
  if ($target_locale != 'all' && empty($available_targets[$target_locale])) {
    throw LingotekException("Config download received unknown target: $target_locale");
  }
  $download_targets = array();
  foreach ($document_ids as $doc_id) {
    if ($target_locale == 'all') {
      foreach (array_keys($available_targets) as $locale) {
        $target = new stdClass();
        $target->document_id = $doc_id;
        $target->locale = $locale;
        $download_targets[] = $target;
      }
    }
    else {
      $target = new stdClass();
      $target->document_id = $doc_id;
      $target->locale = $target_locale;
      $download_targets[] = $target;
    }
  }
  $redirect = 'admin/settings/lingotek/manage/config';
  $operations = lingotek_get_sync_download_batch_elements($download_targets, LingotekSync::STATUS_PENDING);
  $batch = array(
    'title' => t('Syncing Content and Translations'),
    'operations' => $operations,
    'file' => 'lingotek.batch.inc',
    'finished' => 'lingotek_sync_batch_finished'
  );
  batch_set($batch);
  batch_process($redirect);
}
