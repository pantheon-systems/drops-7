<?php

/**
 * @file
 * Lingotek Tab for Nodes
 */

/**
 * Download Translations Form.
 */
function lingotek_download_translations_form($form, $form_state, $node, $document = NULL) {

  module_load_include('lingotek', 'lingotek.bulk_grid', 'inc');
  $form = array();
  
  $document = is_null($document) ? LingotekDocument::load($node->lingotek['document_id']) : $document;

  $document_progress = $document->getProgress();

  $sum_progress = 0;
  if ($document_progress->results == 'success') {

    $rows = array();
    foreach ($document_progress->translationTargets as $target) {
      $current_phase = $document->currentPhase($target->id);
      $marked_complete = FALSE;

      $phase_complete_percent = is_object($current_phase) ? $current_phase->percentComplete : 0;
      if (empty($phase_complete_percent)) {
        $phase_complete_percent = 0;
      }
      $language_link = l(lingotek_language_field_lookup('native', $target->language), 'lingotek/workbench/node/' . $node->nid . '/' . $target->language, array('attributes' => array('target' => '_blank')));
      $language_link .= ' (' . lingotek_language_field_lookup('name', $target->language) . ')';

      $row = array(
        'column_1' => array('data' => $language_link, 'width' => '40%'),
        'column_2' => array('data' => lingotek_grid_create_progress_bar($target->percentComplete), 'width' => '40%'),
        'column_3' => array('data' => ''),//$target->percentComplete == 100 ? '<i class="fa fa-check-sign ltk-complete-check" title="'.t('Workflow complete').'"></i>' : '<i class="fa fa-check-empty ltk-complete-check"></i>', 'width' => '20%'
        '#attributes' => array('class' => array('bold-row')),
      );

      LingotekLog::trace("lingotek_pm table row [@locale]", array('@locale' => $target->language));
      $rows[$target->language] = $row;
      foreach ($target->phases as $phase) {
        $row = array(
          'column_1' => $phase->name,
          'column_2' => lingotek_grid_create_progress_bar($phase->percentComplete),
          'column_3' => $phase->isMarkedComplete ? '<i class="fa fa-check-sign ltk-complete-check" title="' . t('Phase complete') . '"></i>' : '<i class="fa fa-check-empty ltk-complete-check"></i>',
          '#attributes' => array('class' => array('no-checkbox-row')),
        );
        $rows[$target->language . '_' . $phase->name] = $row;
        $marked_complete = $phase->isMarkedComplete;
      }

      // update progress
      $lingotek_locale = $target->language;
      $progress_percentage = empty($target->percentComplete) ? 0 : $target->percentComplete;
      $sum_progress += $progress_percentage;
      $target_status_local = lingotek_keystore('node', $node->nid, 'target_sync_status_' . $lingotek_locale);
      if ($target_status_local === LingotekSync::STATUS_PENDING) {
        $target_status = ($marked_complete) ? LingotekSync::STATUS_READY : LingotekSync::STATUS_PENDING;
        lingotek_keystore('node', $node->nid, 'target_sync_status_' . $lingotek_locale, $target_status);
      }
    }

    $form['fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Download Translations'),
      '#description' => t('Download the latest translations from Lingotek in the selected languages.'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $header = array(
      'column_1' => t('Target Language'),
      'column_2' => t('Progress'),
      'column_3' => t('Status'),
    );

    $form['fieldset']['documents'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $rows,
      '#empty' => t('No content available.'),
      '#after_build' => array('lingotek_remove_checkboxes'),
    );

    $form['fieldset']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Download'),
    );

    $form['fieldset']['nid'] = array(
      '#type' => 'value',
      '#value' => $node->nid,
    );
  }

  return $form;
}

/**
 * Download Translations Form Submit
 */
