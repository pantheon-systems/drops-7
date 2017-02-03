<?php

/**
 * @file
 * Administration pages and forms for beautytips manager.
 */

/**
 * Listing of all beautytips available.
 */
function beautytips_manager_tips_manager_page() {
  $rows = array();
  $empty = '';
  $header = array(t('Element'), t('Style'), t('Status'), t('Visibility'), t('Pages'), t('operations') ,'');
  $tips = beautytips_manager_get_custom_tips();
  if (count($tips)) {
    $visibility = array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.'));
    foreach ($tips as $tip) {
      $tip->pages = check_plain($tip->pages);
      $pages = ($tip->pages != substr($tip->pages, 0, 40)) ? substr($tip->pages, 0, 40) . '...' : substr($tip->pages, 0, 40);
      $rows[$tip->id]['element'] = check_plain($tip->element);
      $rows[$tip->id]['style'] = $tip->style;
      $rows[$tip->id]['enabled'] = $tip->enabled ? t('Enabled') : t('Disabled');
      $rows[$tip->id]['visibility'] = $visibility[$tip->visibility];
      $rows[$tip->id]['pages'] = $pages;
      $rows[$tip->id]['edit'] = l(t('edit'), "admin/config/user-interface/beautytips/custom-tips/$tip->id/edit");
      $rows[$tip->id]['delete'] = l(t('delete'), "admin/config/user-interface/beautytips/custom-tips/$tip->id/delete");
    }
  }
  else {
    $empty = t('There are no custom beautytips yet.');
  }
  return theme('table', array('header' => $header, 'rows' => $rows)) . $empty;
}

/**
 * Page callback for custom styles administration.
 */
function beautytips_manager_styles_manager_page() {
  $rows = array();
  $empty = '';
  $header = array(t('Name'), t('operations'), '');
  $styles = beautytips_manager_get_custom_styles();
  if (count($styles)) {
    $visibility = array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.'));
    foreach ($styles as $style) {
      $name = check_plain($style->name);
      unset($style->name);
      $rows[$style->id]['name'] = '<span class="bt-style-' . $name . '">' . $name . '</span>';
      $rows[$style->id]['edit'] = l(t('edit'), "admin/config/user-interface/beautytips/custom-styles/$style->id/edit");
      if ($name != variable_get('beautytips_default_style', 'plain')) {
        $rows[$style->id]['delete'] = l(t('delete'), "admin/config/user-interface/beautytips/custom-styles/$style->id/delete");
      }
      else {
        $rows[$style->id]['delete'] = t('Default style');
      }

      $options = array(
        'cssSelect' => 'td .bt-style-' . $name,
        'text' => t('<h2>Default Text</h2><p>Nam magna enim, accumsan eu, blandit sed, blandit a, eros.  Nam ante nulla, interdum vel, tristique ac, condimentum non, tellus.</p><p>Nulla facilisi. Nam magna enim, accumsan eu, blandit sed, blandit a, eros.</p>'),
        'trigger' => 'hover',
        'style' => $name,
        //'shrinkToFit' => TRUE,
      );
      beautytips_add_beautytips(array($name => $options));
    }
  }
  else {
    $empty = t('There are no custom beautytip styles yet.');
  }
  return theme('table', array('header' => $header, 'rows' => $rows)) . $empty;
}

/**
 * Form for editing a custom beautytip style.
 */
