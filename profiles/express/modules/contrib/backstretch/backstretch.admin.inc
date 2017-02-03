<?php

/**
 * Page callback for admin/appearance/backstretch.
 */
function backstretch_admin_settings() {
  $form = array();
  $form['backstretch_image_url'] = array(
    '#title' => t('Image URL'),
    '#description' => t('URL of picture to display as backstretched background (begin with / if local to drupal). Leave blank for none.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('backstretch_image_url', ''),
  );
  $form['advanced'] = array(
    '#title' => t('Advanced'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advanced']['backstretch_min_width'] = array(
    '#title' => t('Minimum width'),
    '#description' => t('Window width (in px) must be greater than this value in order for backstretch to apply. This allows you to prevent large images from being loaded on to small devices.'),
    '#type' => 'textfield',
    '#default_value' => variable_get('backstretch_min_width', 0),
  );
  $form['scroller'] = array(
    '#title' => t('Scroller'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Backstretch Scroller enables you to scroll beyond the normal limits of the page in order to view the entire image without the page content.'),
  );
  $form['scroller']['backstretch_scroller'] = array(
    '#title' => t('Enable Backstretch Scroller'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('backstretch_scroller', FALSE),
  );
  $form['scroller']['backstretch_scroller_adjustment'] = array(
    '#title' => t('Scroller adjustment'),
    '#description' => t('If desired, this allows you keep part of your page\'s content from scrolling off the screen - useful to keep the footer in view (almost always a negative number).'),
    '#type' => 'textfield',
    '#default_value' => variable_get('backstretch_scroller_adjustment', 0),
  );
  
  $waypoints = libraries_load('waypoints');
  $scrollto = libraries_load('scrollto');
  
  if (!empty($waypoints['loaded']) && !empty($scrollto['loaded'])) {
    $form['scroller']['backstretch_scrollto'] = array(
      '#title' => t('Enable Backstretch ScrollTo'),
      '#type' => 'checkbox',
      '#default_value' => variable_get('backstretch_scrollto', FALSE),
    );
  }
  else {
    $form['scroller']['scrollto_disabled'] = array(
      '#markup' => t('Note: You can enable the provided <em>ScrollTo</em> functionality by downloading the jQuery ScrollTo and Waypoints libraries.'),
    );
  }
  return system_settings_form($form);
}
