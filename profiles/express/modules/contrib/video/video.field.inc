<?php
/**
 * @file
 * Implement a video field, based on the file module's file field.
 */

/**
 * Implements hook_field_info().
 */
function video_field_info() {
  return array(
    'video' => array(
      'label' => t('Video'),
      'description' => t('This field stores the ID of a video file as an integer value.'),
      'settings' => array(
        'uri_scheme' => variable_get('file_default_scheme', 'public'),
        'uri_scheme_converted' => variable_get('file_default_scheme', 'public'),
        'uri_scheme_thumbnails' => variable_get('file_default_scheme', 'public'),
        'thumbnail_format' => 'jpg',
        'autoconversion' => 1,
        'autothumbnail' => 'auto',
        'default_video_thumbnail' => 0,
        'preview_video_thumb_style' => 'thumbnail',
        'presets' => array(),
      ),
      'instance_settings' => array(
        'file_extensions' => 'mp4 ogg avi mov wmv flv ogv webm',
        'file_directory' => 'videos/original',
        'max_filesize' => '',
        'default_dimensions' => '640x360',
      ),
      'default_widget' => 'video_upload',
      'default_formatter' => 'video_formatter_player',
    ),
  );
}

/**
 * Implements hook_field_settings_form().
 */
function video_field_settings_form($field, $instance, $has_data) {
  $transcoder = new Transcoder();
  $hastranscoder = $transcoder->hasTranscoder();

  $defaults = field_info_field_settings($field['type']);
  $settings = array_merge($defaults, $field['settings']);

  // Copied from file_field_settings_form().
  $scheme_options = array();
  foreach (file_get_stream_wrappers(STREAM_WRAPPERS_WRITE_VISIBLE) as $scheme => $stream_wrapper) {
    $scheme_options[$scheme] = $stream_wrapper['name'];
  }
  $form['uri_scheme'] = array(
    '#type' => 'radios',
    '#title' => t('Upload destination of original file'),
    '#options' => $scheme_options,
    '#default_value' => $settings['uri_scheme'],
    '#description' => t('Select where the original video files should be stored. Private file storage has significantly more overhead than public files, but allows restricted access to files within this field.'),
  );
  $form['uri_scheme_converted'] = array(
    '#type' => 'radios',
    '#title' => t('Upload destination of converted files'),
    '#options' => $scheme_options,
    '#default_value' => $settings['uri_scheme_converted'],
    '#description' => t('Select where the converted video files should be stored. Private file storage has significantly more overhead than public files, but allows restricted access to files within this field.'),
    '#access' => $hastranscoder,
  );
  $form['uri_scheme_thumbnails'] = array(
    '#type' => 'radios',
    '#title' => t('Upload destination of thumbnails'),
    '#options' => $scheme_options,
    '#default_value' => $settings['uri_scheme_thumbnails'],
    '#description' => t('Select where the generated thumbnails should be stored. Private file storage has significantly more overhead than public files, but allows restricted access to files within this field.'),
    '#access' => $hastranscoder,
  );

  // Add warnings for S3
  if ($hastranscoder && isset($scheme_options['s3'])) {
    $value = $transcoder->getTranscoder()->getValue();

    if ($value == 'TranscoderAbstractionFactoryFfmpeg') {
      $form['uri_scheme']['#description'] .= '<br/>';
      $form['uri_scheme']['#description'] .= t('You are using the FFmpeg transcoder. Using @scheme to store videos is not advised because FFmpeg is not able to transcode remote files. Every time FFmpeg needs access to the video, it will be copied to a temporary location.', array('@scheme' => $scheme_options['s3']));
    }

    $form['uri_scheme_thumbnails']['#description'] .= '<br/>';
    $form['uri_scheme_thumbnails']['#description'] .= t('Be aware that using @scheme to store thumbnails increases the overhead of dynamic image manipulation by the Image module.', array('@scheme' => $scheme_options['s3']));
  }

  // Add warnings for streaming to iPad when using the private file system
  // See http://www.metaltoad.com/blog/iphone-video-streaming-drupals-file-system
  if (isset($scheme_options['private']) && !module_exists('xsendfile') && !module_exists('resumable_download')) {
    $ioswarning = '<br/>' . t('Streaming to Apple iOS devices (iPad/iPhone/iPod) is not supported when using the private file system unless a module to support Range requests is installed. Modules that are known to work are <a href="@xsendfile-module">X-Sendfile</a> or <a href="@resumable-download-module">Resumable Download</a>.', array('@xsendfile-module' => url('http://drupal.org/project/xsendfile'), '@resumable-download-module' => url('http://drupal.org/project/resumable_download')));
    $form['uri_scheme']['#description'] .= $ioswarning;
    $form['uri_scheme_converted']['#description'] .= $ioswarning;
  }

  $form['autoconversion'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable auto video conversion'),
    '#description' => t('Convert videos automatically using FFmpeg or Zencoder. You can define presets at !preset to automatically convert videos to web compatible formats eg. FLV, MP4. Make sure to configure your !settings to make this work properly.', array(
      '!settings' => l(t('transcoder settings'), 'admin/config/media/video/transcoders'),
      '!preset' => l(t('preset settings'), 'admin/config/media/video/presets'),
    )),
    '#default_value' => isset($settings['autoconversion']) ? $settings['autoconversion'] : '',
    '#access' => $hastranscoder,
  );

  $form['thumbnail_format'] = array(
    '#title' => t('Thumbnail format'),
    '#type' => 'radios',
    '#options' => array('jpg' => 'JPEG', 'png' => 'PNG'),
    '#default_value' => !empty($settings['thumbnail_format']) ? $settings['thumbnail_format'] : 'jpg',
    '#access' => $hastranscoder,
  );

  $thumb_options = array(
    'auto' => 'Automatically extract thumbnails from video (with fallback to manual upload)',
    'manual_upload' => 'Manually upload a thumbnail',
    'no' => 'Don\'t create thumbnail',
  );

  // When there is no transcoder, the auto option is not available and should not be the default.
  if (!$hastranscoder) {
    unset($thumb_options['auto']);
    if (!isset($settings['autothumbnail']) || $settings['autothumbnail'] == 'auto') {
      $settings['autothumbnail'] = 'no';
    }
  }

  $form['autothumbnail'] = array(
    '#type' => 'radios',
    '#title' => t('Video thumbnails'),
    '#options' => $thumb_options,
    '#description' => t('If you choose <i>Automatically extract thumbnails from video</i> then please make sure to configure your !settings to make this work properly.', array('!settings' => l(t('transcoder settings'), 'admin/config/media/video/transcoders'))),
    '#default_value' => isset($settings['autothumbnail']) ? $settings['autothumbnail'] : 'auto',
  );

  $form['default_video_thumbnail'] = array(
    '#title' => t('Default video thumbnail'),
    '#type' => 'managed_file',
    '#element_validate' => array('video_field_default_thumbnail_validate'),
    '#description' => t('You can use a default thumbnail for all videos or videos from which a thumbnail can\'t be extracted. Settings to use default video thumbnails will be available on node edit. You can change the permissions for other users too.'),
    '#default_value' => !empty($settings['default_video_thumbnail']['fid']) ? $settings['default_video_thumbnail']['fid'] : '',
    '#upload_location' => 'public://videos/thumbnails/default',
  );

  $form['preview_video_thumb_style'] = array(
    '#title' => t('Preview thumbnail style'),
    '#type' => 'select',
    '#options' => image_style_options(FALSE),
    '#empty_option' => '<' . t('no preview') . '>',
    '#default_value' => !empty($settings['preview_video_thumb_style']) ? $settings['preview_video_thumb_style'] : '',
    '#description' => t('This image style will be used to show extracted video thumbnails on video node edit. Extracted thumbnail preview will also use this style.'),
  );

  $selectedpresets = array_filter(variable_get('video_preset', array()));
  $presets = Preset::getAllPresets();
  $presetnames = array();
  foreach ($presets as $preset) {
    $presetnames[$preset['name']] = $preset['name'];

    if (in_array($preset['name'], $selectedpresets)) {
      $presetnames[$preset['name']] .= ' (' . t('default') . ')';
    }
  }

  $form['presets'] = array(
    '#title' => t('Presets'),
    '#type' => 'checkboxes',
    '#options' => $presetnames,
    '#default_value' => !empty($settings['presets']) ? $settings['presets'] : array(),
    '#description' => t('If any presets are selected, these presets will be used for this field instead of the default presets.'),
    '#access' => $hastranscoder,
  );

  return $form;
}

