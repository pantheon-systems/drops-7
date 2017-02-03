<?php
/**
 * @file
 * Contains administrative forms and settings.
 */

/**
 * Delegated hook_menu for admin
 *
 * Since most of hook_menu is delegated to plugins anyway, the admin menu
 * is delegated here to reduce code weight.
 */
function panelizer_admin_hook_menu(&$items) {
  // Configure settings pages.
  $settings_base = array(
    'access arguments' => array('administer panelizer'),
    'file' => 'includes/admin.inc',
  );

  $items['admin/config/content/panelizer'] = array(
    'title' => 'Panelizer',
    'description' => 'Configure panelizer availability and defaults',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('panelizer_settings_page_form'),
    'type' => MENU_NORMAL_ITEM,
  ) + $settings_base;

  $items['admin/config/content/panelizer/%panelizer_handler/%/allowed'] = array(
    'title' => 'Allowed content',
    'page callback' => 'panelizer_allowed_content_page',
    'page arguments' => array(4, 5),
    'type' => MENU_CALLBACK,
    'weight' => -10,
  ) + $settings_base;

  $tabs_base = array(
    'access callback' => 'panelizer_has_no_choice_callback',
    'access arguments' => array(4, 5),
    'page arguments' => array(4, 5, 'default', ''),
    'type' => MENU_LOCAL_TASK,
    'file' => 'includes/admin.inc',
  );

  $items['admin/config/content/panelizer/%panelizer_handler/%'] = array(
    'title callback' => 'panelizer_default_title_callback',
    'title arguments' => array(4, 5),
    'title' => 'Settings',
    'page callback' => 'panelizer_default_settings_page',
    'weight' => -5,
    'type' => MENU_SUGGESTED_ITEM,
  ) + $tabs_base;

  $index = 0;
  foreach (panelizer_operations() as $path => $operation) {
    $items['admin/config/content/panelizer/%panelizer_handler/%/' . $path] = array(
      'title' => $operation['menu title'],
      'page callback' => $operation['admin callback'],
      'weight' => $index - 5,
      'type' => ($index === 0) ? MENU_DEFAULT_LOCAL_TASK : MENU_LOCAL_TASK,
    ) + $tabs_base;
    if (isset($operation['file'])) {
      $items['admin/config/content/panelizer/%panelizer_handler/%/' . $path]['file'] = $operation['file'];
    }
    if (isset($operation['file path'])) {
      $items['admin/config/content/panelizer/%panelizer_handler/%/' . $path]['file path'] = $operation['file path'];
    }
    $index++;
  }

  $subtabs_base = array(
    'access callback' => 'panelizer_administer_panelizer_default',
    'access arguments' => array(4, 5, 7),
    'page arguments' => array(4, 5, 7, ''),
    'type' => MENU_LOCAL_TASK,
    'file' => 'includes/admin.inc',
  );

  $items['admin/config/content/panelizer/%panelizer_handler/%/list/%'] = array(
    'title' => 'Settings',
    'page callback' => 'panelizer_default_settings_page',
    'title callback' => 'panelizer_default_name_title_callback',
    'type' => MENU_CALLBACK,
  ) + $subtabs_base;

  $index = 0;
  foreach (panelizer_operations() as $path => $operation) {
    $items['admin/config/content/panelizer/%panelizer_handler/%/list/%/' . $path] = array(
      'title' => $operation['menu title'],
      'page callback' => $operation['admin callback'],
      'weight' => $index - 5,
    ) + $subtabs_base;
    if (isset($operation['file'])) {
      $items['admin/config/content/panelizer/%panelizer_handler/%/list/%/' . $path]['file'] = $operation['file'];
    }
    if (isset($operation['file path'])) {
      $items['admin/config/content/panelizer/%panelizer_handler/%/list/%/' . $path]['file path'] = $operation['file path'];
    }
    $index++;
  }

  $items['admin/config/content/panelizer/%panelizer_handler/%/list/%/access'] = array(
    'title' => 'Access',
    'page callback' => 'panelizer_default_access_page',
    'weight' => -2,
  ) + $subtabs_base;

}

/**
 * Primary settings page.
 *
 * This settings page allows the administrator to select which node types
 * can be panelized, whether they have a default, and provides links to
 * edit those defaults.
 */
