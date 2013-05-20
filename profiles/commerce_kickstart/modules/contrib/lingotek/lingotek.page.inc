<?php

/**
 * @file
 * Lingotek Tab for Nodes
 */

/**
 * Download Translations Form.
 */
function lingotek_download_translations_form($form, $form_state, $node, $document = NULL) {

  $icon_green_check = base_path() . drupal_get_path('module', 'lingotek') . '/images/ico_tick_17x.png';
  $checkbox = '<input type="checkbox" onchange="lingotek.pm.toggle_checkboxes(this);" />';
  $headers = array(
    $checkbox,
    t('Language'),
    t('Document Progress'),
    t('Phase'),
    t('Phase Progress'),
    t('Phase Complete'),
  );

  $document = is_null($document) ? LingotekDocument::load(lingotek_lingonode($node->nid, 'document_id')) : $document;
  $targets = lingotek_get_document_targets($document->document_id, TRUE);
  $rows = array();
  foreach ($targets as $lingotek_locale => $target) {
    $current_phase = $document->currentPhase($target->id);

    $phase_complete = ($current_phase->isMarkedComplete) ? TRUE : FALSE;

    $phase_complete_percent = is_object($current_phase) ? $current_phase->percentComplete : 0;
    if (empty($phase_complete_percent)) {
      $phase_complete_percent = 0;
    }
    $language_link = l(lingotek_language_field_lookup('native', $lingotek_locale), '', array('attributes' => array('onclick' => 'window.open(\'' . lingotek_get_workbench_url($node, $lingotek_locale) . '\'); return false;')));
    $language_link .= ' ' . t('(@language_name)', array('@language_name' => lingotek_language_field_lookup('name', $lingotek_locale)));
    $row = array(
      '<input type="checkbox" tag="lingotek_pm_row" language="' . $lingotek_locale . '" />',
      $language_link,
      $target->percentComplete . '%',
      $current_phase->name,
      $phase_complete_percent . '%',
      ($phase_complete) ? '<img src="' . $icon_green_check . '" />' : '',
    );

    LingotekLog::trace("lingotek_pm table row [@locale]", array('@locale' => $lingotek_locale));
    $rows[] = $row;
  }

  $table = theme('table', array('header' => $headers, 'rows' => $rows));

  $actions = '<input type="button" id="lingotek-update-button" value="' . t('Download') . '" class="form-submit"/>';

  $vars = '<input type="hidden" id="lingotek_nid" value="' . $node->nid . '" />';

  $token_element = array(
    '#type' => 'hidden',
    '#attributes' => array('id' => 'submit-token'),
    '#value' => drupal_get_token(),
  );
  $vars .= drupal_render($token_element);

  $download_form = array();

  //$node_auto_download = (lingotek_lingonode($node->nid, 'sync_method'));
  $download_form['fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download Translations'),
    '#description' => t('Download the latest translations from Lingotek in the selected languages.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE //!$node_auto_download,
  );

  $download_form['fieldset']['contents'] = array(
    '#markup' => $table . $actions . $vars,
  );

  return $download_form;
}

/**
 * Download Translations Form Submit
 */
function lingotek_download_translations_form_submit($form, $form_state) {
  
}

/**
 * Upload Content Form. (Upload to Lingotek)
 */
