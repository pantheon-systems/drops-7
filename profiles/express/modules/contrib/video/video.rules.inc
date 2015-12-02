<?php
/**
 * @file
 * rules integration for the video module
 *
 * @addtogroup rules
 * @{
 */

/**
 * Implements hook_rules_file_info() on behalf of the video module.
 */
function video_rules_file_info() {
  return array('video.rules.inc');
}

/**
 * Implements hook_rules_event_info() on behalf of the video module.
 */
function video_rules_event_info() {
  $defaults = array(
    'group' => t('Video'),
    'module' => 'video',
    'access callback' => 'rules_video_integration_access',
  );
  return array(
    'video_success' => $defaults + array(
      'label' => t('After video conversion success'),
      'variables' => array(
        'node' => array('type' => 'node', 'label' => t('video success')),
      ),
    ),
    'video_failed' => $defaults + array(
      'label' => t('After video conversion failed.'),
      'variables' => array(
        'node' => array('type' => 'node', 'label' => t('video failed')),
      ),
    ),
  );
  return $items;
}

/**
 * Node integration access callback.
 */
function rules_video_integration_access($type, $name) {
  if ($type == 'event' || $type == 'condition') {
    return entity_access('view', 'node');
  }
}
