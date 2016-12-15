<?php

/**
 * @file
 * Common pages for the Media WYSIWYG module.
 */

/**
 * Form callback used when embedding media.
 *
 * Allows the user to pick a format for their media file.
 * Can also have additional params depending on the media type.
 */
function media_wysiwyg_format_form($form, &$form_state, $file) {
  $form_state['file'] = $file;

  // Allow for overrides to the fields.
  $query_fields = isset($_GET['fields']) ? drupal_json_decode($_GET['fields']) : array();
  $fields = media_wysiwyg_filter_field_parser(array('fields' => $query_fields), $file);

  $options = media_wysiwyg_get_file_view_mode_options($file);

  if (!count($options)) {
    throw new Exception('Unable to continue, no available formats for displaying media.');
  }

  // Generate all the previews.
  if (!isset($form_state['storage']['view_mode_previews'])) {
    $form_state['storage']['view_mode_previews'] = array();
    foreach ($options as $view_mode => $view_mode_label) {
      $view_mode_preview = media_wysiwyg_get_file_without_label($file, $view_mode, array('wysiwyg' => TRUE));
      $form_state['storage']['view_mode_previews'][$view_mode] = drupal_render($view_mode_preview);
    }
  }

  // Add the previews back into the form array so they can be altered.
  $form['#formats'] = &$form_state['storage']['view_mode_previews'];

  // Allow for overrides to the display format.
  $default_view_mode = is_array($query_fields) && isset($query_fields['format']) ? $query_fields['format'] : variable_get('media_wysiwyg_wysiwyg_default_view_mode', 'full');
  if (!isset($options[$default_view_mode])) {
    reset($options);
    $default_view_mode = key($options);
  }

  // Add the previews by reference so that they can easily be altered by
  // changing $form['#formats'].
  $settings['media']['formatFormFormats'] = &$form_state['storage']['view_mode_previews'];
  $form['#attached']['js'][] = array('data' => $settings, 'type' => 'setting');

  // Add the required libraries, JavaScript and CSS for the form.
  $form['#attached']['library'][] = array('media', 'media_base');
  $form['#attached']['library'][] = array('system', 'form');
  $form['#attached']['css'][] = drupal_get_path('module', 'media_wysiwyg') . '/css/media_wysiwyg.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'media_wysiwyg') . '/js/media_wysiwyg.format_form.js';

  $form['title'] = array(
    '#markup' => t('Embedding %filename', array('%filename' => $file->filename)),
  );

  $preview = media_get_thumbnail_preview($file);

  $form['preview'] = array(
    '#type' => 'markup',
    '#markup' => drupal_render($preview),
  );

  // These will get passed on to WYSIWYG.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('options'),
  );

  // @TODO: Display more verbose information about which formatter and what it
  // does.
  $form['options']['format'] = array(
    '#type' => 'select',
    '#title' => t('Display as'),
    '#options' => $options,
    '#default_value' => $default_view_mode,
    '#description' => t('Choose the type of display you would like for this
      file. Please be aware that files may display differently than they do when
      they are inserted into an editor.')
  );

  // Add fields from the file, so that we can override them if necessary.
  $form['options']['fields'] = array();
  foreach ($fields as $field_name => $field_value) {
    $file->{$field_name} = $field_value;
  }
  field_attach_form('file', $file, $form['options']['fields'], $form_state);
  $instance = field_info_instances('file', $file->type);
  foreach ($instance as $field_name => $field_value) {
    $info = field_info_field($field_name);
    // Only single valued fields can be overridden.
    $allow = $info['cardinality'] == 1 && !empty($instance[$field_name]['settings']['wysiwyg_override']);
    $form['options']['fields'][$field_name]['#access'] = $allow;
  }

  // Add view mode preview.
  media_wysiwyg_format_form_view_mode($form, $form_state, $file);

  // Similar to a form_alter, but we want this to run first so that
  // media.types.inc can add the fields specific to a given type (like alt tags
  // on media). If implemented as an alter, this might not happen, making other
  // alters not be able to work on those fields.
  // @todo: We need to pass in existing values for those attributes.
  drupal_alter('media_wysiwyg_format_form_prepare', $form, $form_state, $file);

  if (!element_children($form['options'])) {
    $form['options']['#attributes'] = array('style' => 'display:none');
  }

  return $form;
}

/**
 * Add ajax preview when selecting view mode in wysiwyg editor.
 */
function media_wysiwyg_format_form_view_mode(&$form, $form_state, $file)  {
  // Check to see if a view mode ("format") has already been specified for
  //  this media item. First, check for a standard form-submitted value.
  if (!empty($form_state['values']['format'])) {
    $view_mode = $form_state['values']['format'];
  }
  // Second, check the request for a JSON-encoded value.
  elseif (isset($_GET['fields'])) {
    $query_fields = drupal_json_decode($_GET['fields']);
    if (isset($query_fields['format'])) {
      $view_mode = $query_fields['format'];
    }
  }
  // If we were unable to determine a view mode, or we found a view mode
  //  that does not exist in the list of format options presented on this
  //  form, use the default view mode.
  if (!isset($view_mode) || !array_key_exists($view_mode, $form['options']['format']['#options'])) {
    $view_mode = variable_get('media_wysiwyg_wysiwyg_default_view_mode', 'full');
  }

  $form['preview'] = array();
  $form['preview']['#prefix'] = '<div class="media-item"><div class="media-thumbnail">';
  $form['preview']['#suffix'] = '</div><div class="label-wrapper"><label class="media-filename">' . check_plain($file->filename) . '</label></div></div>';
  $form['preview']['thumbnail'] = file_view_file($file, $view_mode);
  $form['preview']['thumbnail']['#prefix'] = '<div id="media-preview">';
  $form['preview']['thumbnail']['#suffix'] = '</div>';

  if (!isset($form['options']['format']['#default_value'])) {
    $form['options']['format']['#default_value'] = $view_mode;
  }
  $form['options']['format']['#ajax'] = array(
    'callback' => 'media_wysiwyg_format_form_preview',
    'wrapper' => 'media-preview',
  );

  $wysiwyg_view_mode = db_query('SELECT view_mode FROM {media_view_mode_wysiwyg} WHERE type = :type', array(':type' => $file->type))->fetchField();
  $view_modes = media_wysiwyg_get_wysiwyg_allowed_view_modes($file);
  $formats = $options = array();

  foreach ($view_modes as $view_mode => $view_mode_info) {
    $options[$view_mode] = $view_mode_info['label'];

    if (!empty($wysiwyg_view_mode)) {
      $element = media_wysiwyg_get_file_without_label($file, $wysiwyg_view_mode, array('wysiwyg' => TRUE));
    }
    else {
      $element = media_wysiwyg_get_file_without_label($file, $view_mode, array('wysiwyg' => TRUE));
    }

    $formats[$view_mode] = drupal_render($element);
  }

  $form['#formats'] = $formats;
  $form['options']['format']['#options'] = $options;
}

/**
 * AJAX callback to select the portion of the format form to be updated with a preview.
 *
 * @param array $form
 *   An associative array containing the structure of the form.
 * @param array $form_state
 *   An associative array containing the current state of the form.
 *
 * @return array
 *   The preview form item.
 */
function media_wysiwyg_format_form_preview($form, $form_state) {
  return $form['preview']['thumbnail'];
}