function lingotek_push_form($form, $form_state, $node) {
  $form = array();

  $form['content_push'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload'),
    '#description' => t("Upload this node's content to Lingotek for translation."),
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
  $node = node_load($form_state['values']['node_id']);

  $api = LingotekApi::instance();
  if ($existing_document = lingotek_lingonode($node->nid, 'document_id')) {
    // Update an existing Lingotek Document.
    $api->updateContentDocument($node);
  }
  else {
    // Create a new Lingotek Document.
    $api->addContentDocument($node, TRUE);
  }

  drupal_set_message(t('Uploaded content for "@node_title" to Lingotek for translation.', array('@node_title' => $node->title)));
}

/**
 * Form constructor for the node content reset form. (Reset Translations)
 */
function lingotek_node_content_reset_form($form, $form_state, $node) {
  $form = array();

  $form['reset_translations'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reset Translations'),
    '#description' => t("Disassociates the node's locally-published translations from their counterparts on Lingotek's servers. This allows for your Drupal content to be re-uploaded to Lingotek, so that it can be retranslated entirely using the currently-defined workflow. Also resets the node's translation management settings to their defaults."),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['reset_translations']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Reset Translations'),
  );

  $form['node_id'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );

  return $form;
}

/**
 * Submit handler for the lingotek_node_content_reset form.
 */
function lingotek_node_content_reset_form_submit($form, $form_state) {
  $drupal_node_id = $form_state['values']['node_id'];
  $doc_id = LingotekSync::getDocIdFromNodeId($drupal_node_id);

  $api = LingotekApi::instance();
  $api->removeDocument($doc_id);

  $node = node_load($drupal_node_id);
  drupal_set_message(t('Translations reset for "@node_title".', array('@node_title' => $node->title)));
}

/**
 * Form constructor for the Lingotek Publish form (functionality dependent on entity_translation module installed and enabled).
 */
function lingotek_publish_form($form, $form_state, $node) {
  $form = array();

  if (module_exists('entity_translation')) {

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
    $node = node_load($form_state['values']['node_id']);

    $status_request = $form_state['triggering_element']['#id'] == 'publish' ? 1 : 0;
    $handler = entity_translation_get_handler('node', $node);

    $language_codes = $form_state['values']['languages'];
    $languages_updated = array();
    $updates = 0;
    foreach ($language_codes as $langcode) {
      if ($langcode !== 0) {
        // set node (source or target as specified) flag (in the entity_translation table) as published or unpublished
        $handler->setTranslation(array(
          'language' => $langcode,
          'status' => $status_request
        ));
        $languages_updated[$langcode] = lingotek_language_name($langcode);
        $updates++;
      }
    }
    $handler->saveTranslations();

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
  // Display all of the appropriate node management sections (e.g., Upload, Download, Publish)

  $output = array();

  drupal_add_js(drupal_get_path('module', 'lingotek') . '/js/lingotek.pm.js');

  if (class_exists('LingotekDocument')) {
    $document = LingotekDocument::load(lingotek_lingonode($node->nid, 'document_id'));

    if (lingotek_supported_type($node->type) && Lingotek::isSupportedLanguage($node->language)) {

      $node_pushed = lingotek_node_pushed($node);
      $node_auto_upload = (lingotek_lingonode($node->nid, 'create_lingotek_document'));
      if (!$node_pushed || !$node_auto_upload) {
        $content_push_form = drupal_get_form('lingotek_push_form', $node);
        $output['content_push'] = array(
          '#markup' => drupal_render($content_push_form),
        );
      }

      if ($node_pushed) {
        $download_form = drupal_get_form('lingotek_download_translations_form', $node, $document);
        $output[] = array(
          '#markup' => drupal_render($download_form),
        );

        // Add the mark as complete table if there are complete-eligible phrases.
        if ($document->hasPhasesToComplete()) {
          $drupal_phase_complete_from = drupal_get_form('lingotek_mark_phases_complete', $node, $document);
          $output['mark_complete'] = array(
            '#markup' => drupal_render($drupal_phase_complete_from),
          );
        }

        $lingotek_advanced_parsing_form = drupal_get_form('lingotek_advanced_parsing_upgrade_form');
        $output['upgrade_form'] = array(
          '#markup' => drupal_render($lingotek_advanced_parsing_form),
        );

        $reset_translations_form = drupal_get_form('lingotek_node_content_reset_form', $node);
        $output['reset_translations'] = array(
          '#markup' => drupal_render($reset_translations_form),
        );
      }
    }
    else {
      $output[] = array(
        '#markup' => '<p class="help">' . t('This node is not compatible with Lingotek translation. Either it is not a Lingotek tranlsation-enabled content type or the node does not have a defined language.') . '</p>'
      );
    }

    // Publish form
    $output['publish_form'] = drupal_get_form('lingotek_publish_form', $node);
  }

  return $output;
}

/*
 * Synchonize translations with what is available on the Lingotek platform (current translations)
 */

function lingotek_update($nid) {
  $node = node_load($nid);
  foreach ($_POST['targets'] as $locale) {
    lingotek_download_document($node, $locale);
  }
  drupal_set_message(t('Updated local translations for the selected languages: @selected_locales', array('@selected_locales' => implode(", ", $_POST['targets']))));
  drupal_json_output(array('status' => 0, 'updated' => $_POST['targets'], 'nid' => $nid));
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

      $advanced_parsing = lingotek_lingonode($node_id, 'use_advanced_parsing');
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
    lingotek_lingonode($form_state['values']['node_id'], 'use_advanced_parsing', 1);
    $target_node = node_load($form_state['values']['node_id']);
    if ($target_node->nid) {
      if (LingotekApi::instance()->updateContentDocument(LingotekNode::load($target_node))) {
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

  $document = is_null($document) ? LingotekDocument::load(lingotek_lingonode($node->nid, 'document_id')) : $document;
  $document_id = $document->document_id; //lingotek_lingonode($node->nid, 'document_id');
  if (class_exists('LingotekPhase') && $document_id) {
    $api = LingotekApi::instance();

    //$document = LingotekDocument::load($document_id);
    if ($progress = $document->translationProgress()) {
      $targets = $progress->translationTargets;

      foreach ($targets as $target) {
        $language = Lingotek::convertLingotek2Drupal($target->language);
        $current_phase = $document->currentPhase($target->id);

        $phase_complete_percent = $current_phase->percentComplete;
        if (empty($phase_complete_percent)) {
          $phase_complete_percent = 0;
        }

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
