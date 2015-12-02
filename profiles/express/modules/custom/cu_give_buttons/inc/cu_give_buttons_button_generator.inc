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
function cu_give_buttons_button_generator_form($form, &$form_state) {
  // Add some css so the admin looks nice.
  drupal_add_css(drupal_get_path('theme', 'cu_omega') . '/css/flat-buttons.css');
  drupal_add_css(drupal_get_path('module', 'cu_give_buttons') . '/styles/give-button.css');

  // Fieldset for options.
  $form['button_shortcode_options'] = array(
    '#title' => t('Button Options'),
    '#type' => 'fieldset',
  );

  // Button Color Options.
  $form['button_shortcode_options']['button_shortcode_color'] = array(
    '#title' => t('Button Color'),
    '#type' => 'radios',
    '#default_value' => 'dark',
    '#description' => t('Choose a color for the button.'),
    '#options' => array(
      'dark' => t('Dark'),
      'light' => t('Light'),
      'gold' => t('Gold'),
    ),
  );

  // Button URL.
  $form['button_shortcode_options']['button_shortcode_url'] = array(
    '#title' => t('Button URL'),
    '#type' => 'textfield',
    '#default_value' => 'http://www.colorado.edu',
    '#description' => t('The url the button will link to.'),
  );

  // Button Text.
  $form['button_shortcode_options']['button_shortcode_text'] = array(
    '#title' => t('Button Text'),
    '#type' => 'textfield',
    '#default_value' => 'Click Here',
    '#description' => t('The text displayed on the button.'),
  );

  // Button Size Options.
  $form['button_shortcode_options']['button_shortcode_size'] = array(
    '#title' => t('Button Size'),
    '#type' => 'radios',
    '#default_value' => '',
    '#description' => t('Choose a size for the button.'),
    '#options' => array(
      '' => t('Regular'),
      'large' => t('Large'),
      'small' => t('Small'),
    ),
  );

  // Button Style Options.
  $form['button_shortcode_options']['button_shortcode_style'] = array(
    '#title' => t('Button Style'),
    '#type' => 'radios',
    '#default_value' => '',
    '#description' => t('Choose a style for the button.'),
    '#options' => array(
      '' => t('Regular'),
      'full' => t('Full Width'),
    ),
  );

  
  // Build the values array from submitted values, otherwise, use default
  // values.
  if (isset($form_state['values'])) {
    $values = array(
      'color' => $form_state['values']['button_shortcode_color'],
      'url' => $form_state['values']['button_shortcode_url'],
      'text' => $form_state['values']['button_shortcode_text'],
      'size' => $form_state['values']['button_shortcode_size'],
      'style' => $form_state['values']['button_shortcode_style'],
    );
  }
  else {
    $values = array(
      'color' => $form['button_shortcode_options']['button_shortcode_color']['#default_value'],
      'url' => $form['button_shortcode_options']['button_shortcode_url']['#default_value'],
      'text' => $form['button_shortcode_options']['button_shortcode_text']['#default_value'],
      'size' => $form['button_shortcode_options']['button_shortcode_size']['#default_value'],
      'style' => $form['button_shortcode_options']['button_shortcode_style']['#default_value'],
    );
  }

  // Display the shortcode.
  $form['generated_shortcode'] = array(
    '#title' => t('Button Shortcode'),
    '#type' => 'fieldset',
    '#prefix' => '<div id="generated-button-shortcode">',
    '#suffix' => '</div>',
    '#description' => '<p>Copy the shortcode below and paste it in a WYSIWYG field:</p> <textarea rows=5 cols=100>' . _cu_give_buttons_generate_button($values) . '</textarea><p>' . _cu_give_buttons_generate_button_rendered($values) . '</p>',
    '#weight' => 3,
  );

  // Submit/Generate
  $form['button_shortcode_options']['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Generate Shortcode'),
    '#ajax' => array(
      'callback' => 'cu_give_buttons_button_generator_callback',
      'wrapper' => 'generated-button-shortcode',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );
  $form['#submit'][] = 'cu_give_buttons_generate_form_submit';
  return $form;
}

/**
 * Implements hook_callback().
 */
function cu_give_buttons_button_generator_callback($form, $form_state) {
  return $form['generated_shortcode'];
}

/**
 * Implements hook_submit().
 */
function cu_give_buttons_generate_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Generate button.
 */
function _cu_give_buttons_generate_button($values) {
  $icon = $values['icon'] ? ' icon=' . $values['icon'] : '';
  $size = $values['size'] ? ' size=' . $values['size'] : '';
  $style = $values['style'] ? ' style=' . $values['style'] : '';
  $shortcode = '[give url="' . $values['url'] . '" color=' . $values['color']  . $icon . $size . $style . ']' . $values['text'] . '[/give]';
  return $shortcode;
}

/**
 * Button rendered function.
 */
function _cu_give_buttons_generate_button_rendered($values) {
  $classes = array();
  $classes[] = 'button-shortcode';
  $classes[] = 'cu-give-button';
  $classes[] = $values['size'] ? 'button-' . $values['size'] : '';
  if ($values['style'] == 'full') {
    $classes[] = 'button-full';
  }
  $classes[] = $values['color'] ? 'cu-give-button-' . $values['color'] : 'cu-give-button-dark';
  $content = '<i class="icon-cu"></i> ' . $values['text'];
  $button = l($content, $values['url'], array(
              'attributes' => array(
                'class' => $classes,
              ),
              'html' => TRUE,
            ));
  return $button;
}