/**
 * Element specific validation for video default value.
 */
function video_field_default_thumbnail_validate($element, &$form_state) {
  $settings = $form_state['values']['field']['settings'];
  // Make the file permanent and store it in the form.
  if (!empty($settings['default_video_thumbnail']['fid'])) {
    $file = file_load($settings['default_video_thumbnail']['fid']);
    $file->status = FILE_STATUS_PERMANENT;
    $file = file_save($file);
    $form_state['values']['field']['settings']['default_video_thumbnail'] = (array) $file;
  }
}

/**
 * Implements hook_field_instance_settings_form().
 */
function video_field_instance_settings_form(array $field, array $instance) {
  $widget = $instance['settings'];

  // Use the file field instance settings form as a basis.
  $form = file_field_instance_settings_form($field, $instance);

  // Remove the description option.
  unset($form['description_field']);

  $form['default_dimensions'] = array(
    '#type' => 'select',
    '#title' => t('Output video dimensions'),
    '#default_value' => !empty($widget['default_dimensions']) ? $widget['default_dimensions'] : '',
    '#options' => video_utility::getDimensions(),
    '#description' => t('This setting can be overridden in node edit or preset settings. Select the most suitable values to use as output dimensions, as selecting a bad value could cause video conversion to fail.'),
    '#weight' => 16,
  );

  return $form;
}

/**
 * Implements hook_field_load().
 */