function beautytips_manager_custom_styles_form($form, &$form_state, $id = NULL) {
  
  if (!is_null($id)) {
    $style = beautytips_manager_get_custom_style($id);
    $style_map = beautytips_manager_style_mapping();
    $style_options = $style_map['options'];
    $css_style_options = $style_map['css_options'];
  }

  $form = array();
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Style Name'),
    '#description' => t('It must contain only alphanumeric characters and underscores.'),
    '#default_value' => isset($style->name) ? $style->name : '',
  );
  $form['style'] = array(
    '#type' => 'value',
    '#value' => isset($style) ? $style : NULL,
  );
  // TODO: Add this into mapping
  $style_info = array(
    'fill' => t('background color (string - html color)'),
    'strokeWidth' => t('width of border (integer)'),
    'strokeStyle' => t('color of border (string - html color)'),
    'width' => t('width of popup (number with px or em)'),
    'padding' => t('space between content and border (number with px em)'),
    'cornerRadius' => t('Controls roundness of corners (integer)'),
    'spikeGirth' => t('thickness of spike (integer)'),
    'spikeLength' => t('length of spike (integer)'),
    'shadowBlur' => t('Size of popup shadow (integer)'),
    'shadowColor' => t('Color of popup shadow (string - html color)'),
  );
  $form['custom_styles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom Style Options'),
    '#description' => t('<div id="beautytips-popup-changes"><div id="beauty-click-text"><p></p></div></div>'),
    '#attributes' => array('class' => array('bt-custom-styles')),
    '#tree' => TRUE,
  );
  foreach ($style_info as $option => $description) {
    $form['custom_styles'][$option] = array(
      '#title' => $option,
      '#description' => $description,
      '#type' => 'textfield',
      '#default_value' => (isset($style_options) && isset($style->{$style_options[$option]}) && !is_null($style->{$style_options[$option]})) ? $style->{$style_options[$option]} : '',
    );
  }
  $form['custom_styles']['shadow'] = array(
    '#title' => 'shadow',
    '#description' => t('Whether or not the popup has a shadow'),
    '#type' => 'radios',
    '#options' => array('default' => t('Default'), 'shadow' => t('Shadow On'), 'no_shadow' => t('Shadow Off')),
    '#attributes' => array('class' => array('beautytips-options-shadow')),
    '#default_value' => isset($style->shadow) ? $style->shadow : 'default',
  );
  $form['custom_styles']['cssClass'] = array(
    '#title' => 'cssClass',
    '#description' => t('The class that will be applied to the box wrapper div (of the TIP)'),
    '#type' => 'textfield',
    '#default_value' => isset($style->css_class) ? $style->css_class : '',
  );
  $css_style_info = array('color', 'fontFamily', 'fontWeight', 'fontSize');
  $form['custom_styles']['css-styles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Font Styling'),
    '#description' => t('Enter css options for changing the font style'),
    '#attributes' => array('class' => array('beautytips-css-styling')),
    '#collapsible' => FALSE,
  );
  foreach ($css_style_info as $option) {
    $form['custom_styles']['css-styles'][$option] = array(
      '#title' => $option,
      '#type' => 'textfield',
      '#default_value' => (isset($css_style_options) && isset($style->{$css_style_options[$option]}) && !is_null($style->{$css_style_options[$option]})) ? $style->{$css_style_options[$option]} : '',
    );
  }

  $path = drupal_get_path('module', 'beautytips');
  // TODO: This could be in a library
  drupal_add_js($path . '/other_libs/colorpicker/js/colorpicker.js');
  drupal_add_css($path . '/other_libs/colorpicker/css/colorpicker.css');
  beautytips_add_beautytips();
  drupal_add_js($path . '/js/bt_custom_styles.js');
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Validation callback on beautytips_manager_custom_styles_form.
 */
function beautytips_manager_custom_styles_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  if (empty($values['name'])) {
    form_set_error('name', t('You must name this custom style.'));
  }
  if (preg_match('/[^a-zA-Z0-9_]/', $values['name'])) {
    form_set_error('name', t('Style name must be alphanumeric or underscores only.'));
  }

  $integer_fields = array('strokeWidth' => 'style', 'cornerRadius' => 'style', 'spikeGirth' => 'style', 'spikeLength' => 'style', 'shadowBlur' => 'style'); 
  $pixel_fields = array('width' => 'style', 'padding' =>'style', 'fontSize' => 'css');

  // Validate fields that expect a number
  foreach ($integer_fields as $name => $type) {
    $value = $type == 'css' ? $values['custom_styles']['css-styles'][$name] : $values['custom_styles'][$name];
    if ($value) {
      if (!ctype_digit($value)) {
        $error_element = $type == 'css' ? 'custom_styles][css-styles][' . $name : 'custom_styles][' . $name;
        form_set_error($error_element, t('You need to enter an integer value for <em>@name</em>', array('@name' => $name)));
      }
    }
  }

  // Validate fields that expect a number and unit
  foreach ($pixel_fields as $name => $type) {
    $value = $type == 'css' ? $values['custom_styles']['css-styles'][$name] : $values['custom_styles'][$name];
    if ($value) {
      $unit = substr($value, -2, 2);
      $value = str_replace(array('px', ' ', 'em'), '', $value);
      if (!is_numeric($value) || (!$value && $value != 0) || !in_array($unit, array('px', 'em'))) {
        $error_element = $type == 'css' ? 'custom_styles][css-styles][' . $name : 'custom_styles][' . $name;
        form_set_error($error_element, t('You need to enter a numeric value for <em>@name</em>, followed by <em>px</em> or <em>em</em>', array('@name' => $name)));
      }
    }
  }

}

