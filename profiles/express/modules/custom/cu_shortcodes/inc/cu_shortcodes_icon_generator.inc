<?php

/**
 * @file
 * Programmatically create VCR content. Oh Yeah!
 *
 * Will create pages, menus and menu items, beans and more.
 * More power!
 */

/**
 * Implements hook_form().
 */
function cu_shortcodes_icon_generator_form($form, &$form_state) {
  // Add some css so the admin looks nice.
  drupal_add_css(drupal_get_path('module', 'cu_shortcodes') . '/css/cu_shortcodes_admin.css');
  drupal_add_css(drupal_get_path('theme', 'cu_omega') . '/css/flat-buttons.css');

  // Fieldset for options.
  $form['icon_shortcode_options'] = array(
    '#title' => t('Button Options'),
    '#type' => 'fieldset',
  );

  // Icon Color Options.
  $form['icon_shortcode_options']['icon_shortcode_color'] = array(
    '#title' => t('Icon Color'),
    '#type' => 'radios',
    '#default_value' => 'black',
    '#description' => t('Choose a color for the icon.'),
    '#options' => array(
      'gold' => t('Gold'),
      'gray' => t('Gray'),
      'black' => t('Black'),
      'white' => t('White'),
    ),
  );

  // Icon Size Options.
  $form['icon_shortcode_options']['icon_shortcode_size'] = array(
    '#title' => t('Icon Size'),
    '#type' => 'radios',
    '#default_value' => '',
    '#description' => t('Choose a size for the icon.'),
    '#options' => array(
      '' => t('Small'),
      'medium' => t('Medium'),
      'large' => t('Large'),
      'xlarge' => t('X-Large'),
    ),
  );

  // Icons.
  $form['icon_shortcode_options']['icons'] = array(
    '#type' => 'fieldset',
    '#title' => t('Button Icons'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['icon_shortcode_options']['icons']['icon_shortcode_icon'] = array(
    '#type' => 'radios',
    '#title' => '',
    '#options' => cu_font_awesome_icon_list(),
    '#default_value' => 'fa-pencil',
  );

  // Build the values array from submitted values, otherwise, use default
  // values.
  if (isset($form_state['values'])) {
    $values = array(
      'color' => $form_state['values']['icon_shortcode_color'],
      'icon' => $form_state['values']['icon_shortcode_icon'],
      'size' => $form_state['values']['icon_shortcode_size'],
    );
  }
  else {
    $values = array(
      'color' => $form['icon_shortcode_options']['icon_shortcode_color']['#default_value'],
      'icon' => $form['icon_shortcode_options']['icons']['icon_shortcode_icon']['#default_value'],
      'size' => $form['icon_shortcode_options']['icon_shortcode_size']['#default_value'],
    );
  }

  // Display the shortcode.
  $form['generated_shortcode'] = array(
    '#title' => t('Icon Shortcode'),
    '#type' => 'fieldset',
    '#prefix' => '<div id="generated-icon-shortcode">',
    '#suffix' => '</div>',
    '#description' => '<p>Copy the shortcode below and paste it in a WYSIWYG field:</p> <textarea rows=5 cols=100>' . _cu_shortcodes_generate_icon($values) . '</textarea><p>' . _cu_shortcodes_generate_icon_rendered($values) . '</p>',
    '#weight' => 3,
  );

  // Submit/Generate
  $form['icon_shortcode_options']['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Shortcode'),
    '#ajax' => array(
      'callback' => 'cu_shortcodes_icon_generator_callback',
      'wrapper' => 'generated-icon-shortcode',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
  $form['#submit'][] = 'cu_shortcodes_generate_form_submit';
  return $form;
}

/**
 * Implements hook_form().
 */
function cu_shortcodes_icon_generator_callback($form, $form_state) {
  return $form['generated_shortcode'];
}

/**
 * Implements hook_form().
 */
function cu_shortcodes_generate_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Implements hook_form().
 */
function _cu_shortcodes_generate_icon($values) {
  $icon = $values['icon'] ? ' icon=' . $values['icon'] : '';
  $size = $values['size'] ? ' size=' . $values['size'] : '';
  $style = $values['style'] ? ' style=' . $values['style'] : '';
  $shortcode = '[icon shape="' . $values['icon'] . '" color=' . $values['color']  . $size   . ' /]';
  return $shortcode;
}

/**
 * Implements hook_form().
 */
function _cu_shortcodes_generate_icon_rendered($values) {
  $classes = array();
  $classes[] = $values['icon'];
  $classes[] = 'icon-size-' . $values['size'];
  $classes[] = 'icon-color-' . $values['color'];
  $classes[] = !empty($class) ? explode(' ', $class) : NULL;
  $classes = trim(implode(' ', $classes));
  $icon = '<i class="fa ' . $classes . '"></i>';
  return $icon;
}
