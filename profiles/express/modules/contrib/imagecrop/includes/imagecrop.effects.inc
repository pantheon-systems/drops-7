<?php
/**
 * @file
 * Registry for the image style effects from imagecrop.
 *
 */

/**
 * Implements hook_image_effect_info().
 */
function imagecrop_image_effect_info() {

  $effects = array();

  $effects['imagecrop_javascript'] = array(
    'label' => t('Javascript crop'),
    'help' => t('Create a crop with a javascript toolbox.'),
    'effect callback' => 'imagecrop_effect',
    'form callback' => 'imagecrop_effect_form',
    'summary theme' => 'imagecrop_effect_summary',
  );

  $effects['imagecrop_reuse'] = array(
    'label' => t('Reuse a javascript crop selection'),
    'help' => 'Reuse crop selection from another javascript crop preset.',
    'effect callback' => 'imagecrop_reuse_effect',
    'form callback' => 'imagecrop_reuse_form',
    'summary theme' => 'theme_imagecrop_reuse',
  );

  return $effects;

}

/**
 * Image effect callback: Crop the image based on the javascript selected crop.
 *
 * @param $image An image object returned by image_load().
 * @param $data An array of settings from the user for cropping the image.
 * @return TRUE on success. FALSE on failure to crop the image.
 *
 */
function imagecrop_effect(&$image, $data) {

  try {

    $imagecrop = new ImageCrop();
    $imagecrop->loadFile($image->source, FALSE);

    // if a global presetid is been set, it meens the image is generated from the imagecrop module
    if (isset($GLOBALS['imagecrop_style'])) {
      $style_name = $GLOBALS['imagecrop_style'];
    }
    // and if not, then get the id from list of all presets
    else {
      $style_name = imagecrop_get_style_name_from_url();
    }

    $imagecrop->setImageStyle($style_name);
    $imagecrop->loadCropSettings();
    $imagecrop->applyCrop($image);

  }
  catch (Exception $e) {
    watchdog($e->getMessage(), 'error');
    return FALSE;
  }

  return TRUE;

}

/**
 * Settings form for configuring a javascript imagecrop effect.
 */
function imagecrop_effect_form($data) {

  $form = image_resize_form($data);

  $form['xoffset'] = array(
    '#type' => 'textfield',
    '#title' => t('X offset'),
    '#default_value' => isset($data['xoffset']) ? $data['xoffset'] : '',
    '#description' => t('Enter an offset in pixels (without px) or use a keyword: <em>left</em>, <em>center</em>, or <em>right</em>.'),
    '#element_validate' => array('imagecrop_validate_offset'),
  );

  $form['yoffset'] = array(
    '#type' => 'textfield',
    '#title' => t('Y offset'),
    '#default_value' => isset($data['yoffset']) ? $data['yoffset'] : '',
    '#description' => t('Enter an offset in pixels (without px) or use a keyword: <em>top</em>, <em>center</em>, or <em>bottom</em>.'),
    '#element_validate' => array('imagecrop_validate_offset'),
  );

  $form['resizable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Resizable toolbox'),
    '#default_value' => isset($data['resizable']) ? $data['resizable'] : '',
    '#description' => t('If the toolbox is resized, the crop values won\'t be respected, so you should add a Scale action after the ImageCrop.'),
  );

  $form['downscaling'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not allow down scaling'),
    '#default_value' => isset($data['downscaling']) ? $data['downscaling'] : FALSE,
    '#description' => t('If checked, you can\'t resize the toolbox smaller than width and height.'),
  );

  $description = t('Enter an aspect ratio to preserve during resizing. This can take one of the following formats:');
  $description .= '<ul><li>' . t('A float (like 0.5 or 2).') . '</li>';
  $description .= '<li>' . t('The string \'KEEP\'. This will constrain the aspect ratio to that of the original image.') . '</li>';
  $description .= '<li>' . t('The string \'CROP\'. This will constrain the aspect ratio to the dimensions set above.') . '</li></ul>';
  $description .= t('Leave blank for no aspect ratio constraints.');

  $form['aspect_ratio'] = array(
    '#type' => 'textfield',
    '#title' => t('Aspect ratio'),
    '#default_value' => isset($data['aspect_ratio']) ? $data['aspect_ratio'] : '',
    '#description' => $description,
    '#element_validate' => array('imagecrop_validate_aspect'),
  );

  $form['disable_if_no_data'] = array(
    '#type' => 'checkbox',
    '#title' => t('Don\'t crop if cropping region wasn\'t set.'),
    '#default_value' => isset($data['disable_if_no_data']) ? $data['disable_if_no_data'] : FALSE,
  );

  return $form;

}

