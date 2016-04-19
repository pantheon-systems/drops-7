<?php

/**
 * @file
 * Written by Henri MEDOT <henri.medot[AT]absyx[DOT]fr>
 * http://www.absyx.fr
 */

/**
 * Implements hook_field_info_alter().
 */
function file_resup_field_info_alter(&$info) {
  foreach (array('file', 'image') as $type) {
    if (isset($info[$type])) {
      $info[$type]['instance_settings'] += array(
        'resup' => 0,
        'resup_max_filesize' => '',
        'resup_autostart' => 0,
      );
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter() for field_ui_field_edit_form().
 */
function file_resup_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {
  if (file_resup_field_widget_support($form['#instance']['widget']['type'])) {
    $settings = $form['#instance']['settings'];

    $additions['resup_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Resumable upload settings'),
      '#collapsible' => TRUE,
      '#collapsed' => !$settings['resup'],
      '#parents' => array('instance', 'settings'),
      '#weight' => 20,
    );

    $additions['resup_settings']['resup'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable resumable upload'),
      '#default_value' => $settings['resup'],
    );

    $additions['resup_settings']['resup_max_filesize'] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum upload size'),
      '#default_value' => $settings['resup_max_filesize'],
      '#description' => t("You may enter a value greater than the default maximum upload size above. It can exceed PHP's maximum post and file upload sizes as well."),
      '#size' => 10,
      '#element_validate' => array('_file_generic_settings_max_filesize'),
    );

    $additions['resup_settings']['resup_autostart'] = array(
      '#type' => 'checkbox',
      '#title' => t('Start upload on files added'),
      '#default_value' => $settings['resup_autostart'],
      '#description' => t('When checked, upload will start as soon as files are added without requiring to click <em>Upload</em>, unless some of the added files did not pass validation.'),
    );

    $form['instance']['settings'] += $additions;
  }
}

/**
 * Implements hook_field_widget_form_alter().
 */
function file_resup_field_widget_form_alter(&$element, &$form_state, $context) {
  $instance = $context['instance'];
  if (!empty($instance['settings']['resup']) && file_resup_field_widget_support($instance['widget']['type']) && user_access('upload via file_resup') && file_upload_max_size() >= file_resup_chunksize()) {
    $keys = element_children($element);
    $delta = end($keys);

    $cardinality = $context['field']['cardinality'];
    $element[$delta]['#file_resup_max_files'] = $cardinality != FIELD_CARDINALITY_UNLIMITED ? $cardinality - $delta : -1;

    $upload_validators = $element[$delta]['#upload_validators'];
    $upload_validators['file_validate_size'] = array(file_resup_max_filesize($instance));
    $element[$delta]['#file_resup_upload_validators'] = $upload_validators;

    $element[$delta]['#process'][] = 'file_resup_field_widget_process';
    $element[$delta]['#file_value_callbacks'][] = 'file_resup_field_widget_value';
  }
}

/**
 * #process callback for the field widget element.
 */
function file_resup_field_widget_process($element, &$form_state, $form) {
  $path = drupal_get_path('module', 'file_resup');
  $max_files = $element['#file_resup_max_files'];

  // Get the upload validators and build a new description.
  $field = field_widget_field($element, $form_state);
  $instance = field_widget_instance($element, $form_state);
  $description = $field['cardinality'] == 1 ? field_filter_xss($instance['description']) : '';
  $upload_validators = $element['#file_resup_upload_validators'];
  $description = theme('file_upload_help', array('description' => $description, 'upload_validators' => $upload_validators));

  // Add the resup element.
  $element['resup'] = array(
    '#type' => 'hidden',
    '#value_callback' => 'file_resup_value',
    '#field_name' => $element['#field_name'],
    '#field_parents' => $element['#field_parents'],
    '#upload_location' => $element['#upload_location'],
    '#file_resup_upload_validators' => $upload_validators,
    '#attributes' => array(
      'class' => array('file-resup'),
      'data-upload-name' => $element['upload']['#name'],
      'data-upload-button-name' => $element['upload_button']['#name'],
      'data-max-filesize' => $upload_validators['file_validate_size'][0],
      'data-description' => $description,
      'data-url' => url('file_resup/upload/' . implode('/', $element['#array_parents']) . '/' . $form['form_build_id']['#value']),
      'data-drop-message' => $max_files > -1 ? format_plural($max_files, 'Drop a file here or click <em>Browse</em> below.', 'Drop up to @count files here or click <em>Browse</em> below.') : t('Drop files here or click <em>Browse</em> below.'),
    ),
    '#prefix' => '<div class="file-resup-wrapper">',
    '#suffix' => '</div>',
    '#attached' => array(
      'css' => array($path . '/file_resup.css'),
      'js' => array(
        $path . '/js/resup.min.js',
        $path . '/file_resup.js',
        array(
          'type' => 'setting',
          'data' => array('file_resup' => array('chunk_size' => file_resup_chunksize())),
        ),
      ),
    ),
  );

  // Add the extension list as a data attribute.
  if (isset($upload_validators['file_validate_extensions'][0])) {
    $extension_list = implode(',', array_filter(explode(' ', $upload_validators['file_validate_extensions'][0])));
    $element['resup']['#attributes']['data-extensions'] = $extension_list;
  }

  // Add the maximum number of files as a data attribute.
  if ($max_files > -1) {
    $element['resup']['#attributes']['data-max-files'] = $max_files;
  }

  // Add autostart as a data attribute.
  if ($instance['settings']['resup_autostart']) {
    $element['resup']['#attributes']['data-autostart'] = 'on';
  }

  $element['upload_button']['#submit'][] = 'file_resup_field_widget_submit';
  $element['#pre_render'][] = 'file_resup_field_widget_pre_render';

  return $element;
}

/**
 * #file_value_callbacks callback for the field widget element.
 */
function file_resup_field_widget_value(&$element, &$input, $form_state) {
  if (!empty($input['resup'])) {
    $resup_file_ids = explode(',', $input['resup']);
    $resup_file_id = reset($resup_file_ids);
    if ($file = file_resup_save_upload($element, $resup_file_id)) {
      $input['fid'] = $file->fid;
      // When anonymous, file_managed_file_value() does not allow previously
      // uploaded temporary files to be reused, so we also need to pass fid
      // through element's default value.
      if ($file->status != FILE_STATUS_PERMANENT && !$GLOBALS['user']->uid) {
        if ($element['#extended']) {
          $element['#default_value']['fid'] = $file->fid;
          // 'display' must be passed as well, as an integer.
          $element['#default_value']['display'] = $input['display'];
        }
        else {
          $element['#default_value'] = $file->fid;
        }
      }
    }
  }
}

/**
 * #value_callback callback for the resup element.
 */
function file_resup_value($element, $input = FALSE, $form_state = array()) {
  $fids = array();

  if ($input) {
    $resup_file_ids = explode(',', $input);
    array_shift($resup_file_ids);
    if (isset($element['#attributes']['data-max-files'])) {
      $resup_file_ids = array_slice($resup_file_ids, 0, max(0, $element['#attributes']['data-max-files'] - 1));
    }
    foreach ($resup_file_ids as $resup_file_id) {
      if ($file = file_resup_save_upload($element, $resup_file_id)) {
        $fids[] = $file->fid;
      }
    }
  }

  return implode(',', $fids);
}

/**
 * #submit callback for the upload button of the field widget element.
 */
function file_resup_field_widget_submit($form, &$form_state) {
  $button = $form_state['triggering_element'];
  $element = drupal_array_get_nested_value($form, array_slice($button['#array_parents'], 0, -1));
  $field_name = $element['#field_name'];
  $langcode = $element['#language'];
  $parents = $element['#field_parents'];
  $field_state = field_form_get_state($parents, $field_name, $langcode, $form_state);
  $items = $field_state['items'];

  // Remove possible duplicate items.
  $fids = array();
  foreach ($items as $delta => $item) {
    if (in_array($item['fid'], $fids)) {
      unset($items[$delta]);
    }
    else {
      $fids[] = $item['fid'];
    }
  }
  $items = array_values($items);

  // Append our items.
  if (!empty($element['resup']['#value'])) {
    $fids = array_diff(explode(',', $element['resup']['#value']), $fids);
    $field = field_widget_field($element, $form_state);
    foreach ($fids as $fid) {
      $item = array('fid' => $fid);
      // Respect display default when available.
      if (!empty($field['settings']['display_field'])) {
        $item['display'] = $field['settings']['display_default'];
      }
      $items[] = $item;
    }
  }

  drupal_array_set_nested_value($form_state['values'], array_slice($button['#array_parents'], 0, -2), $items);
  $field_state['items'] = $items;
  field_form_set_state($parents, $field_name, $langcode, $form_state, $field_state);
}

/**
 * #pre_render callback for the field widget element.
 */
function file_resup_field_widget_pre_render($element) {
  if (!empty($element['#value']['fid'])) {
    $element['resup']['#access'] = FALSE;
  }
  return $element;
}

/**
 * Check whether our module has support for a widget type.
 */
function file_resup_field_widget_support($widget_type) {
  $supported_types = &drupal_static(__FUNCTION__);

  if (!isset($supported_types)) {
    $supported_types = module_invoke_all('file_resup_supported_field_widget_types');
    $supported_types = array_combine($supported_types, $supported_types);
    drupal_alter('file_resup_supported_field_widget_types', $supported_types);
  }

  return !empty($supported_types[$widget_type]);
}

/**
 * Implements hook_file_resup_supported_field_widget_types().
 */
function file_resup_file_resup_supported_field_widget_types() {
  return array('file_generic', 'image_image');
}

/**
 * Get the file size limit for a field instance.
 */
function file_resup_max_filesize($instance) {
  $max_filesize = file_upload_max_size();

  if (!empty($instance['settings']['max_filesize'])) {
    $size = parse_size($instance['settings']['max_filesize']);
    if ($size < $max_filesize) {
      $max_filesize = $size;
    }
  }

  if (!empty($instance['settings']['resup_max_filesize'])) {
    $size = parse_size($instance['settings']['resup_max_filesize']);
    if ($size > $max_filesize) {
      $max_filesize = $size;
    }
  }

  return $max_filesize;
}
