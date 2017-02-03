<?php
/**
 * @file
 * Helper functions for the Manual Crop module.
 */

/**
 * Returns an array of supported widget types or checks if a type is supported.
 *
 * @param $widget_type
 *   If set, this function will return a boolean indicating if $widget_type
 *   is supported.
 * @param $settings
 *   Only include widgets that support these setting(s).
 *
 * @return
 *   Array of widget types.
 */
function manualcrop_supported_widgets($widget_type = NULL, $settings = array()) {
  $widgets = &drupal_static(__FUNCTION__);

  if (!isset($widgets)) {
    // Collect information about the supported widgets.
    $widgets = module_invoke_all('manualcrop_supported_widgets');
    drupal_alter('manualcrop_supported_widgets', $widgets);
  }

  // Make sure $settings contains only valid entries.
  if (!empty($settings)) {
    if (!is_array($settings)) {
      $settings = array($settings);
    }

    $widget_settings = manualcrop_manualcrop_supported_widgets();
    $widget_settings = $widget_settings['image_image'];

    $settings = array_intersect($settings, $widget_settings);
  }

  if (empty($settings)) {
    // No settings required.
    $result = array_keys($widgets);
  }
  else {
    // Filter all widgets that don't support the required settings.
    $result = array();
    foreach ($widgets as $name => $widget_settings) {
      if (!count(array_diff($settings, $widget_settings))) {
        $result[] = $name;
      }
    }
  }

  if (!empty($widget_type)) {
    return in_array($widget_type, $result);
  }
  else {
    return $result;
  }
}

/**
 * Returns the default widget settings.
 *
 * @return
 *   Array of default widget settings.
 */
function manualcrop_default_widget_settings() {
  $defaults = array(
    // Enable Manual Crop?
    'manualcrop_enable' => FALSE,
    // Enable keyboard shortcuts?
    'manualcrop_keyboard' => TRUE,
    // Show a list of thubnails instead of a selection list or button?
    'manualcrop_thumblist' => FALSE,
    // Enable inline cropping?
    'manualcrop_inline_crop' => FALSE,
    // Show the crop info (width, height...)?
    'manualcrop_crop_info' => TRUE,
    // Automatically update the preview image?
    'manualcrop_instant_preview' => TRUE,
    // Open the crop tool after uploading?
    'manualcrop_instant_crop' => FALSE,
    // Show a default crop area when opening an uncropped image?
    'manualcrop_default_crop_area' => TRUE,
    // Maximize the default crop area?
    'manualcrop_maximize_default_crop_area' => FALSE,
    // Exclude or include the selected styles?
    'manualcrop_styles_mode' => 'include',
    // List of selected styles.
    'manualcrop_styles_list' => array(),
    // List of required crop selections.
    'manualcrop_require_cropping' => array(),
  );

  if (module_exists('insert')) {
    // Filter all styles without a Manual Crop effect?
    $defaults['manualcrop_filter_insert'] = TRUE;
  }

  return $defaults;
}

/**
 * Get the list of required image styles from the widget settings.
 *
 * @param $settings
 *   Widget settings array.
 *
 * @return
 *   List of required image styles.
 */
function manualcrop_instance_required_styles($settings) {
  // Make sure to exclude unavailable styles.
  if (!empty($settings['manualcrop_require_cropping']) && !empty($settings['manualcrop_styles_list'])) {
    if ($settings['manualcrop_styles_mode'] == 'include') {
      return array_intersect($settings['manualcrop_require_cropping'], $settings['manualcrop_styles_list']);
    }
    else {
      return array_diff($settings['manualcrop_require_cropping'], $settings['manualcrop_styles_list']);
    }
  }

  return array();
}

/**
 * Add a croptool to the form element. This extends the FAPI widget or simply adds
 * a new form item to enable cropping in a regular form.
 *
 * @param $form
 *   The form array.
 * @param $form_state
 *   The form state array.
 * @param $element
 *   Form element to be processed. It's preferred to use the form array for
 *   none-FAPI fields.
 * @param $file
 *   The file object.
 * @param $settings
 *   Used to pass-in (additional) widget settings, these settings will
 *   override the instance settings.
 * @param $instance
 *   Field instance array.
 * @param $preview
 *   Name of the preview element.
 *
 * @return
 *    Returns TRUE if a croptool has been added, FALSE otherwise.
 */
