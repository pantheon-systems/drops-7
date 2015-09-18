<?php
/**
 * @file
 * Admin functionality for the Manual Crop module.
 */

/**
 * Form builder; Global settings configuration form.
 */
function manualcrop_global_settings_form() {
  $form = array();

  $form['manualcrop_cache_control'] = array(
    '#type' => 'checkbox',
    '#title' => t('Reload cache-control'),
    '#description' => t('When enabled, a cache-controlling variable will be added to the cropped image path. This variable will change as soon as you re-crop the image and provide the user with the latest, re-cropped version of the image when the image is reloaded.'),
    '#default_value' => variable_get('manualcrop_cache_control', TRUE),
  );

  $form['manualcrop_skip_js_check'] = array(
    '#type' => 'checkbox',
    '#title' => t('Skip javascript cookie check'),
    '#description' => t('Enable Manual Crop even if the <em>has_js</em> cookie (which is set by Drupal when the brower supports javascript) is missing.'),
    '#default_value' => variable_get('manualcrop_skip_js_check', FALSE),
  );

  $form['#submit'] = array('manualcrop_global_settings_form_submit');

  return system_settings_form($form);
}

/**
 * Form submit handler; Clears the Manual Crop cache if the cache control gets disabled.
 */
function manualcrop_global_settings_form_submit($form, &$form_state) {
  if (!$form_state['values']['manualcrop_cache_control']) {
    // Clear the cached image paths.
    cache_clear_all('manualcrop', 'cache', TRUE);
  }
}

/**
 * Form builder; Configuration settings for the crop and scale effect.
 *
 * @param $data
 *   The current configuration for this crop and scale effect.
 *
 * @return
 *   The form structure array.
 */
function manualcrop_crop_and_scale_form($data) {
  $form = image_scale_form($data);

  $form['width']['#required'] = TRUE;
  $form['height']['#required'] = TRUE;

  $form['respectminimum'] = array(
    '#type' => 'checkbox',
    '#title' => t('Respect minimum'),
    '#description' => t("Make sure the selected crop area is at least as big as the destination size. This doesn't enforce minimum image dimensions."),
    '#default_value' => (isset($data['respectminimum']) ? $data['respectminimum'] : TRUE),
  );

  $form['onlyscaleifcrop'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only scale if cropped'),
    '#description' => t('Only scale the image if it was manually cropped.'),
    '#default_value' => (isset($data['onlyscaleifcrop']) ? $data['onlyscaleifcrop'] : FALSE),
  );

  return $form;
}

/**
 * Returns HTML for a summary of an image crop and scale effect.
 *
 * @param $variables
 *   An associative array containing:
 *   - data: The current configuration for this crop and scale effect.
 *
 * @return
 *   Summary HTML.
 *
 * @ingroup themeable
 */
function theme_manualcrop_crop_and_scale_summary($variables) {
  $data = $variables["data"];
  $str = check_plain($data['width']) . 'x' . check_plain($data['height']);

  if ($data['upscale'] || $data['respectminimum'] || !empty($data['respectminimum'])) {
    $str .= ' (' . ($data['upscale'] ? t('upscaling allowed') : '');

    if ($data['upscale'] && ($data['respectminimum'] || !empty($data['onlyscaleifcrop']))) {
      $str .= ($data['respectminimum'] && !empty($data['onlyscaleifcrop']) ? ', ' : ' ' . t('and') . ' ');
    }

    if ($data['respectminimum']) {
      $str .= t('crop minimum dimensions') . (!empty($data['onlyscaleifcrop']) ? ' ' . t('and') . ' ' : '');
    }

    if (!empty($data['onlyscaleifcrop'])) {
      $str .= t('only scale if cropped');
    }

    $str .= ')';
  }

  return $str;
}

/**
 * Form builder; Configuration settings for crop effect.
 *
 * @param $data
 *   The current configuration for this crop effect.
 *
 * @return
 *   The form structure array.
 */
