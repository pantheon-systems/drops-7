<?php
/**
 * @file
 * Theme and preprocess functions for admin.
 */

/**
 * Implements theme_user_admin_permissions().
 *
 * Basically it just removes the 'checkbox' classes to avoid conflicts with Bootstrap.
 */
function radix_user_admin_permissions($variables) {
  $form = $variables['form'];

  $roles = user_roles();
  foreach (element_children($form['permission']) as $key) {
    $row = array();
    // Module name
    if (is_numeric($key)) {
      $row[] = array('data' => drupal_render($form['permission'][$key]), 'class' => array('module'), 'id' => 'module-' . $form['permission'][$key]['#id'], 'colspan' => count($form['role_names']['#value']) + 1);
    }
    else {
      // Permission row.
      $row[] = array(
        'data' => drupal_render($form['permission'][$key]),
        'class' => array('permission'),
      );
      foreach (element_children($form['checkboxes']) as $rid) {
        $form['checkboxes'][$rid][$key]['#title'] = $roles[$rid] . ': ' . $form['permission'][$key]['#markup'];
        $form['checkboxes'][$rid][$key]['#title_display'] = 'invisible';
        $row[] = array('data' => drupal_render($form['checkboxes'][$rid][$key]));
      }
    }
    $rows[] = $row;
  }
  $header[] = (t('Permission'));
  foreach (element_children($form['role_names']) as $rid) {
    $header[] = array('data' => drupal_render($form['role_names'][$rid]));
  }
  $output = theme('system_compact_link');
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'permissions')));
  $output .= drupal_render_children($form);
  return $output;
}

/**
 * Implements template_preprocess_views_ui_display_tab_setting().
 */
function radix_preprocess_views_ui_display_tab_setting(&$variables) {
  $variables['settings_links'] = str_replace('class="label"', 'class="views-admin-label"', $variables['settings_links']);
}

/**
 * Returns HTML for an administrative block for display.
 *
 * @ingroup themeable
 */
function radix_admin_block($variables) {
  $block = $variables['block'];
  $output = '';

  // Don't display the block if it has no content to display.
  if (empty($block['show'])) {
    return $output;
  }

  $output .= '<div class="panel panel-default">';
  if (!empty($block['title'])) {
    $output .= '<div class="panel-heading"><h3 class="panel-title">' . $block['title'] . '</h3></div>';
  }
  if (!empty($block['content'])) {
    $output .= $block['content'];
  }
  else {
    $output .= '<div class="description">' . $block['description'] . '</div>';
  }
  $output .= '</div>';

  return $output;
}

/**
 * Returns HTML for the content of an administrative block.
 *
 * @ingroup themeable
 */
function radix_admin_block_content($variables) {
  $content = $variables['content'];
  $output = '';

  if (!empty($content)) {
    $class = 'admin-list list-group';
    if ($compact = system_admin_compact_mode()) {
      $class .= ' compact';
    }
    $output .= '<ul class="' . $class . '">';
    foreach ($content as $item) {
      $output .= '<li class="list-group-item">';
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      if (!$compact && isset($item['description'])) {
        $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
      }
      $output .= '</li>';
    }
    $output .= '</ul>';
  }
  return $output;
}

/**
 * Returns HTML for an administrative page.
 *
 * @ingroup themeable
 */
function radix_admin_page($variables) {
  $blocks = $variables['blocks'];

  $stripe = 0;
  $container = array();

  foreach ($blocks as $block) {
    if ($block_output = theme('admin_block', array('block' => $block))) {
      if (empty($block['position'])) {
        // perform automatic striping.
        $block['position'] = ++$stripe % 2 ? 'left' : 'right';
      }
      if (!isset($container[$block['position']])) {
        $container[$block['position']] = '';
      }
      $container[$block['position']] .= $block_output;
    }
  }

  $output = '<div class="container clearfix">';
  $output .= '<div class="row">';
  $output .= '<div class="col-md-12">';
  $output .= '</div>';
  foreach ($container as $id => $data) {
    $output .= '<div class="' . $id . ' col-md-12 clearfix">';
    $output .= $data;
    $output .= '</div>';
  }
  $output .= '</div>';
  $output .= '</div>';
  $output .= theme('system_compact_link');
  return $output;
}

/**
 * Returns HTML for a link to show or hide inline help descriptions.
 *
 * @ingroup themeable
 */
function radix_system_compact_link() {
  $output = '<div class="compact-link clearfix">';
  $options = array(
    'query' => drupal_get_destination(),
    'attributes' => array(
      'class' => array('btn', 'btn-primary', 'btn-sm', 'pull-right'),
    ),
  );

  if (system_admin_compact_mode()) {
    $options['attributes']['title'] = t('Expand layout to include descriptions.');
    $output .= l(t('Show descriptions'), 'admin/compact/off', $options);
  }
  else {
    $options['attributes']['title'] = t('Compress layout by hiding descriptions.');
    $output .= l(t('Hide descriptions'), 'admin/compact/on', $options);
  }
  $output .= '</div>';

  return $output;
}