function lingotek_download_translations_form_submit($form, $form_state) {
  $nid = $form_state['values']['nid'];

  // clear any caching from entitycache module
  if (module_exists('entitycache')) {
    cache_clear_all($nid, 'cache_entity_node');
  }

  $document_id = lingotek_keystore('node', $nid, 'document_id');

  $locales = array();
  $download_targets = array();

  foreach ($form_state['values']['documents'] as $locale => $selected) {
    if ($selected) {
      $locales[] = $locale;
      $download_targets[] = (object) array('document_id' => $document_id, 'locale' => $locale);
    }
  }

  $extra_operations = array();

  lingotek_sync_batch_create(array(), $download_targets, array(), 'node/' . $form_state['values']['nid'] . '/lingotek_pm', $extra_operations);

  drupal_set_message(t('Updated local translations for the selected languages: @selected_locales', array('@selected_locales' => implode(", ", $locales))));
  cache_clear_all('field:node:' . $form_state['values']['nid'], 'cache_field');
}

function lingotek_remove_checkboxes($form_element) {
  foreach ($form_element['#options'] as $locale => $data) {
    if (isset($data['#attributes']['class']) && in_array('no-checkbox-row', $data['#attributes']['class'])) {
      unset($form_element[$locale]['#theme']);
    }
  }
  return $form_element;
}

/**
 * Upload Content Form. (Upload to Lingotek)
 */
function lingotek_push_form($form, $form_state, $node) {
  $form = array();

  $last_uploaded_message = isset($node->lingotek['last_uploaded']) ? ' (' . t('Last uploaded @human_readable_timestamp ago.', array('@human_readable_timestamp' => lingotek_human_readable_timestamp($node->lingotek['last_uploaded']))) . ')' : '';
  $form['content_push'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload'),
    '#description' => t("Upload this node's content to Lingotek for translation. @last_uploaded_message", array('@last_uploaded_message' => $last_uploaded_message)),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['content_push']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Upload'),
  );

  $form['node_id'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );

  return $form;
}

/**
 * Submit handler for the lingotek_push_form form.
 */
function lingotek_push_form_submit($form, $form_state) {
  $node = lingotek_node_load_default($form_state['values']['node_id']);

  // clear any caching from entitycache module
  if (module_exists('entitycache')) {
    cache_clear_all($node->nid, 'cache_entity_node');
  }

  lingotek_entity_upload($node, 'node');
}

/**
 * Form constructor for the Lingotek Publish form (functionality dependent on entity_translation module installed and enabled).
 */
