<?php

/**
 * @file
 * Lingotek Module + Rules Module Integration
 */

/**
 * Implements hook_rules_action_info().
 */
function lingotek_rules_action_info() {
  $actions['lingotek_entity_upload'] = array(
    'group' => t('Lingotek'),
    'label' => t('Upload an entity'),
    'base' => 'lingotek_rules_entity_upload',
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
      ),
    ),
  );
  $actions['lingotek_entity_download'] = array(
    'group' => t('Lingotek'),
    'label' => t('Download specified translations'),
    'base' => 'lingotek_rules_entity_download',
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
      ),
      'language' => array(
        'type' => 'list<text>',
        'label' => t('Language'),
        'options list' => 'lingotek_rules_languages',
      ),
    ),
  );
  $actions['lingotek_entity_download_all_translations'] = array(
    'group' => t('Lingotek'),
    'label' => t('Download all translations'),
    'base' => 'lingotek_rules_entity_download_all',
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
      ),
    ),
  );
  $actions['lingotek_entity_change_profile'] = array(
    'group' => t('Lingotek'),
    'label' => t('Change Profile'),
    'base' => 'lingotek_rules_entity_change_profile',
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
      ),
      'language' => array(
        'type' => 'text',
        'label' => t('Profile'),
        'options list' => 'lingotek_get_profile_options',
      ),
    ),
  );
  return $actions;
}

function lingotek_rules_languages() {
  return Lingotek::getLanguages('language');
}

function lingotek_rules_entity_upload($wrapper, $settings, $state) {
  $entity_type = $wrapper->type();
  $id = $wrapper->getIdentifier();
  $entity = lingotek_entity_load_single($entity_type, $id);
  lingotek_entity_changed($entity_type, $entity, $id);
  lingotek_entity_upload($entity, $entity_type);
}

function lingotek_rules_entity_download($wrapper, $languages, $settings, $state) {
  $entity_type = $wrapper->type();
  $id = $wrapper->getIdentifier();
  $entity = $wrapper->value();
  foreach ($languages as $lingotek_locale) {
    lingotek_entity_download($entity, $entity_type, $lingotek_locale);
  }
}

function lingotek_rules_entity_download_all($wrapper, $settings, $state) {
  $entity_type = $wrapper->type();
  $id = $wrapper->getIdentifier();
  $entity = $wrapper->value();
  $source_lingotek_locale = Lingotek::convertDrupal2Lingotek($entity->language);
  $target_languages = Lingotek::getLanguagesWithoutSource($source_lingotek_locale);
  foreach ($target_languages as $lingotek_locale) {
    lingotek_entity_download($entity, $entity_type, $lingotek_locale);
  }
}

function lingotek_rules_entity_change_profile($wrapper, $profile, $settings, $state) {
  $entity_type = $wrapper->type();
  $id = $wrapper->getIdentifier();
  $entity = $wrapper->value();
  $entity->lingotek['profile'] = $profile;
  lingotek_entity_save($entity, $entity_type);
}


/**
 * Implements hook_rules_condition_info().
 */
function lingotek_rules_condition_info() {
  $items = array();
  $items['lingotek_rules_entity_all_downloaded'] = array(
    'group' => t('Lingotek'),
    'module' => 'lingotek',
    'label' => t('Entity has all translations downloaded'),
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'desciption' => t('Specifies the entity for which the translation was downloaded.'),
      ),
    ),
  );
  $items['lingotek_rules_entity_all_ready_for_download'] = array(
    'group' => t('Lingotek'),
    'module' => 'lingotek',
    'label' => t('Entity has all translations ready for download'),
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'desciption' => t('Specifies the entity for which the translation is ready for download.'),
      ),
    ),
  );
  $items['lingotek_rules_entity_all_translations_of_status'] = array(
    'group' => t('Lingotek'),
    'module' => 'lingotek',
    'label' => t('Entity has all translations of status'),
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'desciption' => t('Specifies the entity for which the translation is ready for download.'),
      ),
      'status' => array(
        'type' => 'token',
        'label' => t('Status'),
        'description' => t('The Lingotek translation status'),
        'options list' => 'LingotekSync::getTargetStatusOptions',
        'restriction' => 'input',
      ),
    ),
  );
  $items['lingotek_rules_entity_has_profile'] = array(
    'group' => t('Lingotek'),
    'module' => 'lingotek',
    'label' => t('Entity has profile'),
    'parameter' => array(
      'entity' => array(
        'type' => 'entity',
        'label' => t('Entity'),
        'desciption' => t('Specifies the entity for which the translation was downloaded.'),
      ),
      'profile' => array(
        'type' => 'token',
        'label' => t('Profile'),
        'description' => t('The Lingotek profile assigned to this entity'),
        'options list' => 'lingotek_get_profiles_by_name',
        'restriction' => 'input',
      ),
    ),
  );
  return $items;
}