/**
 * Reset the cropped images when the reset checkbox is enabled.
 */
function imagecrop_javascript_effect_submit($form, &$form_state) {

  if ($form_state['values']['reset-crops'] === 1) {
    db_update('image_crop_settings')
      ->fields(array('width' => $form_state['values']['data']['width'], 'height' => $form_state['values']['data']['height']))
      ->condition('width', $form_state['values']['old-width'])
      ->condition('height', $form_state['values']['old-height'])
      ->condition('style_name', $form_state['image_style']['name'])
      ->execute();
  }

}

/**
 * Validation function to validate an entered offset value. (numbers or left / center / right)
 */
function imagecrop_validate_offset($element, &$form_state) {

  if ($element['#value'] == '') {
    return;
  }

  // numbers are allowed.
  if (is_numeric($element['#value'])) {
    return;
  }

  // if the value is a string, check on allowed strings
  if ($element['#value'] != 'center' && $element['#value'] != 'left' && $element['#value'] != 'right') {
    form_error($element, t('@name must be a correct offset value', array('@name' => $element['#title'])));
  }

}

/**
 * Validation function to validate an entered aspect value.
 */
function imagecrop_validate_aspect($element, &$form_state) {

  if ($element['#value'] == '') {
    return;
  }

  // numbers are allowed.
  if (is_numeric($element['#value'])) {
    return;
  }

  // if the value is a string, check on allowed strings
  if ($element['#value'] != 'KEEP' && $element['#value'] != 'CROP') {
    form_error($element, t('@name must be a correct aspect value', array('@name' => $element['#title'])));
  }

}

/**
 * Settings form for reüsing a crop selection.
 */
function imagecrop_reuse_form($data) {

  $presets = get_imagecrop_styles();
  // Make sure people don't select current preset.
  if ($key = array_search(arg(5), $presets)) {
    unset($presets[$key]);
  }

  if (count($presets) > 0) {
    $form['imagecrop_sid'] = array(
      '#title' => t('Use the crop settings from'),
      '#type' => 'select',
      '#options' => $presets,
      '#default_value' => isset($data['imagecrop_sid']) ? $data['imagecrop_sid'] : '',
    );
  }
  else {
    $form['imagecrop_warning'] = array(
      '#value' => t('No image style is found with the javascript_crop action so far. If you want to take advantage of this module, you will need to create at least one image style with that action.'),
    );
  }

  return $form;

}

/**
 * Image effect callback: Crop the image based on the image style.
 *
 * @param $image An image object returned by image_load().
 * @param $data An array with the preset to use..
 * @return TRUE on success. FALSE on failure to crop the image.
 *
 */
function imagecrop_reuse_effect(&$image, $data) {

  if (empty($data['imagecrop_sid'])) {
    return FALSE;
  }

  // Load selected image style and apply the imagecrop_javascript action.
  $style = image_style_load(NULL, $data['imagecrop_sid']);
  foreach ($style['effects'] as $effect) {
    if ($effect['name'] == 'imagecrop_javascript') {
      $GLOBALS['imagecrop_style'] = $style['name'];
      image_effect_apply($image, $effect);
      return TRUE;
    }
  }

  return FALSE;

}