function manualcrop_crop_form($data) {
  $form = image_resize_form($data);

  $form['width']['#title'] = t('Minimum crop width');
  $form['width']['#description'] = t('Only applied if a user tries to crop, this enforces no minimum image width!');
  $form['width']['#required'] = FALSE;

  $form['height']['#title'] = t('Minimum crop height');
  $form['height']['#description'] = t('Only applied if a user tries to crop, this enforces no minimum image height!');
  $form['height']['#required'] = FALSE;

  $form['keepproportions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Maintain proportions'),
    '#description' => t('Maintain the proportions while cropping. This requires setting a width and height.'),
    '#default_value' => !empty($data['keepproportions']),
    '#element_validate' => array('manualcrop_keepproportions_validate'),
    '#states' => array(
      'enabled' => array(
        'input[name="data[width]"]' => array('filled' => TRUE),
        'input[name="data[height]"]' => array('filled' => TRUE),
      ),
    ),
  );

  // Load the crop image styles and exclude the style that's being edited.
  $styles = manualcrop_styles_with_crop(FALSE, 5, TRUE);

  $form['reuse_crop_style'] = array(
    '#type' => 'select',
    '#title' => t('Fallback cropping style'),
    '#description' => t('The image style whose cropping should be applied if none is given for this style.'),
    '#options' => array('' => '- ' . t('None (no crop)') . ' -') + $styles,
    '#default_value' => isset($data['reuse_crop_style']) ? $data['reuse_crop_style'] : '',
  );

  return $form;
}

/**
 * Element validation handler; validates the keepproportions checkbox from the
 * crop effect form.
 */
function manualcrop_keepproportions_validate($element, $form_state) {
  $values = $form_state['values']['data'];

  if ($values['keepproportions'] && (!$values['width'] || !$values['height'])) {
    form_set_error('data][keepproportions', t('Set a width and height if proportions should be maintained.'));
  }
}

/**
 * Returns HTML for a summary of an image crop effect.
 *
 * @param $variables
 *   An associative array containing:
 *   - data: The current configuration for this crop effect.
 *
 * @return
 *   Summary HTML.
 *
 * @ingroup themeable
 */
function theme_manualcrop_crop_summary($variables) {
  $data = $variables['data'];
  $str = '';

  if ($data['width'] && $data['height']) {
    $str .= t('minimum crop dimensions ') . ' ' . check_plain($data['width']) . 'x' . check_plain($data['height']);
  }
  elseif ($data['width']) {
    $str .= t('minimum crop width @width', array('@width' => $data['width']));
  }
  elseif ($data['height']) {
    $str .= t('minimum crop height @height', array('@height' => $data['height']));
  }

  if (!empty($data['keepproportions'])) {
    if ($str) {
      $str .= ' ' . t('and') . ' ';
    }

    $str .= t('maintain proportions');
  }

  if (!empty($data['reuse_crop_style'])) {
    if ($str) {
      $str .= ' ' . t('and') . ' ';
    }

    $str .= t('using the crop of @style as fallback', array('@style' => _manualcrop_image_style_label($data['reuse_crop_style'])));
  }

  if ($str) {
    $str = '(' . $str . ')';
  }

  return $str;
}

/**
 * Form builder; Configuration settings for the reuse effect.
 *
 * @param $data
 *   The current configuration for this reuse effect.
 *
 * @return
 *   The form structure array.
 */