function manualcrop_croptool_process(&$form, &$form_state, &$element, $file, $settings = array(), $instance = NULL, $preview = 'preview') {
  static $processed_forms;

  if (user_access('use manualcrop') && (!empty($_COOKIE['has_js']) || variable_get('manualcrop_skip_js_check', FALSE))) {
    $styles = manualcrop_styles_with_crop();

    if (!empty($styles)) {
      // Merge-in the instance or default settings.
      if (is_array($instance)) {
        $settings += $instance['widget']['settings'];
      }
      else {
        $settings += manualcrop_default_widget_settings();
      }

      // Exclude or include styles.
      if (!empty($settings['manualcrop_styles_list'])) {
        if ($settings['manualcrop_styles_mode'] == 'include') {
          $styles = array_intersect_key($styles, $settings['manualcrop_styles_list']);
        }
        else {
          $styles = array_diff_key($styles, $settings['manualcrop_styles_list']);
        }

        if (empty($styles)) {
          // Leave if all styles were filtered.
          return FALSE;
        }
      }

      // Required image styles.
      $required = manualcrop_instance_required_styles($settings);

      // Make sure the build id exists.
      if (!isset($form['#build_id'])) {
        $form['#build_id'] = 'form-' . drupal_random_key();
      }

      // Reset the images array.
      if (!isset($processed_forms[$form['#build_id']])) {
        $processed_forms[$form['#build_id']] = $form['#build_id'];

        if (!isset($form_state['manualcrop_data'])) {
          $form_state['manualcrop_data'] = array('images' => array());
        }
        else {
          $form_state['manualcrop_data']['images'] = array();
        }
      }

      // Get the container and item reference.
      if (is_array($instance)) {
        // A FAPI field instance has been passed, so $element is the container
        // and $element['#value'] can be used for storing data.
        $container = &$element;
        $value = &$element['#value'];
      }
      else {
        // The FAPI is not used, we'll create a manualcrop container as parent
        // for all manualcrop enabled files.
        if (!isset($element['manualcrop'])) {
          $element['manualcrop'] = array(
            '#tree' => TRUE,
          );
        }

        // Create a file specific container.
        $element['manualcrop']['file_' . $file->fid] = array(
          '#type' => 'value',
          '#default_value' => array(),
          '#element_validate' => array('manualcrop_croptool_validate'),
          '#parents' => array('manualcrop', 'file_' . $file->fid),
        );

        // Link to the newly created container and item.
        $container = &$element['manualcrop']['file_' . $file->fid];
        $value = &$container['#default_value'];
      }

      // Alter the preview element if it exists and the thumblist option isn't enabled.
      if (empty($settings['manualcrop_thumblist']) && isset($preview) && isset($element[$preview])) {
        $element[$preview]['#prefix'] = '<div class="manualcrop-preview manualcrop-preview-' . $file->fid . '"><div class="manualcrop-preview-cropped"></div>';
        $element[$preview]['#suffix'] = '</div>';
      }

      // Save some image data to improve processing.
      $image = image_get_info($file->uri);
      $form_state['manualcrop_data']['images'][$file->fid] = array(
        'uri' => $file->uri,
        'filename' => $file->filename,
        'width' => $image['width'],
        'height' => $image['height'],
        'element_parents' => $container['#parents'],
        'required_styles' => $required,
      );

      // Get the crop selections for this file.
      $submitted = (isset($form_state['inline_entity_form']) ? FALSE : $form_state['submitted']);
      if (!$submitted && !isset($value['manualcrop_selections'])) {
        $value['manualcrop_selections'] = array();
        foreach (manualcrop_load_crop_selection($file->uri) as $data) {
          $value['manualcrop_selections'][$data->style_name] = $data->x . '|' . $data->y . '|' . $data->width . '|' . $data->height;
        }
      }

      // Add the dependencies and croptool.
      $js_identifier = _manualcrop_js_identifier(is_array($instance) ? $instance : $file);
      _manualcrop_attach_dependencies($element, $form_state, $js_identifier, $settings);
      _manualcrop_add_croptool($container, $value, $form_state, $file->fid, $js_identifier, $styles, $required, $settings);

      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Adds the #after_build entry to a media element.
 *
 * @param $element
 *   The element to alter.
 *
 * @return
 *   TRUE if the element was altered, FALSE otherwise.
 */
function _manualcrop_media_element_add_after_build(&$element) {
  if (isset($element['#media_options']['global'])) {
    $element['#after_build'][] = 'manualcrop_media_element_process';
    return TRUE;
  }

  return FALSE;
}

/**
 * Get the unique javascript crop settings identifier.
 *
 * @param $data
 *   Field instance array (preferred) or file object.
 *
 * @return
 *   Unique javascript crop settings identifier.
 */
function _manualcrop_js_identifier($data) {
  if (is_array($data) && !empty($data['field_name'])) {
    return drupal_clean_css_identifier($data['field_name']);
  }
  elseif (is_object($data) && isset($data->fid)) {
    return 'manualcrop-file-' . $data->fid;
  }
  else {
    return 'manualcrop-' . md5(serialize($data));
  }
}

/**
 * Attach the required croptool dependencies (files and settings).
 *
 * @param $element
 *   The form element.
 * @param $form_state,
 *   The form state array.
 * @param $js_identifier
 *   Unique javascript crop settings identifier.
 * @param $settings
 *   Widget settings.
 */
function _manualcrop_attach_dependencies(&$element, $form_state, $js_identifier, $settings = array()) {
  static $added;

  // Attach the required files.
  _manualcrop_attach_files($element);

  if (!isset($added[$js_identifier]) || $added[$js_identifier] != $form_state['rebuild']) {
    $added[$js_identifier] = $form_state['rebuild'];

    // Generate image style settings.
    $styles = &drupal_static(__FUNCTION__);

    if (!is_array($styles)) {
      $styles = array();
      foreach (manualcrop_styles_with_crop() as $style_name => $data) {
        $styles[$style_name] = array(
          'effect' => $data['effect']['name'],
          'data' => $data['effect']['data']
        );
      }
    }

    // Attach the element settings.
    $element['#attached']['js'][] = array(
      'data' => array(
        'manualcrop' => array(
          'styles' => $styles,
          'elements' => array(
            $js_identifier => array(
              'keyboard' => !empty($settings['manualcrop_keyboard']),
              'required' => manualcrop_instance_required_styles($settings),
              'instantCrop' => !empty($settings['manualcrop_instant_crop']),
              'defaultCropArea' => !empty($settings['manualcrop_default_crop_area']),
              'maximizeDefaultCropArea' => !empty($settings['manualcrop_maximize_default_crop_area']),
            ),
          ),
        )
      ),
      'type' => 'setting',
    );
  }
}

/**
 * Attach the required css, javascript and libraries.
 *
 * @param $element
 *   The form element on which the files should be attached. Use to NULL
 *   to invoke the Internet Explorer lazy loading css fix.
 */
function _manualcrop_attach_files(&$element = NULL) {
  $ie = (!empty($_SERVER['HTTP_USER_AGENT']) && strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') !== FALSE);

  if ($ie || $element) {
    $path = drupal_get_path('module', 'manualcrop');

    if ($ie) {
      // IE has some issues with the lazy loading of css files, so we preload
      // anything that contains CSS. See http://drupal.org/node/1071818 for more info.
      drupal_add_css($path . '/manualcrop.css');
      libraries_load('jquery.imgareaselect');
    }

    if ($element) {
      $element['#attached']['css'][] = $path . '/manualcrop.css';
      $element['#attached']['js'][] = $path . '/manualcrop.js';
      $element['#attached']['libraries_load'][] = array('jquery.imgareaselect');
      $element['#attached']['libraries_load'][] = array('jquery.imagesloaded');
    }
  }
}

/**
 * Add the actual croptool to a form element.
 *
 * @param $element
 *   The form element to add the croptool to.
 * @param $value
 *   The element value storage.
 * @param $form_state
 *   The form state array.
 * @param $fid
 *   The file id.
 * @param $js_identifier
 *   Unique javascript crop settings identifier.
 * @param $styles
 *   Info of the active image styles, keyed by name.
 * @param $required
 *   Array of required image styles.
 * @param $settings
 *   Widget settings.
 */
function _manualcrop_add_croptool(&$element, $value, &$form_state, $fid, $js_identifier, $styles, $required = array(), $settings = array()) {
  // Get the fileinfo.
  $file_info = $form_state['manualcrop_data']['images'][$fid];

  // Overlay or inline cropping.
  if (!empty($settings['manualcrop_inline_crop'])) {
    $crop_type = 'inline';
  }
  else {
    $crop_type = 'overlay';
  }

  // Add a css class
  $element['#attributes']['class'][] = 'manualcrop-file-' . $fid . '-holder';

  // Original image for cropping.
  $element['manualcrop_' . $crop_type] = array(
    '#type' => 'markup',
    '#markup' => theme('manualcrop_croptool_' . $crop_type, array(
      'attributes' => array(
        'id' => array('manualcrop-' . $crop_type . '-' . $fid),
        'class' => array('manualcrop-' . $crop_type, 'element-hidden'),
      ),
      'image' => array(
        'path' => $file_info['uri'],
        'alt' => $file_info['filename'],
        'width' => $file_info['width'],
        'height' => $file_info['height'],
        'attributes' => array('class' => array('manualcrop-image')),
      ),
      'crop_info' => !empty($settings['manualcrop_crop_info']),
      'instant_preview' => !empty($settings['manualcrop_instant_preview']),
    )),
  );

  // Image style options.
  $style_options = array('' => t('Select a style to crop'));

  // Hidden fields to save the crop selection, one for each image style.
  foreach ($styles as $style_name => $info) {
    $element['manualcrop_selections'][$style_name] = array(
      '#type' => 'hidden',
      '#default_value' => (isset($value['manualcrop_selections'][$style_name]) ? $value['manualcrop_selections'][$style_name] : ''),
      '#attributes' => array(
        'id' => 'manualcrop-area-' . $fid . '-' . $style_name,
        'class' => array('manualcrop-cropdata'),
        'onchange' => 'ManualCrop.selectionStored(this, ' . $fid . ', \'' . $style_name . '\');',
      ),
    );

    $style_options[$style_name] = $info['label'];
  }

  // Default form element options.
  $defaults = array('#weight' => (isset($element['title']['#weight']) ? $element['title']['#weight'] : 1));

  // Element to open the croptool.
  if (!empty($settings['manualcrop_thumblist'])) {
    // Thumbnail list, each image style is transformed in a clickable thumbnail.
    array_shift($style_options);
    foreach ($style_options as $style_name => $style_clean_name) {
      $prefix = '<span class="manualcrop-preview manualcrop-preview-' . $fid . ' manualcrop-preview-' . $fid . '-' . $style_name . '"><span class="manualcrop-preview-cropped"></span>';
      $suffix = '</span>';

      $style_options[$style_name] = theme('manualcrop_thumblist_image', array(
        'style' => $style_clean_name,
        'image' => $prefix . theme('image_style', array(
            'path' => $file_info['uri'],
            'alt' => $file_info['filename'],
            'width' => $file_info['width'],
            'height' => $file_info['height'],
            'style_name' => $style_name,
          )) . $suffix,
        'attributes' => array(
          'class' => array_merge(
            array(
              'manualcrop-style-thumb',
              'manualcrop-style-thumb-' . $fid,
              'manualcrop-style-thumb-' . $fid . '-' . $style_name,
              'manualcrop-style-preview-' . $fid,
              'manualcrop-style-preview-' . $fid . '-' . $style_name
            ),
            (in_array($style_name, $required) ? array('manualcrop-style-required') : array())
          ),
          'href' => 'javascript:void(0);',
          'onmousedown' => "ManualCrop.showCroptool('" . $js_identifier . "', '" . $style_name . "', " . $fid . ");",
          'onclick' => 'return false;',
        ),
      ));
    }

    $element['manualcrop_style'] = array_merge($defaults, array(
      '#markup' => theme('manualcrop_thumblist', array(
        'attributes' => array(
          'class' => array('manualcrop-thumblist', 'manualcrop-thumblist-' . $fid),
        ),
        'images' => $style_options,
      )),
    ));
  }
  elseif (count($style_options) == 2) {
    // Only one style, display a button.
    end($style_options);
    $style_name = key($style_options);

    $element['manualcrop_style'] = array_merge($defaults, array(
      '#type' => 'button',
      '#value' => t('Crop'),
      '#attributes' => array(
        'class' => array_merge(array('manualcrop-style-button', 'manualcrop-style-button-' . $fid), (in_array($style_name, $required) ? array('manualcrop-style-required') : array())),
        'onmousedown' => "ManualCrop.showCroptool('" . $js_identifier . "', '" . $style_name . "', " . $fid . ");",
        'onclick' => 'return false;',
      ),
      '#prefix' => '<div class="manualcrop-style-button-holder">',
      '#suffix' => '</div>',
    ));
  }
  else {
    // Style selection list.
    $element['manualcrop_style'] = array_merge($defaults, array(
      '#type' => 'select',
      '#title' => t('Manual Crop'),
      '#description' => t('Select the image style to crop, the corresponding cropping tool will open.'),
      '#options' => $style_options,
      '#multiple' => FALSE,
      '#attributes' => array(
        'class' => array(
          'manualcrop-identifier-' . $js_identifier,
          'manualcrop-style-select',
          'manualcrop-style-select-' . $fid
        ),
        'onchange' => "ManualCrop.showCroptool('" . $js_identifier . "', this, " . $fid . ");",
      ),
    ));
  }

  // The FAPI widget can have a alt and/or title field, increase their weight.
  if (isset($element['alt'])) {
    $element['alt']['#weight']++;
  }

  if (isset($element['title'])) {
    $element['title']['#weight']++;
  }
}

/**
 * Add the crop functionality to the File Entity form.
 *
 * @param $form
 *   Complete form array.
 * @param $form_state
 *   Form state array.
 * @param $instance_info
 *   Field instance info, this array should contain 3 keys: entity_type, bundle
 *   and field_name.
 */
function _manualcrop_process_file_entity_form(&$form, &$form_state, $instance_info) {
  // Check if a field instance was specified and get its settings.
  if (!empty($instance_info['entity_type']) && !empty($instance_info['bundle']) && !empty($instance_info['field_name'])) {
    $instance = field_info_instance($instance_info['entity_type'], $instance_info['field_name'], $instance_info['bundle']);

    if (!empty($instance)) {
      $settings = $instance['widget']['settings'];
    }
  }

  // No valid fields instance specified, use the File Entity settings.
  if (!isset($settings)) {
    $settings = variable_get('manualcrop_file_entity_settings_' . $form['#entity']->type, manualcrop_default_widget_settings());
  }

  // Add the croptool if Manual Crop has been enabled.
  if (!empty($settings['manualcrop_enable'])) {
    manualcrop_croptool_process($form, $form_state, $form, $form['#entity'], $settings);

    // Add the submit handler. Sometimes we have to add it to the action button
    // and sometimes we have to add it to the general #submit, based on whether
    // or not the submit button already has a handler.
    if (!empty($form['actions']['submit']['#submit'])) {
      $form['actions']['submit']['#submit'][] = 'manualcrop_croptool_submit';
    }
    else {
      $form['#submit'][] = 'manualcrop_croptool_submit';
    }
  }
}

/**
 * Save the Manual Crop data for a file.
 *
 * @param $file
 *   The file entity being saved for.
 * @param $data
 *   The data as it is to be written, keyed by style name.
 */
function manualcrop_save_crop_data($file, $data) {
  // If file_entity_revisions isn't enabled, use 0.
  $vid = (isset($file->vid) ? $file->vid : 0);

  // Delete the existing data.
  db_delete('manualcrop')
    ->condition('fid', $file->fid)
    ->condition('vid', $vid)
    ->condition('style_name', array_keys($data))
    ->execute();

  // Save the new crop selections.
  foreach ($data as $style_name => $selection) {
    if ($selection) {
      $record = array_merge($selection, array(
        'fid' => $file->fid,
        'vid' => $vid,
        'style_name' => $style_name
      ));
      drupal_write_record('manualcrop', $record);
    }
  }
}

/**
 * Gets the crop area for an image.
 *
 * @param $file
 *   Path to an image file.
 * @param $style_name
 *   Image style machine name, leave empty for all styles.
 *
 * @return
 *   When $style_name is set, a single crop selection will be returned. Otherwise
 *   the result is an array of crop selection objects keyed by style name.
 *   Each object contains following items:
 *   - "style_name": The machine name of the image style this cropping area applies on.
 *   - "x": An integer representing the top left corner's x-position in pixels.
 *   - "y": An integer representing the top left corner's y-position in pixels.
 *   - "width": An integer representing the width in pixels.
 *   - "height": An integer representing the height in pixels.
 */
function manualcrop_load_crop_selection($file, $style_name = NULL) {
  // Choose which table to use based on whether or not file_entity_revisions module is enabled.
  if (module_exists('file_entity_revisions')) {
    $table = 'file_managed_revisions';
    $key = 'vid';
  }
  else {
    $table = 'file_managed';
    $key = 'fid';
  }

  if (isset($style_name)) {
    $result = db_query('SELECT c.x, c.y, c.width, c.height FROM {manualcrop} c INNER JOIN {' . $table . '} f ON c.' . $key . ' = f.' . $key . ' WHERE c.style_name = :name AND f.uri = :uri', array(
      ':name' => $style_name,
      ':uri' => $file,
    ));

    return $result->fetchObject();
  }
  else {
    $result = db_query('SELECT c.style_name, c.x, c.y, c.width, c.height FROM {manualcrop} c INNER JOIN {' . $table . '} f ON c.' . $key . ' = f.' . $key . ' WHERE f.uri = :uri', array(
      ':uri' => $file,
    ));

    return $result->fetchAllAssoc('style_name');
  }
}

/**
 * Returns the styles that have crop settings.
 *
 * @param $include_reuse
 *   Set to TRUE to include styles with a Manual Crop reuse effect.
 * @param $exclude_arg
 *   Exclude the style that is set as a menu argument on this index.
 * @param $return_label
 *   Set to TRUE to return the label instead of an array.
 *
 * @return
 *   If $label is set to TRUE, this function will return an array of style labels
 *   keyed by style name. Otherwise an array of crop-enabled styles will be
 *   returned. This array is also keyed by style name and each element in this
 *   array is also an array with 2 elements:
 *   - "label": Human readable style label.
 *   - "effect": Manual Crop effect data.
 */
function manualcrop_styles_with_crop($include_reuse = FALSE, $exclude_arg = NULL, $return_label = FALSE) {
  $hascrop = &drupal_static(__FUNCTION__);

  if (!is_array($hascrop)) {
    $hascrop = array(array(), array());

    foreach (image_styles() as $style_name => $style) {
      if (!empty($style['effects'])) {
        // Check if the first effect is a Manual Crop cropping effect.
        $effect = reset($style['effects']);

        if (_manualcrop_is_own_effect($effect)) {
          $label = _manualcrop_image_style_label($style);

          $hascrop[1][$style_name] = array(
            'label' => $label,
            'effect' => $effect,
          );

          if (_manualcrop_is_own_effect($effect, TRUE)) {
            $hascrop[0][$style_name] = array(
              'label' => $label,
              'effect' => $effect,
            );
          }
        }
      }
    }
  }

  // With or without reuse effects.
  $styles = $hascrop[(int) $include_reuse];

  // Exclude a style by menu arument.
  if (!is_null($exclude_arg)) {
    $exclude_arg = arg($exclude_arg);
    if (isset($styles[$exclude_arg])) {
      unset($styles[$exclude_arg]);
    }
  }

  // Only the labels should be returned.
  if ($return_label) {
    foreach ($styles as $style_name => $style) {
      $styles[$style_name] = $style['label'];
    }
  }

  return $styles;
}

/**
 * Transform a style name into a more readable variant.
 *
 * @param $style
 *   Image style info array or style name.
 *
 * @return
 *   Cleaned-up image style name.
 */
function _manualcrop_image_style_label($style) {
  global $language;
  static $custom_strings;

  // Get the image style info.
  if (!is_array($style)) {
    $styles = image_styles();
    if (isset($styles[$style])) {
      $style = $styles[$style];
    }
    else {
      // Normally we shouldn't be here...
      $style = array('name' => $style);
    }
  }

  // The label is only available in Drupal 7.23 and up.
  if (isset($style['label'])) {
    return $style['label'];
  }
  else {
    $style_name = $style['name'];
    $langcode = (isset($language->language) ? $language->language : 'en');

    // Load custom string for overriding.
    if (!isset($custom_strings[$langcode])) {
      $custom_strings[$langcode] = variable_get('locale_custom_strings_' . $langcode, array());
    }

    // Get the human readable name from the custom strings or make it ourself.
    if (isset($custom_strings[$langcode]['']['image-style-' . $style_name])) {
      return $custom_strings[$langcode]['']['image-style-' . $style_name];
    }
    else {
      return ucwords(str_replace('_', ' ', $style_name));
    }
  }
}

/**
 * Checks if the effect is a Manual Crop effect.
 *
 * @param $effect
 *   Image style effect information array.
 * @param $crop_effect
 *   Set to TRUE to require a cropping effect; set to FALSE to require
 *   a reuse effect. Defaults to NULL, which ignores effect type.
 *
 * @return
 *   TRUE if this is a Manual Crop (cropping/reuse) effect, FALSE otherwise.
 */
function _manualcrop_is_own_effect($effect, $crop_effect = NULL) {
  if ($effect['module'] == 'manualcrop') {
    if (is_null($crop_effect)) {
      return TRUE;
    };

    return $crop_effect ^ in_array($effect['name'], array('manualcrop_reuse', 'manualcrop_auto_reuse'));
  }
  return FALSE;
}

/**
 * Get the image style name that should be used when processing the auto reuse effect.
 *
 * @param $file
 *   Path to an image file.
 * @param $data
 *   Auto reuse effect data.
 *
 * @return
 *   Image style name that can be reused or FALSE if no crop selection was found.
 */
function _manualcrop_get_auto_reuse_style_name($file, $data) {
  // Get the crop selections.
  $crop = manualcrop_load_crop_selection($file);

  if ($crop) {
    if (!empty($data['style_priority'])) {
      // Get a list of existing crop styles (keys) ordered by priority.
      $styles = array_flip($data['style_priority']) + $crop;
      $crop = array_intersect_key($styles, $crop);
    }

    // Return the first crop selection style name.
    reset($crop);
    return key($crop);
  }

  return FALSE;
}

/**
 * Adds a cache control parameter to the image URI so the image will be reloaded
 * if the crop selection was changed since it was last feched by the browser.
 *
 * @param $style_name
 *   Image style name.
 * @param $path
 *   The absolute URL where the styled image can be downloaded.
 *
 * @return
 *   The altered image URL or NULL if the URL wasn't changed.
 */
function _manualcrop_add_cache_control($style_name, $url) {
  // Is cache control enabled?
  if (variable_get('manualcrop_cache_control', TRUE)) {
    $styles = manualcrop_styles_with_crop(TRUE);

    // Does this image style have a Manual Crop effect?
    if (isset($styles[$style_name])) {
      $cache_key = 'manualcrop:' . md5($url);

      // Attempt to load the HTTP cache-controller from cache.
      if ($cached_url = cache_get($cache_key)) {
        return $cached_url->data;
      }

      // Get the image path from the URL.
      $match = '/styles/' . $style_name . '/';
      $path = parse_url($url, PHP_URL_PATH);
      $path = drupal_substr($path, (strrpos($path, $match) + drupal_strlen($match)));
      $path = explode('/', $path);

      // Build the local image URI.
      $scheme = array_shift($path);
      $target = implode('/', $path);
      $image_uri = $scheme . '://' . urldecode($target);

      // Get the image effect.
      $effect = $styles[$style_name]['effect'];

      if (_manualcrop_is_own_effect($effect, FALSE)) {
        unset($style_name);

        switch ($effect['name']) {
          case 'manualcrop_reuse':
            // Use the reuse style to load the crop selection.
            if (!empty($effect['data']['reuse_crop_style'])) {
              $style_name = $effect['data']['reuse_crop_style'];
            }
            break;

          case 'manualcrop_auto_reuse':
            // Get the first applied crop selection.
            if ($crop = manualcrop_load_crop_selection($image_uri)) {
              $crop = reset($crop);
            }
            break;
        }
      }

      // Load the crop selection.
      if (isset($style_name)) {
        $crop = manualcrop_load_crop_selection($image_uri, $style_name);
      }

      // Add the cache controller and cache the new URL.
      if (!empty($crop)) {
        $url .= (strpos($url, '?') ? '&' : '?') . 'c=' . md5($crop->x . '|' . $crop->y . '|' . $crop->width . '|' . $crop->height);
        cache_set($cache_key, $url);

        return $url;
      }
    }
  }

  return NULL;
}

/**
 * Update or remove a style name in all Manual Crop field widgets.
 *
 * @param $style_name
 *   Current image style name.
 * @param $new_style_name
 *   New image style name if renamed, a NULL value will remove the style from the settings.
 */
function _manualcrop_update_style_name_in_field_widget($style_name, $new_style_name = NULL) {
  foreach (field_info_fields() as $field) {
    if ($field['module'] == 'image') {
      foreach ($field['bundles'] as $entity_type => $bundles) {
        foreach ($bundles as $bundle) {
          // Check each instance for processing.
          $instance = field_info_instance($entity_type, $field['field_name'], $bundle);
          $settings = &$instance['widget']['settings'];

          if (manualcrop_supported_widgets($instance['widget']['type']) && (!empty($settings['manualcrop_require_cropping']) || !empty($settings['manualcrop_styles_list']))) {
            $list = array();

            // Add all existing settings to the list.
            if (!empty($settings['manualcrop_require_cropping'])) {
              $list['manualcrop_require_cropping'] = &$settings['manualcrop_require_cropping'];
            }

            if (!empty($settings['manualcrop_styles_list'])) {
              $list['manualcrop_styles_list'] = &$settings['manualcrop_styles_list'];
            }

            // Process all settings.
            foreach ($list as $key => &$item) {
              if (isset($item[$style_name])) {
                if (!is_null($new_style_name)) {
                  $item[$new_style_name] = $new_style_name;
                }

                unset($item[$style_name]);
              }
              else {
                // Not processed, so remove it from the list.
                unset($list[$key]);
              }
            }

            if (!empty($list)) {
              // Settings where updated, save the instance.
              field_update_instance($instance);
            }
          }
        }
      }
    }
  }
}

/**
 * Update or remove a style name in all Manual Crop reuse image effects.
 *
 * @param $style_name
 *   Current image style name.
 * @param $new_style_name
 *   New image style name if renamed, a NULL value will remove the effect from the style.
 */
function _manualcrop_update_style_name_in_image_effect($style_name, $new_style_name = NULL) {
  foreach (image_styles() as $style) {
    if (!empty($style['effects'])) {
      // Check if the first effect is a Manual Crop effect.
      $effect = reset($style['effects']);

      // Check if this is a Manual Crop reuse effect.
      if (_manualcrop_is_own_effect($effect)) {
        $data = $effect['data'];
        $key = (isset($data['reuse_crop_style']) ? 'reuse_crop_style' : 'fallback_style');

        // Only update if needed.
        if (isset($data[$key]) && $data[$key] == $style_name) {
          if (is_null($new_style_name) && $effect['name'] == 'manualcrop_reuse') {
            // The reuse effect requires an image style.
            image_effect_delete($effect);
          }
          else {
            $effect['data'][$key] = $new_style_name;
            image_effect_save($effect);
          }
        }
      }
    }
  }
}
