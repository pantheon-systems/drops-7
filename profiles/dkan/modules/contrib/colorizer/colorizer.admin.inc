<?php

/**
 * @file
 * Colorize your theme Admin settings form
 *
 * We split the actual menu hook used in Colorizer into a series of
 * helper functions to allow Colorizer settings to be embedded elsewhere
 */

/**
 * This is a helper function that can be used to return the "subform"
 * of Colorizer settings to be used on any other page as needed
 *
 * $show_files determines if fields for template files is shown
 * $show_submit determines is a submit button is added to the form
 */
function colorizer_admin_form($show_files = TRUE, $show_submit = TRUE) {
  $base = drupal_get_path('module', 'colorizer');
  drupal_add_js($base . '/js/colorizer.js');

  $theme = $GLOBALS['theme'];
  $instance = $theme;
  // allow other modules to change the instance we are updating
  // allows for group-specific color instances rather than tying to theme
  drupal_alter('colorizer_instance', $instance);

  $css_file = variable_get('colorizer_cssfile', '');
  $inc_file = variable_get('colorizer_incfile', '');
  $preview_file = variable_get('colorizer_previewfile', '');
  if ($show_files) {

    $form['colorizer_global'] = array(
      '#type' => 'fieldset',
      '#title' => t('Global Settings'),
    );
    $form['colorizer_global']['colorizer_cssfile'] = array(
      '#type' => 'textfield',
      '#title' => t('CSS template'),
      '#default_value' => $css_file,
      '#size' => 30,
      '#description' => t('CSS file template (relative to theme path) to control theme colors.'),
    );
    $form['colorizer_global']['colorizer_incfile'] = array(
      '#type' => 'textfield',
      '#title' => t('Color.inc file'),
      '#default_value' => $inc_file,
      '#size' => 30,
      '#description' => t('Path to color.inc file (relative to theme path) defining color templates.'),
    );
    $form['colorizer_global']['colorizer_previewfile'] = array(
      '#type' => 'textfield',
      '#title' => t('Extra preview HTML file'),
      '#default_value' => $preview_file,
      '#size' => 30,
      '#description' => t('Path to extra preview HTML file (relative to theme path).'),
    );
  }

  $file = variable_get('colorizer_' . $instance . '_stylesheet', '');
  if (!empty($file)) {
    $form['colorizer_reset'] = array(
      '#type' => 'checkbox',
      '#title' => t('Reset colors to default'),
      '#description' => t('Check this option and Save configuration to remove custom colors cached for this theme.')
    );
  }

  $form['colorizer_colors'] = array(
    '#type' => 'fieldset',
    '#title' => t('Color Scheme Settings'),
  );

  $preview_file = drupal_get_path('theme', $theme) . '/' . $preview_file;
  if (file_exists($preview_file)) {
    $preview_data = file_get_contents($preview_file);
    $form['colorizer_colors']['preview'] = array(
      '#markup' => $preview_data,
      '#prefix' => "<div id='colorizer-preview'>",
      '#suffix' => '</div>',
      '#title' => t('Color Scheme Settings'),
    );
  }

  // Most of this copied from Color module
  $info = colorizer_get_info($theme, TRUE);
  if (!empty($info['schemes'])) {
    $info['schemes'][''] = array('title' => t('Custom'), 'colors' => array());

    $color_sets = array();
    $schemes = array();
    foreach ($info['schemes'] as $key => $scheme) {
      $color_sets[$key] = $scheme['title'];
      $schemes[$key] = $scheme['colors'];
      $schemes[$key] += $info['schemes']['default']['colors'];
    }

    // See if we're using a predefined scheme.
    // Note: we use the original theme when the default scheme is chosen.
    $current_scheme = variable_get('colorizer_' . $instance . '_palette', array());
    foreach ($schemes as $key => $scheme) {
      if ($current_scheme == $scheme) {
        $scheme_name = $key;
        break;
      }
    }
    if (empty($scheme_name)) {
      if (empty($current_scheme)) {
        $scheme_name = 'default';
      }
      else {
        $scheme_name = '';
      }
    }

    $file = drupal_get_path('theme', $theme) . '/' . $css_file;
    $style = '';

    if (file_exists($file)) {
      // Aggregate @imports recursively for each configured top level CSS file
      // without optimization. Aggregation and optimization will be
      // handled by drupal_build_css_cache() only.
      $style = drupal_load_stylesheet($file, FALSE);
    }

    // Add scheme selector.
    // we re-use files from the core color module here
    // hard code modules/color path since drupal_get_path only
    // returns for enabled modules
    $form['colorizer_colors']['scheme'] = array(
      '#type' => 'select',
      '#title' => t('Color scheme'),
      '#options' => $color_sets,
      '#default_value' => $scheme_name,
      '#prefix' => '<div class="colorizer-form clearfix">',
      '#attached' => array(
        // Add Farbtastic color picker.
        'library' => array(
          array('system', 'farbtastic'),
        ),
        // Add custom CSS.
        'css' => array(
          $base . '/css/colorizer_admin.css' => array(),
        ),
        // Add custom JavaScript.
        'js' => array(
          $base . '/js/colorizer.js',
          array(
            'data' => array(
              'color' => array(
                'reference' => colorizer_get_palette($theme, $instance, TRUE),
                'schemes' => $schemes,
              ),
              'gradients' => $info['gradients'],
              'css' => $style,
            ),
            'type' => 'setting',
          ),
        ),
      ),
    );

    // Add palette fields.
    $palette = colorizer_get_palette($theme, $instance);
    $names = $info['fields'];
    $form['colorizer_colors']['palette']['#tree'] = TRUE;
    $form['colorizer_colors']['palette']['#prefix'] = '<div id="palette" class="clearfix">';
    $form['colorizer_colors']['palette']['#suffix'] = '</div></div>';
    foreach ($palette as $name => $value) {
      if (isset($names[$name])) {
        $form['colorizer_colors']['palette'][$name] = array(
          '#type' => 'textfield',
          '#title' => check_plain($names[$name]),
          '#default_value' => $value,
          '#size' => 8,
        );
      }
    }
    $form['theme'] = array('#type' => 'value', '#value' => $theme);
    $form['instance'] = array('#type' => 'value', '#value' => $instance);
    $form['info'] = array('#type' => 'value', '#value' => $info);

  }
  elseif (!$show_files) {
    $form['colorizer_help'] = array(
      '#markup' => '<p>' . t('Configure the Colorizer module here: ') .
        l('admin/appearance/colorizer', 'admin/appearance/colorizer') . '</p>',
    );
  }

  if ($show_submit) {
    $form['actions'] = array(
      '#type' => 'actions',
    );
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    );
    $form['actions']['colorizer_clearcache'] = array(
      '#type' => 'submit',
      '#value' => t('Clear Colorizer Cache'),
    );
  }

  return $form;
}

