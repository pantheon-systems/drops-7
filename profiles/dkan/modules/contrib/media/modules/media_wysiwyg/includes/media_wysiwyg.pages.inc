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

  $view_modes = media_wysiwyg_get_wysiwyg_allowed_view_modes($file);
  $formats = $options = array();
  foreach ($view_modes as $view_mode => $view_mode_info) {
    // @TODO: Display more verbose information about which formatter and what it
    // does.
    $options[$view_mode] = $view_mode_info['label'];
    $element = media_wysiwyg_get_file_without_label($file, $view_mode, array('wysiwyg' => TRUE));

    // Make a pretty name out of this.
    $formats[$view_mode] = drupal_render($element);
  }

  // Add the previews back into the form array so they can be altered.
  $form['#formats'] = &$formats;

  if (!count($formats)) {
    throw new Exception('Unable to continue, no available formats for displaying media.');
    return;
  }

  // Allow for overrides to the display format.
  $default_view_mode = is_array($query_fields) && isset($query_fields['format']) ? $query_fields['format'] : variable_get('media_wysiwyg_wysiwyg_default_view_mode', 'full');
  if (!isset($formats[$default_view_mode])) {
    $default_view_mode = key($formats);
  }

  // Add the previews by reference so that they can easily be altered by
  // changing $form['#formats'].
  $settings['media']['formatFormFormats'] = &$formats;
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
    '#title' => check_plain(basename($file->uri)),
    '#markup' => drupal_render($preview),
  );

  // These will get passed on to WYSIWYG.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('options'),
  );

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
    if (isset($instance[$field_name]['settings']) && isset($instance[$field_name]['settings']['wysiwyg_override']) && !$instance[$field_name]['settings']['wysiwyg_override']) {
      unset($form['options']['fields'][$field_name]);
    }
  }

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