/**
 * Submission callback on beautytips_manager_custom_styles_form.
 */
function beautytips_manager_custom_styles_form_submit($form, &$form_state) {
  $style = new stdClass;
  $style->name = $form_state['values']['name'];
  $mapping = beautytips_manager_style_mapping();
  foreach ($form_state['values']['custom_styles'] as $custom_style => $value) {
    if (!is_array($value)) {
      $field = $custom_style;
      if (isset($mapping['options'][$custom_style])) {
        $style->{$mapping['options'][$custom_style]} = $value;
      }
    }
    else {
      if ($custom_style == 'css-styles') {
        foreach ($value as $css_style => $css_value) {
          if (isset($mapping['css_options'][$css_style])) {
            $style->{$mapping['css_options'][$css_style]} = $css_value;
          }
        }
      }
    }
  }
  if (!is_null($form_state['values']['style'])) {
    $style->id = $form_state['values']['style']->id;
  }
  beautytips_manager_save_custom_style($style);
  cache_clear_all('beautytips:beautytips-styles', 'cache');
  $form_state['redirect'] = 'admin/config/user-interface/beautytips/custom-styles';
}

/**
 * Confirm form for deleting a custom beautytip.
 */
function beautytips_manager_delete_tip_confirm_form($form, &$form_state, $id) {
  $tip = beautytips_manager_get_custom_tip($id);
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $question = t('Are you sure you want to the beautytip applied to element %element?', array('%element' => $tip->element));
  return confirm_form($form, $question, 'admin/config/user-interface/beautytips/custom-tips');
}

/**
 * Submit callback for beautytips delete confirm form.
 */
function beautytips_manager_delete_tip_confirm_form_submit($form, &$form_state) {
  beautytips_manager_delete_custom_tip($form_state['values']['id']);
  cache_clear_all('beautytips:beautytips-ui-custom-tips', 'cache');
  $form_state['redirect'] = 'admin/config/user-interface/beautytips/custom-tips';
}

/**
 * Page callback for style deletion form. 
 */
function beautytips_manager_delete_style_page($id) {
  $style = beautytips_manager_get_custom_style($id);
  if ($style->name == variable_get('beautytips_default_style', 'plain')) {
    return t('You cannot delete this style because is set as the default style.  You may change this <a href="@link">here</a>.', array('@link' => '/admin/config/user-interface/beautytips'));
  }

  return drupal_get_form('beautytips_manager_delete_style_confirm_form', $id);
  
}

/**
 * Confirm form for deleting a custom beautytip style.
 */
function beautytips_manager_delete_style_confirm_form($form, &$form_state, $id) {
  $style = beautytips_manager_get_custom_style($id);
  $form['id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );
  $question = t('Are you sure you want to the beautytip stlye %style?', array('%style' => $style->name));
  return confirm_form($form, $question, 'admin/config/user-interface/beautytips/custom-styles');
}

/**
 * Submit callback for beautytips delete confirm form.
 */
function beautytips_manager_delete_style_confirm_form_submit($form, &$form_state) {
  beautytips_manager_delete_custom_style($form_state['values']['id']);
  $form_state['redirect'] = 'admin/config/user-interface/beautytips/custom-styles';
}