function lingotek_rules_entity_all_downloaded($wrapper) {
  return lingotek_rules_entity_all_translations_of_status($wrapper, LingotekSync::STATUS_CURRENT);
}

function lingotek_rules_entity_all_ready_for_download($wrapper) {
  return lingotek_rules_entity_all_translations_of_status($wrapper, LingotekSync::STATUS_READY);
}

function lingotek_rules_entity_all_translations_of_status($wrapper, $status) {
  $entity = $wrapper->value();
  $entity_id = $wrapper->getIdentifier();
  $entity_type = $wrapper->type();
  $source_lingotek_locale = Lingotek::convertDrupal2Lingotek($entity->language);
  $target_languages = Lingotek::getLanguagesWithoutSource($source_lingotek_locale);
  $target_status_keys = array();
  foreach ($target_languages as $target_lingotek_locale) {
    $target_status_keys[] = 'target_sync_status_' . $target_lingotek_locale;
  }
  if (empty($target_status_keys)) {
    return FALSE;
  }
  $query = db_select('{lingotek_entity_metadata}', 'lem')
      ->fields('lem', array('entity_id'))
      ->condition('lem.entity_id', $entity_id)
      ->condition('lem.entity_type', $entity_type)
      ->condition('lem.entity_key', $target_status_keys, 'IN')
      ->condition('lem.value', $status);
  $result = $query->execute();
  return ($result->rowCount() === count($target_languages));
}

function lingotek_rules_entity_has_profile($wrapper, $profile) {
  $entity = $wrapper->value(); // $wrapper is a DrupalEntityWrapper object
  $entity_profile_id = isset($entity->lingotek['profile']) ? $entity->lingotek['profile'] : "NONE";
  return $entity_profile_id === $profile;
}

/**
 * Implements hook_rules_event_info().
 */
function lingotek_rules_event_info() {
  $defaults = array(
    'group' => t('Lingotek'),
    'module' => 'lingotek',
  );
  return array(
    'lingotek_entity_pre_upload' => $defaults + array(
  'label' => t('Before an entity is uploaded'),
  'variables' => array(
    'entity' => array('type' => 'entity', 'label' => t('entity uploaded')),
  ),
    ),
    'lingotek_entity_post_upload' => $defaults + array(
  'label' => t('After an entity was uploaded'),
  'variables' => array(
    'entity' => array('type' => 'entity', 'label' => t('entity uploaded')),
  ),
    ),
    'lingotek_entity_translation_pre_download' => $defaults + array(
  'label' => t('Before a translation is downloaded (acting on source entity)'),
  'variables' => array(
    'entity' => array('type' => 'entity', 'label' => t('entity translation downloaded')),
  ),
    ),
    'lingotek_entity_translation_post_download' => $defaults + array(
  'label' => t('After a translation was downloaded (acting on translated entity)'),
  'variables' => array(
    'entity' => array('type' => 'entity', 'label' => t('entity translation downloaded')),
  ),
    ),
    'lingotek_entity_translation_ready' => $defaults + array(
  'label' => t('A translation is ready for download'),
  'variables' => array(
    'entity' => array('type' => 'entity', 'label' => t('entity translation ready for download'))
  ),
    ),
  );
}