function panelizer_settings_page_form($form, &$form_state) {
  $form['entities'] = array('#tree' => TRUE);

  $plugins = panelizer_get_entity_plugins();
  foreach ($plugins as $entity_type => $plugin) {
    $entity_info = entity_get_info($entity_type);
    if ($entity_info) {
      if ($handler = panelizer_entity_plugin_get_handler($entity_type)) {
        $handler->settings_form($form, $form_state);
      }
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Format the output of the main settings form.
 *
 * We want our checkboxes to show up in a table.
 */
function theme_panelizer_settings_page_table($variables) {
  $element = $variables['element'];

  // Render the table
  $header = $element['#header'];
  $columns = $element['#columns'];

  $rows = array();
  foreach (element_children($element) as $bundle) {
    foreach (element_children($element[$bundle]) as $view_mode) {
      $row = array();
      foreach ($columns as $column) {
        $row[] = drupal_render($element[$bundle][$view_mode][$column]);
      }

      $rows[] = $row;
    }
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows));

  // Render everything else
  $output .= drupal_render_children($element);
  return $output;
}

function panelizer_settings_page_form_validate($form, &$form_state) {
  $plugins = panelizer_get_entity_plugins();
  foreach ($plugins as $entity_type => $plugin) {
    $entity_info = entity_get_info($entity_type);
    if ($entity_info) {
      if ($handler = panelizer_entity_plugin_get_handler($entity_type)) {
        $handler->settings_form_validate($form, $form_state);
      }
    }
  }
}

function panelizer_settings_page_form_submit($form, &$form_state) {
  $plugins = panelizer_get_entity_plugins();
  foreach ($plugins as $entity_type => $plugin) {
    $entity_info = entity_get_info($entity_type);
    if ($entity_info) {
      if ($handler = panelizer_entity_plugin_get_handler($entity_type)) {
        $handler->settings_form_submit($form, $form_state);
      }
    }
  }

  // Ensure that defaults get cached to recognize new settings immediately.
  drupal_flush_all_caches();
}

/**
 * Page to configure what content is available for a given node type.
 */
function panelizer_allowed_content_page($handler, $bundle) {
  // Old URLs would include a view mode, but allowed content does not
  // change for view modes, so make that version of the page not found.
  if (strpos($bundle, '.') !== FALSE) {
    return MENU_NOT_FOUND;
  }

  if (is_string($handler)) {
    $handler = panelizer_entity_plugin_get_handler($handler);
  }

  if (!$handler->is_panelized($bundle)) {
    return MENU_NOT_FOUND;
  }

  ctools_include('common', 'panels');
  return drupal_get_form('panels_common_settings', 'panelizer_' . $handler->entity_type . ':' . $bundle);
}

/**
 * Page to configure basic settings for a panelizer default.
 */
function panelizer_default_settings_page($handler, $bundle, $name, $view_mode) {
  if (is_string($handler)) {
    $handler = panelizer_entity_plugin_get_handler($handler);
  }

  if ($view_mode) {
    $bundle .= '.' . $view_mode;
    // We use a $mode copy of $view_mode because the presence of $view_mode
    // tells us later if we want fake tabs, but we still need to know the
    // actual view mode regardless of that.
    $mode = $view_mode;
  }
  else {
    // We are NOT testing for the . here because if this is missing
    // that is a sign of a bug and we want the notice.
    list(, $mode) = explode('.', $bundle);
  }

  $panelizer = $handler->get_default_panelizer_object($bundle, $name);
  if (empty($panelizer)) {
    return MENU_NOT_FOUND;
  }

  $form_state = array(
    'panelizer' => &$panelizer,
    'no_redirect' => TRUE,
    'view_mode' => $mode,
  );

  if ($handler->has_panel_choice($bundle)) {
    $form_state['has title'] = TRUE;
  }

  ctools_include('common', 'panelizer');
  $output = drupal_build_form('panelizer_settings_form', $form_state);
  if (!empty($form_state['executed'])) {
    ctools_include('export');
    ctools_export_crud_save('panelizer_defaults', $panelizer);
    drupal_set_message(t('The settings have been updated.'));
    drupal_goto($_GET['q']);
  }

  if ($view_mode && !$handler->has_panel_choice($bundle)) {
    $output = $handler->wrap_default_panelizer_pages($bundle, $output);
  }

  return $output;
}

/**
 * Delete a panelizer node panel from the database.
 */
function panelizer_delete_panelizer_defaults($panelizer) {
  if (!empty($panelizer->pnid)) {
    if (!empty($panelizer->did)) {
      panels_delete_display($panelizer->did);
    }
    db_delete('panelizer_defaults')
      ->condition('pnid', $panelizer->pnid)
      ->execute();
  }
}

/**
 * Page to configure what content is available for a given node type.
 */
function panelizer_default_context_page($handler, $bundle, $name, $view_mode) {
  if (is_string($handler)) {
    $handler = panelizer_entity_plugin_get_handler($handler);
  }

  if ($view_mode) {
    $bundle .= '.' . $view_mode;
  }

  $cache_key = $handler->entity_type . ':' . $bundle . ':' . $name;
  $panelizer = panelizer_context_cache_get('default', $cache_key);
  if (empty($panelizer)) {
    return MENU_NOT_FOUND;
  }

  $form_state = array(
    'panelizer' => &$panelizer,
    'panelizer type' => 'default',
    'cache key' => $cache_key,
    'no_redirect' => TRUE,
  );

  ctools_include('common', 'panelizer');
  $output = drupal_build_form('panelizer_default_context_form', $form_state);
  if (!empty($form_state['executed'])) {
    if (!empty($form_state['clicked_button']['#write'])) {
      drupal_set_message(t('The settings have been updated.'));
      ctools_include('export');
      ctools_export_crud_save('panelizer_defaults', $panelizer);
    }
    else {
      drupal_set_message(t('Changes have been discarded.'));
    }

    panelizer_context_cache_clear('default', $cache_key);
    drupal_goto($_GET['q']);
  }

  if ($view_mode && !$handler->has_panel_choice($bundle)) {
    $output = $handler->wrap_default_panelizer_pages($bundle, $output);
  }

  return $output;
}

/**
 * Pass through to the panels layout editor.
 */
function panelizer_default_layout_page($handler, $bundle, $name, $view_mode, $step = NULL, $layout = NULL) {
  if (is_string($handler)) {
    $handler = panelizer_entity_plugin_get_handler($handler);
  }

  if ($view_mode) {
    $bundle .= '.' . $view_mode;
  }

  $panelizer = $handler->get_default_panelizer_object($bundle, $name);
  if (empty($panelizer)) {
    return MENU_NOT_FOUND;
  }

  $display = $panelizer->display;
  $display->context = $handler->get_contexts($panelizer);

  if ($name == 'default' && $handler->has_default_panel($bundle)) {
    $path = 'admin/config/content/panelizer/' . $handler->entity_type . '/' . $bundle;
  }
  else {
    $path = 'admin/config/content/panelizer/' . $handler->entity_type . '/' . $bundle . '/list/' . $name ;
  }

  $form_state = array(
    'display' => $display,
    'wizard path' => $path . '/layout/%step',
    'allowed_layouts' => 'panelizer_' . $handler->entity_type . ':' . $bundle,
  );

  ctools_include('common', 'panelizer');
  $output = panelizer_change_layout_wizard($form_state, $step, $layout);
  if (!empty($form_state['complete'])) {
    $panelizer->display = $form_state['display'];
    ctools_export_crud_save('panelizer_defaults', $panelizer);
    drupal_set_message(t('The layout has been changed.'));
    drupal_goto($path . '/content');
  }

  if ($view_mode && !$handler->has_panel_choice($bundle)) {
    $output = $handler->wrap_default_panelizer_pages($bundle, $output);
  }

  return $output;
}

/**
 * Determine edit access for this panelizer default.
 */
function panelizer_default_access_page($handler, $bundle, $name, $view_mode) {
  if (is_string($handler)) {
    $handler = panelizer_entity_plugin_get_handler($handler);
  }

  if ($view_mode) {
    $bundle .= '.' . $view_mode;
  }

  $panelizer = $handler->get_default_panelizer_object($bundle, $name);
  if (empty($panelizer)) {
    return MENU_NOT_FOUND;
  }

  $argument = $name;

  ctools_include('context-access-admin');
  ctools_include('object-cache');

  // Ensure that if they visit this page fresh, any cached data is removed:
  if (empty($_POST)) {
    ctools_object_cache_clear('panelizer_access', $argument);
  }
  else {
    $access = ctools_object_cache_get('panelizer_access', $argument);
  }

  if (empty($access)) {
    $access = $panelizer->access;
  }

  if (!is_array($access)) {
    $access = array();
  }

  $form_state = array(
    'access' => $access,
    'module' => 'panelizer',
    'callback argument' => $argument,
    // A bug in context-access-admin requires this until it's fixed.
    'argument' => $argument,
    'contexts' => $handler->get_contexts($panelizer),
    'no_redirect' => TRUE,
  );

  $output = drupal_build_form('ctools_access_admin_form', $form_state);

  if (!empty($form_state['executed'])) {
    ctools_object_cache_clear('panelizer_access', $argument);
    $panelizer->access = $form_state['access'];
    ctools_export_crud_save('panelizer_defaults', $panelizer);

    drupal_set_message(t('The access settings have been updated.'));
    drupal_goto($_GET['q']);
  }

  return $output;
}

/**
 * Pass through to the panels content editor.
 */
function panelizer_default_content_page($handler, $bundle, $name, $view_mode) {
  if (is_string($handler)) {
    $handler = panelizer_entity_plugin_get_handler($handler);
  }

  if ($view_mode) {
    $bundle .= '.' . $view_mode;
  }

  $panelizer = $handler->get_default_panelizer_object($bundle, $name);
  if (empty($panelizer)) {
    return MENU_NOT_FOUND;
  }

  $cache_key = 'panelizer:default:' . $handler->entity_type . ':' . $bundle . ':' . $name;

  $form_state = array(
    'display cache' => panels_edit_cache_get($cache_key),
    'no_redirect' => TRUE,
  );

  ctools_include('common', 'panelizer');
  $output = drupal_build_form('panelizer_edit_content_form', $form_state);
  if (!empty($form_state['executed'])) {
    if (!empty($form_state['clicked_button']['#save-display'])) {
      drupal_set_message(t('The settings have been updated.'));
      $panelizer->display = $form_state['display'];
      ctools_export_crud_save('panelizer_defaults', $panelizer);
    }
    else {
      drupal_set_message(t('Changes have been discarded.'));
    }

    panels_edit_cache_clear($form_state['display cache']);
    drupal_goto($_GET['q']);
  }

  ctools_set_no_blocks(FALSE);

  if ($view_mode && !$handler->has_panel_choice($bundle)) {
    $output = $handler->wrap_default_panelizer_pages($bundle, $output);
  }

  drupal_set_page_content($output);
  $page = element_info('page');

  return $page;
}