function video_field_load($entity_type, $entities, $field, $instances, $langcode, &$items, $age) {
  file_field_load($entity_type, $entities, $field, $instances, $langcode, $items, $age);

  // Load all files we need for these entities
  $videofids = array();
  $fids = array();
  foreach ($entities as $id => $entity) {
    // Load the files from the files table.
    foreach ($items[$id] as $delta => $item) {
      if (empty($item) || empty($item['fid'])) {
        continue;
      }
      $videofids[] = $item['fid'];
      if (!empty($item['thumbnail'])) {
        $fids[] = $item['thumbnail'];
      }
    }
  }

  if (empty($videofids)) {
    return;
  }

  // Load the queues
  $queues = db_select('video_queue', 'q')
    ->fields('q')
    ->condition('q.fid', $videofids, 'IN')->execute()->fetchAllAssoc('fid');

  // Load the derived files
  $outputs = db_select('video_output', 'vo')
    ->fields('vo')
    ->condition('vo.original_fid', $videofids, 'IN')
    ->execute()->fetchAllAssoc('output_fid');

  $fids = array_merge($fids, array_keys($outputs));
  $files = file_load_multiple($fids);

  // Apply the found information to all files for all entities
  foreach ($entities as $id => $entity) {
    foreach ($items[$id] as $delta => $item) {
      if (empty($item) || empty($item['fid'])) {
        continue;
      }
      // Check whether transcoding is enabled for this file
      if (isset($queues[$item['fid']])) {
        $items[$id][$delta]['autoconversion'] = TRUE;
        $items[$id][$delta]['conversioncompleted'] = $queues[$item['fid']]->status == VIDEO_RENDERING_COMPLETE;
        $items[$id][$delta]['conversionstatus'] = $queues[$item['fid']]->status;

        // Load converted files
        $items[$id][$delta]['playablefiles'] = array();
        foreach ($outputs as $outputfid => $output) {
          if ($output->original_fid == $items[$id][$delta]['fid'] && isset($files[$outputfid]) && $files[$outputfid]->filesize > 0) {
            $conv = (array)$files[$outputfid] + (array)$output;
            $items[$id][$delta]['playablefiles'][] = (object)$conv;
          }
        }

        // If for some reason there are no playable files, mark the status as failed
        if (empty($items[$id][$delta]['playablefiles']) && $items[$id][$delta]['conversioncompleted']) {
          $items[$id][$delta]['conversioncompleted'] = FALSE;
          $items[$id][$delta]['conversionstatus'] = VIDEO_RENDERING_FAILED;
        }
      }
      else {
        $items[$id][$delta]['playablefiles'] = array((object)$items[$id][$delta]);
        $items[$id][$delta]['autoconversion'] = FALSE;
      }

      // Load thumbnail
      if (empty($item['thumbnail']) || !isset($files[$item['thumbnail']])) {
        $items[$id][$delta]['thumbnail'] = NULL;
        $items[$id][$delta]['thumbnailfile'] = NULL;
      }
      else {
        $items[$id][$delta]['thumbnailfile'] = $files[$item['thumbnail']];
      }
    }
  }
}

/**
 * Implements hook_field_validate().
 *
 * This allows field validation even when the file is added programmatically to
 * the entity. The function only validates new files, not files that have been
 * added to the entity before.
 */
function video_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  $current_fids = array();

  // Get the video fids
  foreach ($items as $delta => $item) {
    if (empty($item['fid'])) {
      continue;
    }
    $current_fids[$item['fid']] = $item['fid'];
  }

  if (empty($current_fids)) {
    return;
  }

  // Create a bare-bones entity so that we can load its previous values.
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
  $original = entity_create_stub_entity($entity_type, array($id, $vid, $bundle));
  field_attach_load($entity_type, array($id => $original), FIELD_LOAD_CURRENT, array('field_id' => $field['id']));

  // Remove the entries from $current_fids that are already present in the database,
  // leaving the ones that are new during this save operation.
  if (!empty($original->{$field['field_name']}[$langcode])) {
    foreach ($original->{$field['field_name']}[$langcode] as $originalitem) {
      if (isset($originalitem['fid']) && isset($current_fids[$originalitem['fid']])) {
        unset($current_fids[$originalitem['fid']]);
      }
    }
  }

  if (empty($current_fids)) {
    // No new files have been added, only new files are checked
    return;
  }

  $current_files = file_load_multiple($current_fids);
  $validators = file_field_widget_upload_validators($field, $instance);
  if (empty($validators)) {
    // No validation rules
    return;
  }

  // Validate the new entries
  foreach ($items as $delta => $item) {
    if (empty($item['fid']) || !isset($current_files[$item['fid']])) {
      continue;
    }

    $fileerrors = file_validate($current_files[$item['fid']], $validators);

    foreach ($fileerrors as $fileerror) {
      $errors[$field['field_name']][$langcode][$delta][] = array(
        'error' => 'video_upload',
        'message' => $fileerror,
      );
    }
  }
}

/**
 * Implements hook_field_widget_error().
 *
 * Work-around for Drupal bug http://drupal.org/node/1432732
 */
function video_field_widget_error($element, $error, $form, &$form_state) {
  form_error($element, $error['message']);
}

/**
 * Implements hook_field_presave().
 */
function video_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // change the thumbnails if default is checked
  foreach ($items as $delta => $item) {
    if (!empty($field['settings']['default_video_thumbnail']['fid'])) {
      if (!empty($item['use_default_video_thumb'])) {
        $items[$delta]['thumbnail'] = $field['settings']['default_video_thumbnail']['fid'];
      }
    }
  }

  file_field_presave($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_insert().
 */
function video_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  file_field_insert($entity_type, $entity, $field, $instance, $langcode, $items);
  // calling function to handle conversion when auto conversion is enabled
  _video_field_file_autoconversion($entity_type, $entity, $field, $instance, $langcode, $items);

  // Update the thumbnails to be permanent and register with entity
  $thumbnails = _video_field_get_all_thumbnails($field, $items);
  if (!empty($thumbnails)) {
    db_update('file_managed')
      ->fields(array('status' => FILE_STATUS_PERMANENT))
      ->condition('fid', array_keys($thumbnails), 'IN')->execute();
    file_field_insert($entity_type, $entity, $field, $instance, $langcode, $thumbnails);
  }

  // Update the thumbnails to be permanent and register with entity
  $converted = _video_field_get_all_converted($items);
  if (!empty($converted)) {
    file_field_insert($entity_type, $entity, $field, $instance, $langcode, $converted);
  }
}

/**
 * Implements hook_field_update().
 *
 * @todo handle new revisions
 */