function lingotek_publish_form($form, $form_state, $node) {
  $form = array();

  if (module_exists('entity_translation') && lingotek_managed_by_entity_translation($node->type)) {

    $handler = entity_translation_get_handler('node', $node);
    $languages = entity_translation_languages();
    $translations = $handler->getTranslations();

    $form['publish'] = array(
      '#type' => 'fieldset',
      '#title' => t('Publish'),
      '#description' => t("Manage content visibility to your site's visitors for the selected languages."),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $options = array();

    $path = $handler->getViewPath();
    if ($path) {
      $links = EntityTranslationDefaultHandler::languageSwitchLinks($path);
    }

    foreach ($languages as $language) {
      $language_name = $language->name;
      $langcode = $language->language;
      $is_original = $langcode == $node->language;
      $translation = isset($translations->data[$langcode]) ? $translations->data[$langcode] : array();
      $translation_status = isset($translation['status']) ? $translation['status'] : 1; // default to published

      $link_label = lingotek_language_native($langcode);
      $language_link = isset($links->links[$langcode]['href']) ? $links->links[$langcode] : array('href' => $path, 'language' => $language);
      $subtext = $is_original ? '<i title="' . $langcode . '">source</i>' : '<span title="' . $langcode . '">' . lingotek_language_name($langcode) . '</span>'; //$language_name;

      $row = array(
        l($link_label, $language_link['href'], $language_link) . ' (' . $subtext . ')',
        $translation_status ? t('Published') : t('Unpublished')
      );
      $options[$langcode] = $row;
    }
    $form['publish']['languages'] = array(
      '#type' => 'tableselect',
      '#header' => array(
        t('Language'),
        t('Status')
      ),
      '#options' => $options,
    );

    $form['publish']['submit_publish'] = array(
      '#type' => 'submit',
      '#id' => 'publish',
      '#value' => t('Publish'),
    );

    $form['publish']['submit_unpublish'] = array(
      '#type' => 'submit',
      '#id' => 'unpublish',
      '#value' => t('Unpublish'),
    );

    $form['node_id'] = array(
      '#type' => 'hidden',
      '#value' => $node->nid,
    );
  }
  return $form;
}

/**
 * Submit handler for the lingotek_publish_form form.
 * Update the entity_translation module publishing fields
 */
function lingotek_publish_form_submit($form, $form_state) {
  if (module_exists('entity_translation')) {
    $node = lingotek_node_load_default($form_state['values']['node_id']);
    $status_request = $form_state['triggering_element']['#id'] == 'publish' ? 1 : 0;
    $language_codes = $form_state['values']['languages'];
    $addtl_params = array('status_request' => $status_request);

    list($languages_updated, $updates) = lingotek_entity_translation_save_status('node', $node, $language_codes, $addtl_params);

    $publish_status_text = $status_request ? 'published' : 'unpublished';
    $languages_updated_html = "<ul><li>" . implode("</li><li>", $languages_updated) . "</li></ul>";

    if ($updates > 0) {
      drupal_set_message(t('The following languages have been <b><i>@publish_status_text</i></b>: ' . $languages_updated_html, array('@publish_status_text' => $publish_status_text, '@languages_updated_html' => $languages_updated_html))); //'@node_title' => $node->title, 
    }
    else {
      drupal_set_message(t('Nothing was changed, since no languages were selected'));
    }
  }
}

/**
 * Page callback for the Lingotek local task on node detail pages.
 *
 * Construct the table summarizing information a Product Manager would want
 * to know about the progress of the translations.
 *
 * @return array
 *   A Drupal render array of the page contents.
 */
function lingotek_pm($node) {

  $storage_method = $node->lingotek['lingotek_nodes_translation_method'];
  if (lingotek_managed_by_entity_translation($node->type) && $storage_method == 'field') {

    $handler = entity_translation_get_handler('node', $node);
    $handler->initPathScheme();

    // Initialize translations if they are empty.
    $translations = $handler->getTranslations();
    if (empty($translations->original)) {
      $handler->initTranslations();
      $handler->saveTranslations();
    }
  }
  // Display all of the appropriate node management sections (e.g., Upload, Download, Publish)

  $output = array(
    '#attached' => array(
      'css' => array(
        '//netdna.bootstrapcdn.com/font-awesome/3.2.1/css/font-awesome.css' => array(
          'type' => 'external',
        ),
      ),
    ),
  );
  
  $sync_status = $node->lingotek['node_sync_status'];

  // node translation support
  if ($node->tnid != 0 && $node->tnid != $node->nid) {
    $output[] = array(
      '#markup' => t('This is a translated node. Go to the <a href="@link">source node</a> to manage the translations.', array('@link' => url('node/' . $node->tnid . '/lingotek_pm'))),
    );
    return $output;
  }

  // node disabled support
  if ($node->lingotek['profile'] == LingotekSync::PROFILE_DISABLED) {
    $output[] = array(
      '#markup' => t('Lingotek Translation is currently disabled for this node. You can enable translation by editing this node and selecting a Translation Profile (e.g. "Automatic").')
    );
    return $output;
  }

  if (lingotek_supported_node($node) && Lingotek::isSupportedLanguage($node->language)) {

    if (($node->lingotek['node_sync_status'] == LingotekSync::STATUS_EDITED)) {
      $content_push_form = drupal_get_form('lingotek_push_form', $node);
      $output['content_push'] = array(
        '#markup' => drupal_render($content_push_form),
      );
    }

    if ($node->lingotek['document_id']) {

      $document = LingotekDocument::load($node->lingotek['document_id']);
      $progress = $document->getProgress();
      
      if ($progress->results == 'success') {

        $download_form = drupal_get_form('lingotek_download_translations_form', $node, $document);
        $output[] = array(
          '#markup' => drupal_render($download_form),
        );
        
        if ($document->hasPhasesToComplete()) {
          // Add the mark as complete table if there are complete-eligible phrases.
          $drupal_phase_complete_from = drupal_get_form('lingotek_mark_phases_complete', $node, $document);
          $output['mark_complete'] = array(
            '#markup' => drupal_render($drupal_phase_complete_from),
          );
        }

        $lingotek_advanced_parsing_form = drupal_get_form('lingotek_advanced_parsing_upgrade_form');
        $output['upgrade_form'] = array(
          '#markup' => drupal_render($lingotek_advanced_parsing_form),
        );

      }
      else {
        // Document not found (may still be importing)
        // Import Status (check and report on import status)
        $status = $document->getImportStatus();

        $message = ' <i>' . check_plain($status) . '</i> ' . l('<i class="fa fa-refresh"></i> ' . t('Refresh'), '', array('html' => TRUE, 'attributes' => array('class' => 'ltk-icon', 'title' => t('Refresh'), 'onclick' => array('location.reload();return false;'))));
        drupal_set_message(t('Content Import Status:') . ' ' . $message, 'status');

        $output['import_status'] = array(
          '#type' => 'fieldset',
          '#title' => t('Content Import Status'),
          '#description' => $message,
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );
      
      }
      
      /* REMOVE DISASSOCIATE_TRANSLTIONS FORM FROM THE TRANSLATE PAGE, FOR NOW
      $disassociate_translations_form = drupal_get_form('lingotek_entity_disassociate_form', $node->nid);
      $output['disassociate_translations'] = array(
        '#markup' => drupal_render($disassociate_translations_form),
      );
      */
    }
  }
  else {
    $output[] = array(
      '#markup' => '<p class="help">' . t('This node is not compatible with Lingotek translation. Either it is not a Lingotek tranlsation-enabled content type or the node does not have a defined language.') . '</p>'
    );
  }

  // Publish form
  $output['publish_form'] = drupal_get_form('lingotek_publish_form', $node);

  return $output;
}

/**
 * Form constructor for parsing upgrade of a node.
 *
 * @return array
 *   A FAPI form array.
 */
function lingotek_advanced_parsing_upgrade_form($form_state) {
  $form = array();

  if (!variable_get('lingotek_advanced_parsing', FALSE)) {
    $router_item = menu_get_item();
    if (!empty($router_item['page_arguments'][0]->nid)) {
      $node_id = $router_item['page_arguments'][0]->nid;

      $form['node_id'] = array(
        '#type' => 'hidden',
        '#value' => $node_id,
      );

      $form['advanced_parsing_upgrade'] = array(
        '#type' => 'fieldset',
        '#title' => t('Advanced Content Parsing'),
        '#description' => t('Your site is currently set to use legacy ("simple") content parsing. Use the button below to upgrade this node to advanced content parsing.'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      $advanced_parsing = lingotek_keystore('node', $node_id, 'use_advanced_parsing');
      if (empty($advanced_parsing)) {
        $form['advanced_parsing_upgrade']['submit'] = array(
          '#type' => 'submit',
          '#value' => t('Upgrade node'),
        );
      }
      else {
        $form['advanced_parsing_upgrade']['already_upgraded'] = array(
          '#markup' => t('This node has already been upgraded to use advanced parsing.')
        );
      }
    }
    else {
      LingotekLog::error('Unable to locate node ID for advanced parsing upgrade form: @path', array('@path' => $_GET['q']));
    }
  }

  return $form;
}

/**
 * Submit handler for the lingotek_advanced_parsing_upgrade_form form.
 *
 * @param array $form
 *   A FAPI form array.
 * @param array $form_state
 *   A FAPI form state array.
 */
function lingotek_advanced_parsing_upgrade_form_submit($form, $form_state) {
  $error = FALSE;

  if (!empty($form_state['values']['node_id'])) {
    lingotek_keystore('node', $form_state['values']['node_id'], 'use_advanced_parsing', 1);
    $target_node = lingotek_node_load_default($form_state['values']['node_id']);
    if ($target_node->nid) {
      if (LingotekApi::instance()->updateContentDocument(LingotekEntity::load($target_node))) {
        drupal_set_message(t('This node has been upgraded to use advanced content parsing.'));
      }
      else {
        $error = TRUE;
        LingotekLog::error('Error updating node for advanced content parsing. Lingotek updateContentDocument call failed.', array());
      }
    }
    else {
      $error = TRUE;
      LingotekLog::error('Unable to load target node for content parsing upgrade: @node_id', array('@node_id' => $form_state['values']['node_id']));
    }
  }
  else {
    $error = TRUE;
    LingotekLog::error('No target node ID in parsing upgrade form data.', array());
  }

  if ($error) {
    drupal_set_message(t('There was an error upgrading this node. It has <strong>not</strong> been updated to use advanced parsing.'), 'error');
  }
}

/**
 * Form constructor for the lingotek_mark_phases_complete form.
 *
 * @param array $form
 *   A FAPI form array.
 * @param array $form_state
 *   A FAPI form state array.
 * @param object $node
 *   The Drupal node whose complete-eligible phases should be displayed.
 *
 * @return array
 *   A FAPI form data array.
 */
function lingotek_mark_phases_complete($form, $form_state, $node, $document = NULL) {
  $form = array();

  $document = is_null($document) ? LingotekDocument::load($node->lingotek['document_id']) : $document;
  $document_id = $document->document_id;
  if (class_exists('LingotekPhase') && $document_id) {
    $api = LingotekApi::instance();
    $progress = $document->getProgress();
    
    if ($progress) {
      $targets = $progress->translationTargets;

      foreach ($targets as $target) {
        $language = Lingotek::convertLingotek2Drupal($target->language);
        $current_phase = $document->currentPhase($target->id);

        $phase_complete_percent = empty($current_phase->percentComplete) ? 0 : $current_phase->percentComplete;

        if ($current_phase && $current_phase->canBeMarkedComplete()) {
          $phase_link = l($current_phase->name, '', array('attributes' => array(
              'onclick' => 'window.open(\'' . $api->getWorkbenchLink($document_id, $current_phase->id) . '\'); return false;')));

          $row = array(
            lingotek_language_native($language) . ' (' . lingotek_language_name($language) . ')',
            $phase_link,
            $phase_complete_percent . '%',
          );

          $options[$current_phase->id] = $row;
        }
      }

      $form['mark_complete'] = array(
        '#type' => 'fieldset',
        '#title' => t('Mark Workflow Phases complete'),
        '#description' => t('The following Translation Targets have Phases that can be marked as complete.'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      $form['mark_complete']['phases'] = array(
        '#type' => 'tableselect',
        '#header' => array(
          t('Language'),
          t('Phase'),
          t('Phase Progress'),
        ),
        '#options' => $options,
      );

      $form['mark_complete']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Mark Selected Phases as Complete'),
      );
    }
    else {
      LingotekLog::error('Unable to build mark as complete form: could not get progress data from API.');
    }
  }

  return $form;
}

/**
 * Submit handler for the lingotek_mark_phases_complete form.
 */
function lingotek_mark_phases_complete_submit($form, $form_state) {
  if (!empty($form_state['values']['phases'])) {
    $api = LingotekApi::instance();
    $errors = FALSE;
    foreach ($form_state['values']['phases'] as $phase_id) {
      if ($phase_id) {
        if (!$api->markPhaseComplete($phase_id)) {
          $errors = TRUE;
        }
      }
    }

    if (!$errors) {
      drupal_set_message(t('All selected phases were marked as complete.'));
    }
    else {
      drupal_set_message(t('There were errors marking one or more phases as complete.'), 'error');
    }
  }
  else {
    drupal_set_message(t('No phases were selected.'), 'error');
  }
}

/**
 * Page handler for manually refreshing the local translations of a Lingotek-assciated comment.
 *
 * @param int $comment_id
 *   The ID of a Drupal comment.
 */
function page_sync_comment_translations($comment_id) {
  if (!empty($_GET['token']) && drupal_valid_token($_GET['token'])) {
    if (class_exists('LingotekComment') && class_exists('LingotekApi')) {
      $comment = LingotekComment::loadById($comment_id);
      if ($lingotek_document_id = $comment->getMetadataValue('document_id')) {
        if ($document = LingotekApi::instance()->getDocument($lingotek_document_id)) {
          if ($document->percentComplete == 100) {
            if ($comment->updateLocalContent()) {
              drupal_set_message(t('Local translations for the comment have been updated.'));
            }
            else {
              LingotekLog::error('Unable to update local translations for comment @id', array('@id' => $comment_id));
              drupal_set_message(t('An error occurred. Local translations for the comment were not updated.'), 'error');
            }
          }
          else {
            drupal_set_message(t('The translation of this comment is not yet complete. Please try again later.'), 'warning');
          }
        }
        else {
          LingotekLog::error('Unable to retrieve the status of the lingotek document: @id while updating
            comment @comment_id', array('@id' => $lingotek_document_id, '@comment_id' => $comment_id), WATCHDOG_ERROR);

          drupal_set_message(t('An error occurred. Local translations for the comment were not updated.'), 'error');
        }
      }
      else {
        LingotekLog::error('Attempt to refresh local translations for comment @id, but the comment
          is not associated with a Lingotek document.', array('@id' => $comment_id));
      }
    }
  }
  else {
    LingotekLog::error('Attempt to refresh local translations for comment @id without a valid security token.', array('@id' => $comment_id));
  }

  drupal_goto();
}

/*
 * 
 */
function lingotek_workbench_redirect($entity_type, $entity_id, $lingotek_locale) {
  $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale);

  if (module_exists('entitycache')) {
    cache_clear_all($entity_id, "cache_entity_$entity_type");
  }

  $entity = lingotek_entity_load_single($entity_type, $entity_id);

  if ($entity->language == $drupal_language_code) {
    drupal_goto($entity_type . '/' . $entity_id . '/edit');
  }
  else {
    $translation_edit_link = lingotek_get_workbench_url($entity->lingotek['document_id'], $lingotek_locale);
    drupal_goto($translation_edit_link);
  }
}

/**
 * This is a function that redirects to the entity specified for a particular lingotek locale (e.g., en_US, de_DE, fr_FR)
 */
function lingotek_entity_view_redirect($entity_type, $entity_id, $lingotek_locale) {
  $drupal_language_code = Lingotek::convertLingotek2Drupal($lingotek_locale, TRUE);
  $url = drupal_get_normal_path($entity_type . '/' . $entity_id); //assumes url detection
  $url_language_detection_enabled = TRUE; //TO-DO: detect when it is not enabled, currently it is assumed

  if ($drupal_language_code) {
    $default_language = language_default();
    if ($drupal_language_code !== $default_language->language) {
      $url = $drupal_language_code . "/" . $url;
    }
  }
  else {
    drupal_set_message(t('The language requested is not available. The default language will be shown instead.'), 'warning');
  }
  if (!$url_language_detection_enabled) { //TO-DO: consider extending this functionality to use additional language detection methods
    $output = array();
    $message = t('The URL detection method is required for viewing content in a specified language. In order to use this feature an administrator will need to enable the URL detection method in the <a href="@link">Language detection and selection settings</a>', array('@link' => url('admin/config/regional/language/configure')));
    $output['message'] = array(
      '#type' => 'fieldset',
      '#title' => t('This feature requires URL language detection to be enabled'),
      '#description' => $message,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    drupal_set_message($message, 'warning');
    return $output;
  }
  drupal_goto($url);
}