/**
 * Menu callback for Appearance settings form for Colorizer
 */
function colorizer_admin_settings() {
  $form = colorizer_admin_form();
  return $form;
}

/**
 * Helper function to validate a form for colorizer fields
 * Copied from Color module
 */
function colorizer_admin_form_validate($form, &$form_state) {
  // Only accept hexadecimal CSS color strings to avoid XSS upon use.
  if (!empty($form_state['values']['palette'])) {
    foreach ($form_state['values']['palette'] as $key => $color) {
      if (!preg_match('/^#([a-f0-9]{3}){1,2}$/iD', $color)) {
        form_set_error('palette][' . $key, t('%name must be a valid hexadecimal CSS color value.',
          array('%name' => $form['colorizer_colors']['palette'][$key]['#title'])));
      }
    }
  }
}

/**
 * Form validation handler for colorizer_admin_settings().
 */
function colorizer_admin_settings_validate($form, &$form_state) {
  return colorizer_admin_form_validate($form, $form_state);
}

/**
 * Helper function to submit/save a colorizer form
 * Mostly copied from Color module
 */
function colorizer_admin_form_submit($form, &$form_state) {
  // Save the file locations
  if (!empty($form_state['values']['op']) && ($form_state['values']['op'] == $form_state['values']['colorizer_clearcache'])) {
    // clear the colorizer css cache
    colorizer_clearcache();
  }
  if (isset($form_state['values']['colorizer_cssfile'])) {
    variable_set('colorizer_cssfile', $form_state['values']['colorizer_cssfile']);
    variable_set('colorizer_incfile', $form_state['values']['colorizer_incfile']);
    variable_set('colorizer_previewfile', $form_state['values']['colorizer_previewfile']);
  }

  if (!empty($form_state['values']['colorizer_reset'])) {
    $instance = $form_state['values']['instance'];
    variable_del('colorizer_' . $instance . '_palette');
    // Delete old files.
    foreach (variable_get('colorizer_' . $instance . '_files', array()) as $file) {
      @drupal_unlink($file);
    }
    if (isset($file) && $file = dirname($file)) {
      @drupal_rmdir($file);
    }
    variable_del('colorizer_' . $instance . '_files');
    return;
  }

  // The rest of this is from Color
  // Get theme coloring info.
  if (!isset($form_state['values']['info'])) {
    return;
  }
  $theme = $form_state['values']['theme'];
  $instance = $form_state['values']['instance'];
  $info = $form_state['values']['info'];

  // Resolve palette.
  $palette = $form_state['values']['palette'];
  if ($form_state['values']['scheme'] != '') {
    foreach ($palette as $key => $color) {
      if (isset($info['schemes'][$form_state['values']['scheme']]['colors'][$key])) {
        $palette[$key] = $info['schemes'][$form_state['values']['scheme']]['colors'][$key];
      }
    }
    $palette += $info['schemes']['default']['colors'];
  }

  variable_set('colorizer_' . $instance . '_palette', $palette);

  colorizer_update_stylesheet($theme, $instance, $palette);
}

/**
 * Form submission handler for colorizer_admin_settings().
 */
function colorizer_admin_settings_submit($form, &$form_state) {
  return colorizer_admin_form_submit($form, $form_state);
}