function video_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // The derived files are handled first, because when the original file is deleted first,
  // the derived files can not be looked up anymore.

  // Modification of file_field_update().
  // That function can't be called because the logic for finding
  // existing fids is not the same.
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  // Register the thumbnails in file_usage
  $thumbnails = _video_field_get_all_thumbnails($field, $items);
  if (!empty($thumbnails)) {
    db_update('file_managed')
      ->fields(array('status' => FILE_STATUS_PERMANENT))
      ->condition('fid', array_keys($thumbnails), 'IN')->execute();
  }

  // Register the thumbnails in file_usage
  $converted = _video_field_get_all_converted($items);
  $derivedfiles = array_merge($converted, $thumbnails);

  // Build a display of the current FIDs.
  $current_fids = array();
  foreach ($derivedfiles as $item) {
    $current_fids[] = $item['fid'];
  }

  // Create a bare-bones entity so that we can load its previous values.
  $original = entity_create_stub_entity($entity_type, array($id, $vid, $bundle));
  field_attach_load($entity_type, array($id => $original), FIELD_LOAD_CURRENT, array('field_id' => $field['id']));

  // Compare the original field values with the ones that are being saved.
  $original_fids = array();
  if (!empty($original->{$field['field_name']}[$langcode])) {
    $original_thumbs = _video_field_get_all_thumbnails($field, $original->{$field['field_name']}[$langcode], $entity_type, $id);
    $original_converted = _video_field_get_all_converted($original->{$field['field_name']}[$langcode], $entity_type, $id);
    $original_derivedfiles = array_merge($original_thumbs, $original_converted);
    foreach ($original_derivedfiles as $original_derivedfile) {
      $original_fids[] = $original_derivedfile['fid'];
      if (isset($original_derivedfile['fid']) && !in_array($original_derivedfile['fid'], $current_fids)) {
        // Decrement the file usage count by 1 and delete the file if possible.
        file_field_delete_file($original_derivedfile, $field, $entity_type, $id);
      }
    }
  }

  // Add new usage entries for newly added files.
  foreach ($derivedfiles as $item) {
    if (!in_array($item['fid'], $original_fids)) {
      $file = (object) $item;
      file_usage_add($file, 'file', $entity_type, $id);
    }
  }

  // Process the original file
  file_field_update($entity_type, $entity, $field, $instance, $langcode, $items);
  _video_field_file_autoconversion($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_delete().
 */
function video_field_delete($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // Deregister the thumbnails in file_usage
  $thumbnails = _video_field_get_all_thumbnails($field, $items);
  file_field_delete($entity_type, $entity, $field, $instance, $langcode, $thumbnails);

  // Deregister the converted files in file_usage
  $converted = _video_field_get_all_converted($items);
  file_field_delete($entity_type, $entity, $field, $instance, $langcode, $converted);

  file_field_delete($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_delete_revision().
 */
function video_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, &$items) {
  // Deregister the thumbnails in file_usage
  $thumbnails = _video_field_get_all_thumbnails($field, $items);
  file_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, $thumbnails);

  // Deregister the converted files in file_usage
  $converted = _video_field_get_all_converted($items);
  file_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, $converted);

  file_field_delete_revision($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implements hook_field_is_empty().
 */
function video_field_is_empty($item, $field) {
  return file_field_is_empty($item, $field);
}

/**
 * Implements hook_entity_insert().
 *
 * Processes newly uploaded files that should be converted on save.
 */
function video_entity_insert($entity, $type) {
  _video_field_convert_on_save($entity, $type);
}

/**
 * Implements hook_entity_updated().
 *
 * Processes newly uploaded files that should be converted on save.
 */
function video_entity_update($entity, $type) {
  _video_field_convert_on_save($entity, $type);
}

/**
 * Implements hook_entity_translation_save().
 *
 * Process saving a "Entity Translation" save.
 */
function video_entity_translation_save($type, $entity, $language) {
  _video_field_convert_on_save($entity, $type);
}

/**
 * Widget
 */

/**
 * Implements hook_field_widget_info().
 */
function video_field_widget_info() {
  return array(
    'video_upload' => array(
      'label' => t('Video Upload'),
      'field types' => array('video'),
      'settings' => array(
        'progress_indicator' => 'throbber',
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_settings_form().
 */
function video_field_widget_settings_form($field, $instance) {
  // Use the file widget settings form.
  return file_field_widget_settings_form($field, $instance);
}

/**
 * Implements hook_field_widget_form().
 */
function video_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  // Add display_field setting to field because file_field_widget_form() assumes it is set.
  $field['settings']['display_field'] = 0;

  if (module_exists('filefield_role_limit')) {
    filefield_role_limit_file_field_process($element, $form_state, $form);
  }
  $elements = file_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  $settings = $instance['settings'];

  foreach (element_children($elements) as $delta) {
    // If not using custom extension validation, ensure this is an video.
    $supported_extensions = array_keys(video_utility::getVideoExtensions());
    $extensions = isset($elements[$delta]['#upload_validators']['file_validate_extensions'][0]) ? $elements[$delta]['#upload_validators']['file_validate_extensions'][0] : implode(' ', $supported_extensions);
    $extensions = array_intersect(explode(' ', $extensions), $supported_extensions);
    $elements[$delta]['#upload_validators']['file_validate_extensions'][0] = implode(' ', $extensions);

    // Add all extra functionality provided by the video widget.
    $elements[$delta]['#process'][] = 'video_field_widget_process';

    // Add thumbnail stub to prevent errors in file_ajax_upload().
    $elements[$delta]['thumbnail'] = array(
      '#type' => 'value',
      '#value' => NULL,
    );

    // Override value loader
    $elements[$delta]['#value_callback'] = 'video_field_widget_value';
  }

  if ($field['cardinality'] == 1) {
    // If there's only one field, return it as delta 0.
    if (empty($elements[0]['#default_value']['fid'])) {
      $elements[0]['#description'] = theme('file_upload_help', array('description' => $instance['description'], 'upload_validators' => $elements[0]['#upload_validators']));
    }
  }
  else {
    $elements['#file_upload_description'] = theme('file_upload_help', array('upload_validators' => $elements[0]['#upload_validators']));
  }
  return $elements;
}

/**
 * The #value_callback for the video field element.
 *
 * This function loads additional information related to the video to be used in
 * video_field_widget_process().
 */
function video_field_widget_value($element, $input = FALSE, $form_state) {
  $value = file_field_widget_value($element, $input, $form_state);

  // Extra default values.
  $alldimensions = video_utility::getDimensions();
  $value += array(
    'bypass_autoconversion' => variable_get('video_bypass_conversion', FALSE),
    'convert_video_on_save' => variable_get('video_convert_on_save', FALSE),
    'use_default_video_thumb' => variable_get('video_use_default_thumb', FALSE),
    'dimensions' => key($alldimensions),
    'conversionstatus' => NULL,
    'thumbnail' => 0,
  );

  // Load the default dimensions.
  $instance = field_widget_instance($element, $form_state);
  if (in_array($instance['settings']['default_dimensions'], $alldimensions)) {
    $value['dimensions'] = $instance['settings']['default_dimensions'];
  }

  if ($value['fid'] > 0) {
    $transcoder = new Transcoder();
    if ($transcoder->hasTranscoder()) {
      $video = video_jobs::load($value['fid']);

      if ($video) {
        $value['dimensions'] = $video->dimensions;
        $value['conversionstatus'] = $video->video_status;
        // When there is a job, unselect "Bypass auto conversion" because it
        // would remove the converted files.
        $value['bypass_autoconversion'] = 0;
      }
      else {
        // No job (yet), find the video dimensions to use as default.
        $video_ratio = _video_aspect_ratio($value);

        if (!empty($video_ratio['width']) && !empty($video_ratio['height'])) {
          $filedimensions = $video_ratio['width'] . 'x' . $video_ratio['height'];
          if (in_array($filedimensions, $alldimensions)) {
            $value['dimensions'] = $filedimensions;
          }
        }

        // If there is no job, it might be because the user selected
        // "bypass autoconversion" previously. When the file is permanent, 
        // this probably was the case.
        if (isset($value['status']) && $value['status'] == FILE_STATUS_PERMANENT) {
          $value['bypass_autoconversion'] = 1;
        }
      }
    }
  }

  // Find the values for thumbnail and use_default_video_thumb.
  if (!empty($value['thumbnailfile']) && !empty($value['thumbnailfile']->fid)) {
    $value['thumbnail'] = $value['thumbnailfile']->fid;
    $field = field_widget_field($element, $form_state);
    if (!empty($field['settings']['default_video_thumbnail']['fid'])) {
      if ($field['settings']['default_video_thumbnail']['fid'] == $value['thumbnail']) {
        $value['use_default_video_thumb'] = 1;
      }
    }
  }

  return $value;
}

/**
 * An element #process callback for the video field type.
 */
function video_field_widget_process($element, &$form_state, $form) {
  $file = $element['#value'];
  $file['fid'] = $fid = intval($element['fid']['#value']);
  $field = field_widget_field($element, $form_state);

  $element['#theme'] = 'video_widget';

  // Title is not necessary for each individual field.
  if ($field['cardinality'] != 1) {
    unset($element['#title']);
  }

  // Add our extra fields if in preview mode
  if (empty($file['fid'])) {
    return $element;
  }

  $transcoder = new Transcoder();

  // Various settings.
  if (!empty($field['settings']['autoconversion']) && $transcoder->hasTranscoder()) {
    $description = t('Set the size of the converted video.');
    $options = video_utility::getDimensions();
    $video_info = _video_dimensions_options($options, $file);
    if (!empty($video_info['width']) && !empty($video_info['height'])) {
      $description .= ' ' . t('The original video size is %size. If you choose a higher resolution, this could cause video distortion. You are shown dimensions that match your aspect ratio, if you choose dimensions that do not match your ratio, black bars will be added to maintain the original aspect ratio.', array('%size' => $video_info['width'] . 'x' . $video_info['height']));
    }

    // Dimensions dropdown.
    $element['dimensions'] = array(
      '#type' => 'select',
      '#title' => t('Output video dimensions'),
      '#default_value' => $file['dimensions'],
      '#description' => $description,
      '#options' => $options,
      '#access' => user_access('override player dimensions'),
    );

    if (!empty($file['conversionstatus']) && ($file['conversionstatus'] == VIDEO_RENDERING_COMPLETE || $file['conversionstatus'] == VIDEO_RENDERING_FAILED)) {
      $status = array(VIDEO_RENDERING_COMPLETE => 'was successful', VIDEO_RENDERING_FAILED => 'failed');
      $element['re_convert_video'] = array(
        '#type' => 'checkbox',
        '#title' => t('Video conversion ' . $status[$file['conversionstatus']] . '. Re-queue video conversion?'),
        '#description' => t('This will re-convert your video and schedule it for cron, unless you also check “convert video on save,” below.'),
        '#attributes' => array('class' => array('video-re-convert', 'video-' . $file['conversionstatus'])),
        '#access' => user_access('re convert video'),
      );
    }

    // Bypass conversion checkbox.
    $element['bypass_autoconversion'] = array(
      '#type' => 'checkbox',
      '#title' => t('Bypass auto conversion'),
      '#default_value' => $file['bypass_autoconversion'],
      '#description' => t('This video will not convert your video when you save, and it will not be scheduled for cron.'),
      '#attributes' => array('class' => array('video-bypass-auto-conversion')),
      '#access' => user_access('bypass conversion video'),
    );

    // Convert on save checkbox.
    $element['convert_video_on_save'] = array(
      '#type' => 'checkbox',
      '#title' => t('Convert video on save'),
      '#default_value' => $file['convert_video_on_save'],
      '#description' => t('This will convert your video on save, instead of scheduling it for cron.'),
      '#attributes' => array('class' => array('video-convert-video-on-save')),
      '#access' => user_access('convert on submission'),
    );
  }

  // Use of default thumbnail checkbox.
  if (!empty($field['settings']['default_video_thumbnail']['fid'])) {
    $element['use_default_video_thumb'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use the default thumbnail for this video'),
      '#default_value' => $file['use_default_video_thumb'],
      '#description' => t('This will set a flag for this video to use the default video thumbnail when output.'),
      '#attributes' => array(
        'class' => array('video-use-default-video-thumb'),
        'data-defaultimage' => image_style_url($field['settings']['preview_video_thumb_style'], $field['settings']['default_video_thumbnail']['uri']),
      ),
      '#access' => user_access('use default thumb'),
    );
  }

  // Thumbnails.
  $defaultthumbfid = isset($field['settings']['default_video_thumbnail']) ? intval($field['settings']['default_video_thumbnail']['fid']) : 0;
  $thumbnailfid = is_array($file['thumbnail']) ? intval($file['thumbnail']['fid']) : intval($file['thumbnail']);

  $gen_fail = FALSE;

  if ($field['settings']['autothumbnail'] == 'auto' && $transcoder->hasTranscoder()) {
    $thumbs = $transcoder->extractFrames($file, $field);
    $thumbstyle = !empty($field['settings']['preview_video_thumb_style']) ? $field['settings']['preview_video_thumb_style'] : 'thumbnail';

    if (!empty($thumbs)) {
      $thumbss = array();
      foreach ($thumbs as $img) {
        $thumbss[$img->fid] = theme('image_style', array('style_name' => $thumbstyle, 'path' => $img->uri));
      }

      $currentthumb = 0;
      if ($thumbnailfid > 0 && isset($thumbss[$thumbnailfid])) {
        $currentthumb = $thumbnailfid;
      }
      elseif ($thumbnailfid != $defaultthumbfid) {
        $currentthumb = array_rand($thumbss);
      }

      $element['thumbnail'] = array(
        '#type' => 'radios',
        '#title' => t('Video thumbnail'),
        '#options' => $thumbss,
        '#default_value' => $currentthumb,
        '#weight' => 10,
        '#attributes' => array('class' => array('video-thumbnails')),
      );
    }
    elseif ($thumbs === FALSE) {
      $gen_fail = TRUE;
    }
  }

  if ($gen_fail || $field['settings']['autothumbnail'] == 'manual_upload') {
    $scheme = isset($field['settings']['uri_scheme_thumbnails']) ? $field['settings']['uri_scheme_thumbnails'] : 'public';
    $element['thumbnail'] = array(
      '#title' => t('Video thumbnail'),
      '#type' => 'managed_file',
      '#description' => t('The uploaded image will be used as video thumbnail on this video.'),
      '#default_value' => NULL,
      '#upload_location' => $scheme . '://' . variable_get('video_thumbnail_path', 'videos/thumbnails') . '/' . $fid,
      '#weight' => 10,
    );

    // Set the current thumbnail fid, if it is not the default one
    if ($thumbnailfid > 0) {
      if ($defaultthumbfid == 0 || $thumbnailfid != $defaultthumbfid) {
        $element['thumbnail']['#default_value'] = $thumbnailfid;
      }
      else {
        // Unset the form_state value for the thumbnail field if it is the default thumbnail.
        // This prevents the "may not be referenced" error.
        form_set_value($element, NULL, $form_state);
      }
    }
  }

  // Setup our large thumbnail that is on the left.
  if (!empty($field['settings']['preview_video_thumb_style'])) {
    $large_thumb = NULL;
    if (!empty($currentthumb)) {
      $large_thumb = file_load($currentthumb);
    }
    elseif ($thumbnailfid > 0) {
      $large_thumb = file_load($thumbnailfid);
    }
    elseif ($defaultthumbfid > 0) {
      $large_thumb = file_load($defaultthumbfid);
    }

    if (!empty($large_thumb)) {
      $element['preview']['#markup'] = '<div class="video-preview video_large_thumbnail-' . $fid . '">' . theme('image_style', array('style_name' => $field['settings']['preview_video_thumb_style'], 'path' => $large_thumb->uri)) . '</div>';
    }
  }

  return $element;
}

/**
 * Updates options list to show matching aspect ratios and matching resolutions
 *
 * We will update the options array by reference and return the aspect ratio of
 * the file.
 */
function _video_dimensions_options(&$options, $video) {
  $aspect_ratio = _video_aspect_ratio($video);

  if (empty($aspect_ratio)) {
    return $aspect_ratio;
  }

  // Loop through our options and find matching ratio's and also the exact width/height
  foreach ($options as $key => $value) {
    $wxh = explode('x', $value);
    // Lets check our width and height first
    if ($aspect_ratio['width'] == $wxh[0] && $aspect_ratio['height'] == $wxh[1]) {
      $options[$key] = $value . ' (' . t('Matches resolution of original file') . ')';
    }
    else {
      // Now lets check our ratio's
      $ratio = number_format($wxh[0] / $wxh[1], 4);
      if ($ratio == $aspect_ratio['ratio']) {
        $options[$key] = $value . ' (' . t('Matches ratio of original file') . ')';
      }
    }
  }

  return $aspect_ratio;
}

/**
 * Returns the width/height and aspect ratio of the video
 */
function _video_aspect_ratio($video) {
  $transcoder = new Transcoder();
  $transcoder = $transcoder->getTranscoder();
  $transcoder->setInput((array) $video);
  $wxh = $transcoder->getDimensions();

  if (empty($wxh) || empty($wxh['width']) || empty($wxh['height'])) {
    // No width and height found. This may be because the transcoder does not support retrieving dimensions.
    return NULL;
  }

  return array(
    'width' => $wxh['width'],
    'height' => $wxh['height'],
    'ratio' => number_format($wxh['width'] / $wxh['height'], 4),
  );
}

/**
 * Formatters
 */

/**
 * Implements hook_field_formatter_info().
 */
function video_field_formatter_info() {
  return array(
    'video_formatter_player' => array(
      'label' => t('Video player'),
      'field types' => array('video'),
      'settings' => array(
        'widthxheight' => '640x360',
        'poster_image_style' => '',
      ),
    ),
    'video_formatter_thumbnail' => array(
      'label' => t('Video thumbnail'),
      'field types' => array('video'),
      'settings' => array(
        'image_style' => '',
        'image_link' => '',
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function video_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $image_styles = image_style_options(FALSE);

  switch ($display['type']) {
    case 'video_formatter_thumbnail':
      $element['image_style'] = array(
        '#title' => t('Video thumbnail style'),
        '#type' => 'select',
        '#default_value' => $settings['image_style'],
        '#empty_option' => t('None (original video thumbnail)'),
        '#options' => $image_styles,
      );

      $element['image_link'] = array(
        '#title' => t('Link video or video thumbnail to'),
        '#type' => 'select',
        '#default_value' => $settings['image_link'],
        '#empty_option' => t('Nothing'),
        '#options' => array(
          'content' => t('Content'),
          'file' => t('File'),
        ),
      );
      break;

    case 'video_formatter_player':
      $element['widthxheight'] = array(
        '#title' => t('Dimensions'),
        '#type' => 'select',
        '#default_value' => $settings['widthxheight'],
        '#description' => t('Select the desired dimensions of the video player. You can add your own dimensions at !settings.', array('!settings' => l(t('video module settings'), 'admin/config/media/video'))),
        '#options' => video_utility::getDimensions(),
      );

      $element['poster_image_style'] = array(
        '#title' => t('Poster image style'),
        '#type' => 'select',
        '#default_value' => $settings['poster_image_style'],
        '#empty_option' => t('None (original image)'),
        '#description' => t('The original video thumbnail will be displayed. Otherwise, you can add a custom image style at !settings.', array('!settings' => l(t('media image styles'), 'admin/config/media/image-styles'))),
        '#options' => $image_styles,
      );
      break;
  }

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function video_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];
  $summary = array();
  $image_styles = image_style_options(FALSE);
  // Unset possible 'No defined styles' option.
  unset($image_styles['']);

  // Styles could be lost because of enabled/disabled modules that defines
  // their styles in code.
  switch ($display['type']) {
    case 'video_formatter_thumbnail':
      if (isset($image_styles[$settings['image_style']])) {
        $summary[] = t('Video thumbnail style: @style', array('@style' => $image_styles[$settings['image_style']]));
      }
      else {
        $summary[] = t('Original video thumbnail');
      }

      $link_types = array(
        'content' => t('Linked to content'),
        'file' => t('Linked to video file'),
      );
      // Display this setting only if image is linked.
      if (isset($link_types[$settings['image_link']])) {
        $summary[] = $link_types[$settings['image_link']];
      }
      break;

    case 'video_formatter_player':
      $summary[] = t('Player dimensions: @widthxheight', array('@widthxheight' => $settings['widthxheight']));
      if (isset($image_styles[$settings['poster_image_style']])) {
        $summary[] = t('Poster image style: @style', array('@style' => $image_styles[$settings['poster_image_style']]));
      }
      break;
  }

  return implode('<br />', $summary);
}

/**
 * Implements hook_field_formatter_view().
 */
function video_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];

  foreach ($items as $delta => $item) {
    switch ($display['type']) {
      case 'video_formatter_thumbnail':
        // Check if the formatter involves a link.
        if ($settings['image_link'] == 'content') {
          $uri = entity_uri($entity_type, $entity);
        }
        elseif ($settings['image_link'] == 'file') {
          $link_file = TRUE;
        }
        if (isset($link_file)) {
          $uri = array(
            'path' => file_create_url($item['uri']),
            'options' => array(),
          );
        }
        $element[$delta] = array(
          '#theme' => 'video_formatter_thumbnail',
          '#item' => $item,
          '#image_style' => $settings['image_style'],
          '#path' => isset($uri) ? $uri : '',
          '#entity' => $entity,
          '#entity_type' => $entity_type,
          '#field' => $field,
          '#instance' => $instance
        );
        break;

      case 'video_formatter_player':
        $element[$delta] = array(
          '#theme' => 'video_formatter_player',
          '#item' => $item,
          '#entity' => $entity,
          '#entity_type' => $entity_type,
          '#field' => $field,
          '#instance' => $instance,
          '#player_dimensions' => $settings['widthxheight'],
          '#poster_image_style' => $settings['poster_image_style'],
        );
        break;
    }
  }

  return $element;
}

/**
 * Implements hook_filefield_sources_widgets().
 */
function video_filefield_sources_widgets() {
  return array('video_upload');
}

/**
 * Implements hook_filefield_role_limit_supported_widgets_alter().
 *
 * Add the video field to the list of supported FileField Role Limit widgets.
 */
function video_filefield_role_limit_supported_widgets_alter(&$types) {
  $types[] = 'video_upload';
}

/**
 * Video file save to the video_queue table for conversions
 */
function _video_field_file_autoconversion($entity_type, $entity, $field, $instance, $langcode, &$items) {
  $transcoder = new Transcoder();

  if (empty($field['settings']['autoconversion']) || !$transcoder->hasTranscoder()) {
    return;
  }

  foreach ($items as $delta => &$item) {
    $fid = intval($item['fid']);
    // skip adding entry if bypass conversion is checked
    if (!empty($item['bypass_autoconversion'])) {
      // delete the conversion job if any
      video_jobs::delete($fid);
      return;
    }

    // Try to load the job
    $video = video_jobs::load($fid);

    // Create the job if it doesn't exist
    if (!$video) {
      list($entity_id, $entity_vid, $bundle) = entity_extract_ids($entity_type, $entity);
      // @todo get the default dimension when not available in $item
      if (!video_jobs::create($item['fid'], $item['dimensions'], $entity_id, $entity_type, $field['field_name'], $langcode, $delta)) {
        drupal_set_message(t('Something went wrong with your video job creation. Please check your recent log entries for further debugging.'), 'error');
        return;
      }
      // Load a fresh copy of the job
      $video = video_jobs::load($fid);
    }

    // re queue for video conversion
    if (!empty($item['re_convert_video'])) {
      $video->video_status = VIDEO_RENDERING_PENDING;
      $video->statusupdated = time();
      $video->dimensions = $item['dimensions'];
      video_jobs::update($video);
    }

    if ($video->video_status == VIDEO_RENDERING_PENDING) {
      // Convert on save if the job is pending and if it is requested
      // The automatic conversions of files is handled in _video_field_convert_on_save(),
      // which is called from hook_entity_insert/hook_entity_update.
      if (!empty($item['convert_video_on_save'])) {
        // Only save the fid, because the location of the file may change
        // between now and _video_field_convert_on_save().
        $entity->video_convert_on_save[$video->fid] = $video->fid;
      }
      else {
        drupal_set_message(t('Transcoding job was successfully queued for %transcoder-name. The video %video-name will be converted soon.', array('%transcoder-name' => $transcoder->getTranscoder()->getName(), '%video-name' => $video->filename)));
      }
    }
  }
}

/**
 * Processes video fields that should convert on save.
 *
 * This is not performed in _video_field_file_autoconversion() because
 * of incompatibilities with Rules and File(field) Paths.
 *
 * When hook_entity_insert/update are called, all field related operations
 * are complete and the entity is fully saved to the database. This
 * allows Rules to supply a proper entity to the event handlers.
 *
 * The $entity->video_convert_on_save property is set in
 * _video_field_file_autoconversion().
 */
function _video_field_convert_on_save($entity, $type) {
  if (!empty($entity->video_convert_on_save)) {
    $transcoder = new Transcoder();
    $transcodername = $transcoder->getTranscoder()->getName();

    foreach ($entity->video_convert_on_save as $videofid) {
      $video = video_jobs::load($videofid);
      if ($transcoder->executeConversion($video)) {
        if ($video->video_status == VIDEO_RENDERING_COMPLETE) {
          drupal_set_message(t('The video %video-name was converted.', array('%video-name' => $video->filename)));
        }
        else {
          drupal_set_message(t('Transcoding job was successfully submitted to %transcoder-name. The video %video-name will be converted soon.', array('%transcoder-name' => $transcodername, '%video-name' => $video->filename)));
        }
      }
      else {
        drupal_set_message(t('Something went wrong with transcoding %video-name. Please check your <a href="@log-page">recent log entries</a> for further debugging.', array('%video-name' => $video->filename, '@log-page' => url('admin/reports/dblog'))), 'error');
      }
    }

    unset($entity->video_convert_on_save);
  }
}

function _video_field_get_all_thumbnails(array $field, array $items, $usage_entity_type = NULL, $usage_entity_id = NULL) {
  if (empty($items)) {
    return array();
  }

  $defaultthumbnailfid = !empty($field['settings']['default_video_thumbnail']) ? $field['settings']['default_video_thumbnail']['fid'] : 0;
  $videofids = array();
  $thumbnailfids = array();
  foreach ($items as $item) {
    if (!empty($item['fid'])) {
      $videofids[] = intval($item['fid']);

      // Add the selected thumbnail if it is not the default thumbnail for the field
      if (!empty($item['thumbnail']) && $item['thumbnail'] != $defaultthumbnailfid) {
        $thumbnailfids[] = intval($item['thumbnail']);
      }
    }
  }

  if (empty($videofids)) {
    return array();
  }

  // Add the automatically extracted thumbnails
  $query = db_select('video_thumbnails', 't')
    ->fields('t', array('thumbnailfid'))
    ->condition('videofid', $videofids, 'IN');

  // Only return files with usage when requested
  if ($usage_entity_type != NULL && $usage_entity_id != NULL) {
    $query->innerJoin('file_usage', 'u', 'u.fid = t.thumbnailfid AND u.type = :type AND u.id = :id', array(
        ':type' => $usage_entity_type,
        ':id' => $usage_entity_id,
    ));
  }

  $thumbnailfids = array_merge($thumbnailfids, $query->execute()->fetchCol(0));

  return video_utility::objectToArray(file_load_multiple($thumbnailfids));
}

function _video_field_get_all_converted(array $items, $usage_entity_type = NULL, $usage_entity_id = NULL) {
  if (empty($items)) {
    return array();
  }

  $videofids = array();
  foreach ($items as $item) {
    if (!empty($item['fid'])) {
      $videofids[] = intval($item['fid']);
    }
  }

  if (empty($videofids)) {
    return array();
  }

  $query = db_select('video_output', 't')
    ->fields('t', array('output_fid'))
    ->condition('original_fid', $videofids, 'IN');

  // Only return files with usage when requested
  if ($usage_entity_type != NULL && $usage_entity_id != NULL) {
    $query->innerJoin('file_usage', 'u', 'u.fid = t.output_fid AND u.type = :type AND u.id = :id', array(
      ':type' => $usage_entity_type,
      ':id' => $usage_entity_id,
    ));
  }

  $convertedfids = $query->execute()->fetchCol(0);

  return video_utility::objectToArray(file_load_multiple($convertedfids));
}