function manualcrop_reuse_form($data) {
  // Load the crop image styles and exclude the style that's being edited.
  $styles = manualcrop_styles_with_crop(FALSE, 5, TRUE);

  if (empty($styles)) {
    drupal_set_message(t('No Manual Crop enabled image style could be found. To reuse a crop selection, you need to create at least one image style that uses Manual Crop.'), 'warning');
    drupal_goto('admin/config/media/image-styles/edit/' . arg(5));
  }

  $form['reuse_crop_style'] = array(
    '#type' => 'select',
    '#title' => t('Base image style'),
    '#description' => t('Base this image style upon the crop selection of the selected style.'),
    '#options' => $styles,
    '#default_value' => (isset($data['reuse_crop_style']) ? $data['reuse_crop_style'] : ''),
    '#required' => TRUE,
  );

  $form['apply_all_effects'] = array(
    '#type' => 'checkbox',
    '#title' => t('Apply all effects'),
    '#description' => t('Apply all effects of the base image style.'),
    '#default_value' => !empty($data['apply_all_effects']),
  );

  return $form;
}

/**
 * Returns HTML for a summary of an image crop selection reuse effect.
 *
 * @param $variables
 *   An associative array containing:
 *   - data: The current configuration for this crop selection reuse effect.
 *
 * @return
 *   Summary HTML.
 *
 * @ingroup themeable
 */
function theme_manualcrop_reuse_summary($variables) {
  return '(' . t('using the crop of @style', array('@style' => _manualcrop_image_style_label($variables['data']['reuse_crop_style']))) . ')';
}

/**
 * Form Builder; Configuration settings for the auto reuse effect.
 *
 * @param $data
 *   The current configuration for this effect.
 *
 * @return
 *   The form structure array.
 */
function manualcrop_auto_reuse_form($data) {
  // Load the crop image styles and exclude the style that's being edited.
  $styles = manualcrop_styles_with_crop(FALSE, 5, TRUE);

  if (empty($styles)) {
    drupal_set_message(t('No Manual Crop enabled image style could be found. To reuse a crop selection, you need to create at least one image style that uses Manual Crop.'), 'warning');
    drupal_goto('admin/config/media/image-styles/edit/' . arg(5));
  }

  // Table with all crop image styles.
  $form['style_priority'] = array(
    '#tree' => TRUE,
    '#header' => array(
      t('Image style'),
      t('Weight'),
    ),
    '#rows' => array(),
    '#theme' => 'table',
    '#attributes' => array(
      'id' => 'manualcrop-style-priority',
    ),
  );

  if (empty($data['style_priority'])) {
    // Make sure the style_priority key exists.
    $data['style_priority'] = array();
  }
  else {
    // Put the styles in their correct order.
    $styles = array_merge(array_fill_keys($data['style_priority'], 0), $styles);
    $styles = array_filter($styles);
  }

  // Add each crop image style as row with a weight (priority) element.
  $maxweight = count($data['style_priority']);
  foreach ($styles as $style_name => $label) {
    $weight = array_search($style_name, $data['style_priority']);
    $weight = array(
      '#type' => 'weight',
      '#default_value' => ($weight === FALSE ? $maxweight : $weight),
      '#attributes' => array(
        'class' => array('manualcrop-style-priority-weight'),
      ),
    );

    $form['style_priority'][$style_name] = &$weight;
    $form['style_priority']['#rows'][] = array(
      'data' => array(
        array('data' => check_plain($label)),
        array('data' => &$weight),
      ),
      'class' => array('draggable'),
    );

    unset($weight);
  }

  // Add a tabledrag for a cleaner interface.
  drupal_add_tabledrag('manualcrop-style-priority', 'order', 'sibling', 'manualcrop-style-priority-weight');

  // Get a list of allowed fallback styles.
  $styles = array_diff_key(image_styles(), manualcrop_styles_with_crop(TRUE));
  $styles = array_map('_manualcrop_image_style_label', $styles);
  $styles = array('' => t('None')) + $styles;

  // Remove the current style.
  if (isset($styles[arg(5)])) {
    unset($styles[arg(5)]);
  }

  $form['apply_all_effects'] = array(
    '#type' => 'checkbox',
    '#title' => t('Apply all effects'),
    '#description' => t('Apply all effects of the reused image style.'),
    '#default_value' => !empty($data['apply_all_effects']),
  );

  $form['fallback_style'] = array(
    '#type' => 'select',
    '#title' => t('Fallback image style'),
    '#description' => t('Image style to use if no cropping selections is found for an image.'),
    '#options' => $styles,
    '#default_value' => (isset($data['fallback_style']) ? $data['fallback_style'] : ''),
  );

  return $form;
}

