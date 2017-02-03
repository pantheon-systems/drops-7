<?php
/**
 * @file
 * Hooks for Maintenance and Install pages.
 */

/**
 * Implements template_preprocess_maintenance_page().
 */
function radix_preprocess_maintenance_page(&$variables) {
  global $install_state;

  if ($install_state) {
    // Find the number of tasks to run.
    $tasks = install_tasks_to_display($install_state);
    $total = sizeof($tasks);

    // Find the position of the active task.
    $keys = array_keys($tasks);
    $active_task = $install_state['active_task'];
    $current = array_search($active_task, $keys) + 1;

    // Show steps.
    $variables['steps'] = t('Step @current of @total', array(
      '@current' => $current,
      '@total' => $total,
    ));

    $variables['title'] = $tasks[$active_task];
  }

  // Load some config from the profile.
  $profile = isset($_GET['profile']) ? $_GET['profile'] : '';
  if ($profile) {
    // Get info from profile.info.
    $path = drupal_get_path('profile', $profile);
    $info_file = $path . '/' . $profile . '.info';
    $info = drupal_parse_info_file($info_file);

    // Load the site name, version and logo from info file.
    $variables['site_name'] = $info['name'];
    $variables['logo'] = drupal_get_path('profile', $profile) . '/' . $profile . '.png';

    // Quick fix to add the required radix-progress.js.
    $radix_path = drupal_get_path('theme', 'radix');
    drupal_add_js($radix_path . '/assets/javascripts/radix-progress.js');
  }
}

/**
 * Implements theme_task_list().
 */
function radix_task_list($variables) {
  $items = $variables['items'];
  $active = $variables['active'];

  $done = isset($items[$active]) || $active == NULL;
  $output = '<h2 class="element-invisible">Installation tasks</h2>';
  $output .= '<div class="tasks-list">';

  $step = 1;
  foreach ($items as $k => $item) {
    if ($active == $k) {
      $class = 'active';
      $status = '(' . t('active') . ')';
      $done = FALSE;
    }
    else {
      $class = $done ? 'done' : '';
      $status = $done ? '(' . t('done') . ')' : '';
    }

    $output .= '<div';
    $output .= ($class ? ' class="' . $class . '"' : '') . '>';
    $output .= '<h6 class="text-muted">' . st('Step @step', array('@step' => $step)) . '</h6>';
    $output .= '<h4>' . $item . '</h4>';
    $output .= ($status ? '<span class="element-invisible">' . $status . '</span>' : '');
    $output .= '</div>';

    $step++;
  }
  $output .= '</div>';
  return $output;
}