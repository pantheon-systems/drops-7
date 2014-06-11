<?php

/**
 * @file
 * Page handlers and supporting routines for Ajax callbacks.
 */

/**
 * Page handler for completing the current phases of remote Lingotek documents.
 *
 * @param object $node
 *   The node whose targets should have phases marked as complete.
 *
 * @return string
 *   JSON-encoded return data.
 */
function lingotek_page_mark_phases_complete($node) {
  $output = array(
    'success' => TRUE,
  );

  if (!empty($_POST['token']) && drupal_valid_token($_POST['token'])) {
    if (!empty($_POST['targets']) && is_array($_POST['targets'])) {
      $languages = language_list();
      if ($lingotek_document_id = lingotek_keystore('node', $node->nid, 'document_id')) {
        $targets = LingotekDocument::load($lingotek_document_id)->translationTargets();
        $api = LingotekApi::instance();
        foreach ($_POST['targets'] as $target) {
          if ($remote_target = $api->getTranslationTarget($targets[$target]->id)) {
            $current_phase_id = lingotek_current_phase($remote_target->phases);
            if ($api->markPhaseComplete($current_phase_id)) {
              drupal_set_message(t('Marked phase complete for @language', array('@language' => $languages[$target]->name)));
            }
            else {
              drupal_set_message(t('Unable to mark the current phase as complete for @language.',
                array('@language' => $languages[$target]->name)), 'error');
            }
          }
          else {
            drupal_set_message(t('Unable to mark the current phase as complete for @language.',
              array('@language' => $languages[$target]->name)), 'error');
          }
        }
      }
      else {
        drupal_set_message(t('Unable to location Lingotek Document. No phases were marked as complete.'), 'error');
        LingotekLog::error('Unable to locate Lingotek Document for node @node_id.', array('@node_id' => $node->nid));
      }
    }
    else {
      drupal_set_message(t('No target langauges were selected.'), 'error');
    }
  }

  return drupal_json_output($output);
}