/**
 * Form submit handler for the auto reuse effect; Sorts and prepares the styles
 * priority array for saving.
 */
function manualcrop_auto_reuse_form_submit($form, &$form_state) {
  $data = &$form_state['values']['data'];

  // Sort by weight and use the keys as values.
  asort($data['style_priority']);
  $data['style_priority'] = array_keys($data['style_priority']);
}

/**
 * Returns HTML for a summary of an image crop selection auto reuse effect.
 *
 * @param $variables
 *   An associative array containing:
 *   - data: The current configuration for this crop selection select effect.
 *
 * @return
 *   Summary HTML.
 *
 * @ingroup themeable
 */
function theme_manualcrop_auto_reuse_summary($variables) {
  if (!empty($variables['data']['fallback_style'])) {
    return '(' . t('using @style as fallback style', array('@style' => _manualcrop_image_style_label($variables['data']['fallback_style']))) . ')';
  }

  return '';
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function manualcrop_form_image_effect_form_alter(&$form, &$form_state) {
  $style = $form_state['image_style'];
  $effect = $form_state['image_effect'];

  if (_manualcrop_is_own_effect($effect)) {
    // Effect specific submit handler.
    if (function_exists($effect['form callback'] . '_submit')) {
      array_unshift($form['#submit'], $effect['form callback'] . '_submit');
    }

    // Add the effect info on submit.
    array_unshift($form['#submit'], 'manualcrop_add_effect_info');
  }
  elseif (!empty($style['effects'])) {
    // Force the effect order for none-Manual Crop effects.
    $form['#submit'][] = 'manualcrop_force_effect_order';
  }
}

/**
 * Effect form submit handler; Adds some extra info (style name and weight)
 * to the effect.
 */
function manualcrop_add_effect_info($form, &$form_state) {
  $style = $form_state['image_style'];
  $effect = $form_state['image_effect'];

  if (_manualcrop_is_own_effect($form_state['image_effect'], TRUE)) {
    // Set style name so the effect can use it.
    $form_state['values']['data']['style_name'] = $style['name'];
  }

  if (empty($style['effects'])) {
    // First effect, so weight is 0.
    $form_state['values']['weight'] = 0;
  }
  elseif (!isset($effect['ieid'])) {
    // New effect, the weight must be the weight of the first effect minus one.
    $first = reset($style['effects']);
    $form_state['values']['weight'] = $first['weight'] - 1;
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function manualcrop_form_image_effect_delete_form_alter(&$form, &$form_state) {
  if (!is_array($form['#submit'])) {
    $form['#submit'] = array('image_effect_delete_form_submit');
  }

  if (_manualcrop_is_own_effect($form_state['image_effect'], TRUE)) {
    // Submit handler to cleanup the crop selections.
    array_unshift($form['#submit'], 'manualcrop_image_effect_delete_form_submit');
  }
}

/**
 * Effect deletion form submit handler; Cleanup old selections.
 */
function manualcrop_image_effect_delete_form_submit($form, &$form_state) {
  manualcrop_image_style_delete($form_state['image_style']);
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function manualcrop_form_image_style_form_alter(&$form, &$form_state) {
  $style = $form_state['image_style'];

  if (!empty($style['effects'])) {
    if (!is_array($form['#submit'])) {
      $form['#submit'] = array('image_style_form_submit');
    }

    // Reorder effects on submit.
    $form['#submit'][] = 'manualcrop_force_effect_order';

    // Update style name in effect data.
    $form['#submit'][] = 'manualcrop_update_style_name';

    // If there's already a Manual Crop effect we have to filter all
    // our effects from the add new effect list.
    if (isset($form['effects']['new']['new'])) {
      foreach ($style['effects'] as $effect) {
        if (_manualcrop_is_own_effect($effect)) {
          $form['effects']['new']['new']['#options'] = array_diff_key(
            $form['effects']['new']['new']['#options'],
            manualcrop_image_effect_info()
          );
          break;
        }
      }
    }
  }
}

/**
 * Effect and style form submit handler; Force the Manual Crop effect to be the first one.
 */
function manualcrop_force_effect_order($form, &$form_state) {
  // In both forms (style and effect) we find the style in the same key.
  $style = $form_state['image_style'];

  // If it's the style form, check if the style name was updated.
  if ($form['#form_id'] == 'image_style_form' && isset($form_state['values']['name']) && $style['name'] != $form_state['values']['name']) {
    $style['name'] = $form_state['values']['name'];
  }

  // The style array is received trough the loader to make sure the function
  // works with the latest version.
  $style = image_style_load($style['name']);

  if (!empty($style['effects'])) {
    foreach ($style['effects'] as $eid => $effect) {
      if (_manualcrop_is_own_effect($effect)) {
        $first = reset($style['effects']);

        if ($eid != key($style['effects'])) {
          // Change the weight of this effect to the weight of the first effect minus 1.
          $effect['weight'] = $first['weight'] - 1;
          image_effect_save($effect);
        }

        break;
      }
    }
  }
}

/**
 * Style form submit handler; Update the style name in the effect data and in the widgets.
 */
function manualcrop_update_style_name($form, &$form_state) {
  $style = $form_state['image_style'];

  // Check if the style name should be updated.
  if (isset($form_state['values']['name']) && $style['name'] != $form_state['values']['name']) {
    if (!empty($style['effects'])) {
      $effect = reset($style['effects']);

      if (_manualcrop_is_own_effect($effect, TRUE)) {
        // Change the style name in the effect data and save it.
        $effect['data']['style_name'] = $form_state['values']['name'];
        image_effect_save($effect);

        // Update the crop selections.
        db_update('manualcrop')
          ->fields(array('style_name' => $form_state['values']['name']))
          ->condition('style_name', $style['name'])
          ->execute();

        // Update the style name in the field widgets.
        _manualcrop_update_style_name_in_field_widget($style['name'], $form_state['values']['name']);
      }
    }

    // Update the style name in the image effects.
    _manualcrop_update_style_name_in_image_effect($style['name'], $form_state['values']['name']);
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function manualcrop_form_field_ui_field_edit_form_alter(&$form, $form_state) {
  if (isset($form['instance']) && isset($form['instance']['widget']['type']['#value'])) {
    // Get the instance and widget type.
    $element = &$form['instance'];
    $widget_type = $element['widget']['type']['#value'];

    if (manualcrop_supported_widgets($widget_type)) {
      // Get the field instance and setting.
      $instance = field_info_instance($element['entity_type']['#value'], $element['field_name']['#value'], $element['bundle']['#value']);
      $settings = $instance['widget']['settings'];

      // Add our custom field settings.
      $element['settings'] += manualcrop_field_widget_settings_form($widget_type, $settings);

      // Alter the existing form elements.
      manualcrop_field_widget_settings_form_alter($element, $settings);
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function manualcrop_form_file_entity_file_type_form_alter(&$form, &$form_state) {
  if (isset($form['#file_type']) && $form['#file_type']->type == 'image') {
    // Get the previously saved or default settings.
    $settings = variable_get('manualcrop_file_entity_settings_' . $form['#file_type']->type, manualcrop_default_widget_settings());

    // Add the settings form elements.
    $form += manualcrop_field_widget_settings_form('image_image', $settings);
    $form['manualcrop']['#tree'] = TRUE;
    unset($form['manualcrop']['#parents']);

    $form['#submit'][] = 'manualcrop_form_file_entity_file_type_form_submit';
  }
}

/**
 * Form submit handler; Saves the Manual Crop settings of a File Entity type.
 */
function manualcrop_form_file_entity_file_type_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['manualcrop'])) {
    variable_set('manualcrop_file_entity_settings_' . $form['#file_type']->type, $form_state['values']['manualcrop']);
  }
}

/**
 * Add the Manual Crop field widget settings.
 *
 * @param $widget_type
 *   Widget type.
 * @param $settings
 *   Current settings.
 *
 * @return
 *   Form elements to add.
 */
function manualcrop_field_widget_settings_form($widget_type, $settings) {
  $form = array();

  // Get a list of styles.
  $style_options = manualcrop_styles_with_crop(FALSE, NULL, TRUE);

  // Add the Manual Crop fieldset.
  $form['manualcrop'] = array(
    '#type' => 'fieldset',
    '#title' => t('Manual Crop'),
    '#description' => t(''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 25,
    '#parents' => array('instance', 'widget', 'settings'),
  );

  $form['manualcrop']['manualcrop_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Enable the Manual Crop features. To enable Manual Crop for this field, you must configure a Manual Crop image effect on at least one image style.'),
    '#default_value' => ($settings['manualcrop_enable'] && !empty($style_options)),
    '#disabled' => empty($style_options),
  );

  $form['manualcrop']['manualcrop_keyboard'] = array(
    '#type' => 'checkbox',
    '#title' => t('Keyboard shortcuts'),
    '#description' => t('Enable the keyboard shortcuts.'),
    '#default_value' => $settings['manualcrop_keyboard'],
  );

  if (manualcrop_supported_widgets($widget_type, 'thumblist')) {
    $form['manualcrop']['manualcrop_thumblist'] = array(
      '#type' => 'checkbox',
      '#title' => t('List thumbs'),
      '#description' => t('Instead showing a button or selection list, show all thumbails (this will disable the preview thumbnail).'),
      '#default_value' => $settings['manualcrop_thumblist'],
    );
  }

  if (manualcrop_supported_widgets($widget_type, 'inline_crop')) {
    $form['manualcrop']['manualcrop_inline_crop'] = array(
      '#type' => 'checkbox',
      '#title' => t('Inline cropping'),
      '#description' => t('Instead of opening an overlay, use inline cropping.'),
      '#default_value' => $settings['manualcrop_inline_crop'],
    );
  }

  $form['manualcrop']['manualcrop_crop_info'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show crop info'),
    '#description' => t('Show the crop selection details.'),
    '#default_value' => $settings['manualcrop_crop_info'],
  );

  $form['manualcrop']['manualcrop_instant_preview'] = array(
    '#type' => 'checkbox',
    '#title' => t('Instant preview'),
    '#description' => t('Show an instant preview of the crop selection.'),
    '#default_value' => $settings['manualcrop_instant_preview'],
  );

  if (manualcrop_supported_widgets($widget_type, 'instant_crop')) {
    $form['manualcrop']['manualcrop_instant_crop'] = array(
      '#type' => 'checkbox',
      '#title' => t('Crop after upload'),
      '#description' => t('Open the cropping tool immediatly after a file upload. Please note that this feature requires a single image style to be enabled. Also it will only be applied for screens of at least 768px width.'),
      '#default_value' => $settings['manualcrop_instant_crop'],
    );
  }

  $form['manualcrop']['manualcrop_default_crop_area'] = array(
    '#type' => 'checkbox',
    '#title' => t('Default crop area'),
    '#description' => t('Create a default crop area when opening the croptool for uncropped images.'),
    '#default_value' => $settings['manualcrop_default_crop_area'],
  );

  $form['manualcrop']['manualcrop_maximize_default_crop_area'] = array(
    '#type' => 'checkbox',
    '#title' => t('Maximize default crop area'),
    '#description' => t('Maximize the default crop area.'),
    '#default_value' => $settings['manualcrop_maximize_default_crop_area'],
    '#states' => array(
      'disabled' => array(
        'input[name="instance[widget][settings][manualcrop_default_crop_area]"]' => array('checked' => FALSE),
        'input[name="instance[widget][settings][manualcrop_default_crop_area]"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['manualcrop']['manualcrop_styles_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Styles list mode'),
    '#options' => array(
      'include' => t('Include selected styles'),
      'exclude' => t('Exclude selected styles'),
    ),
    '#default_value' => $settings['manualcrop_styles_mode'],
  );

  $style_options = array_map('check_plain', $style_options);

  $form['manualcrop']['manualcrop_styles_list'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Styles list'),
    '#description' => t('Select all styles that should be shown or hidden (as selected above) in the widget. Please note that hiding styles will override requiring them and existing cropping selections will be kept.'),
    '#options' => $style_options,
    '#default_value' => $settings['manualcrop_styles_list'],
    '#element_validate' => array('manualcrop_checkboxes_filter'),
    '#multicolumn' => array('width' => 3),
  );

  $form['manualcrop']['manualcrop_require_cropping'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Required croppings'),
    '#description' => t('Select all styles that must have a crop selection.'),
    '#options' => $style_options,
    '#default_value' => $settings['manualcrop_require_cropping'],
    '#element_validate' => array('manualcrop_checkboxes_filter'),
    '#multicolumn' => array('width' => 3),
  );

  return $form;
}

/**
 * Alters the existing field widget settings form elements.
 *
 * @param $form
 *   Instance form elements array.
 */
function manualcrop_field_widget_settings_form_alter(&$form, $settings) {
  if ($form['widget']['type']['#value'] == 'image_image' && isset($settings['preview_image_style'])) {
    // Disable the image field preview if the thumblist option is enabled.
    $form['widget']['settings']['preview_image_style']['#states'] = array(
      'disabled' => array(
        'input[name="instance[widget][settings][manualcrop_enable]"]' => array('checked' => TRUE),
        'input[name="instance[widget][settings][manualcrop_thumblist]"]' => array('checked' => TRUE),
      ),
    );
  }

  if (module_exists('insert') && isset($form['settings']['insert'])) {
    // Get the reference to the insert fieldset.
    $insert = &$form['settings']['insert'];

    // Make sure the insert checkbox stays on top.
    $insert['insert']['#weight'] = -2;

    // Add an option to exclude image styles without a Manual Crop effect.
    $insert['manualcrop_filter_insert'] = array(
      '#type' => 'checkbox',
      '#title' => t('Only Manual Crop styles'),
      '#description' => t('Enable this option to filter out all image styles without a Manual Crop effect.'),
      '#default_value' => !empty($settings['manualcrop_filter_insert']),
      '#weight' => -1,
      '#states' => array(
        'disabled' => array(
          'input[name="instance[widget][settings][manualcrop_enable]"]' => array('checked' => FALSE),
        ),
      )
    );

    // If checked, remove all non-Manual Crop styles.
    if (!empty($settings['manualcrop_enable']) && !empty($settings['manualcrop_filter_insert'])) {
      $styles = array_merge(array('image' => 0), manualcrop_insert_styles());

      $insert['insert_styles']['#options'] = array_intersect_key($insert['insert_styles']['#options'], $styles);
      $insert['insert_default']['#options'] = array_intersect_key($insert['insert_default']['#options'], $styles);
    }
  }
}

/**
 * Checkboxes element validation callback used to filter all unselected items
 * from the #value array.
 */
function manualcrop_checkboxes_filter($element, &$form_state, $form) {
  if (isset($element['#value'])) {
    form_set_value($element, array_filter($element['#value']), $form_state);
  }
}
