<?php

/**
 * @file
 * Adds options to ThemeKey's administration back end and alters Drupal forms
 * to add theme select boxes.
 *
 * @see themekey_ui.module
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'themekey') . '/themekey_base.inc';
require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'themekey_ui') . '/themekey_ui_helper.inc';

/**
 * ThemeKey UI settings form
 *
 * @see themekey_ui_settings_form_submit()
 *
 * @ingroup forms
 */
function themekey_ui_settings_form() {
  themekey_ui_theme_build_select_form(
    $form,
    t('Selectable Themes'),
    t('Select all the the themes that should be provided in theme selectors.'),
    variable_get('themekey_ui_selectable_themes', array('default')),
    NULL,
    TRUE,
    'themekey_ui_selectable_themes',
    list_themes(),
    FALSE);

  $form['themekey_ui_themes']['themekey_ui_selectable_themes']['#type'] = 'checkboxes';

  $form['themekey_ui'] = array(
    '#type' => 'fieldset',
    '#title' => t('UI Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  if (module_exists('path')) {
    $form['themekey_ui']['themekey_ui_pathalias'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show theme option in the \'URL aliases\' administration'),
      '#default_value' => variable_get('themekey_ui_pathalias', 0),
      '#description' => t('Assign themes to paths/path aliases from the \'URL aliases\' administration pages.'),
    );
  }
  //
  $nodeform = variable_get('themekey_ui_nodeform', 0);
  $form['themekey_ui']['themekey_ui_nodeform'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show theme option in create/edit node forms'),
    '#default_value' => $nodeform,
    '#description' => t('Assign themes from create/edit node forms. This will show a \'Theme\' section on create/edit node pages.'),
  );
  if ($nodeform) {
    $form['themekey_ui']['content_type'] = array(
      '#type' => 'fieldset',
      '#title' => t('Show \'Theme\' option for nodes of type'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['themekey_ui']['content_type']['table'] = array(
      '#theme' => 'themekey_ui_table',
      '#header' => array(t('Content Type'), t('Enabled')),
    );
    foreach (node_type_get_names() as $type => $title) {
      $form['themekey_ui']['content_type']['table'][$type]['title'] = array('#markup' => $title);
      $form['themekey_ui']['content_type']['table'][$type]['themekey_ui_nodeform|' . $type] = array(
        '#type' => 'checkbox',
        '#default_value' => variable_get('themekey_ui_nodeform|' . $type, 1),
      );
    }
  }

  $form['themekey_ui']['themekey_ui_author'] = array(
    '#type' => 'checkbox',
    '#title' => t('Let the user select a theme for her nodes in her user profile'),
    '#default_value' => variable_get('themekey_ui_author', 0),
    '#description' => t('Assign themes from user profile. All nodes created by a user will be shown to all visitors using the theme she selected in her profile.'),
  );

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}


/**
 * Form submission handler for themekey_ui_settings_form().
 *
 * @see themekey_ui_settings_form()
 */
function themekey_ui_settings_form_submit($form, &$form_state) {
  //
  foreach ($form_state['values'] as $key => $value) {
    $pos = strpos($key, 'themekey_ui_');
    if ($pos !== FALSE && $pos == 0) {
      variable_set($key, $value);
    }
  }

  themekey_update_static_rule('themekey_ui:node_triggers_theme', $form_state['values']['themekey_ui_nodeform']);
  themekey_update_static_rule('themekey_ui:node_author_triggers_theme', $form_state['values']['themekey_ui_author']);

  drupal_set_message(t('The configuration options have been saved.'));
}


/**
 * Adds theme selector to a form.
 *
 * @see themekey_ui_theme_build_select_form()
 *
 * @param $form
 *   form array
 *
 * @param $title
 *   title as string
 *
 * @param $description
 *   description as string
 *
 * @param $weight
 *   integer
 *
 * @param $collapsed
 *   boolean
 *
 * @param $element_key
 *   the name of form element
 */
function themekey_ui_theme_select_form(&$form, $title, $description, $default = 'default', $weight = NULL, $collapsed = TRUE, $element_key = 'themekey_ui_theme') {
  $themes = list_themes();
  $selectable_themes = variable_get('themekey_ui_selectable_themes', array('default' => t('System default')));
  foreach (array_keys($themes) as $name) {
    if (!array_key_exists($name, $selectable_themes) || empty($selectable_themes[$name])) {
      unset($themes[$name]);
    }
  }
  themekey_ui_theme_build_select_form($form, $title, $description, $default, $weight, $collapsed, $element_key, $themes, /*add_default*/ TRUE);
}


/**
 * Internal function to add a theme selector to a form.
 *
 * The API for contrib modules is provided by
 * @see themekey_ui_theme_select_form()
 *
 * @param $form
 *   form array
 *
 * @param $title
 *   title as string
 *
 * @param $description
 *   description as string
 *
 * @param $weight
 *   integer
 *
 * @param $collapsed
 *   boolean
 *
 * @param $element_key
 *   the name of form element
 *
 * @param $themes
 *   array of theme names to be provided as
 *   options in the select form
 *
 * @param $add_default
 *   boolean, TRUE if 'System default' should be
 *   be provided as options in the select form
 */
function themekey_ui_theme_build_select_form(&$form, $title, $description, $default, $weight, $collapsed, $element_key, $themes, $add_default) {
  $theme_options = themekey_theme_options();

  $form['themekey_ui_themes'] = array(
    '#type' => 'fieldset',
    '#title' => check_plain($title),
    '#description' => filter_xss($description),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
    '#theme' => 'themekey_ui_theme_select_form',
    '#themekey_ui_theme_select_form_element_key' => $element_key,
  );

  if ($add_default) {
    $form['themekey_ui_themes']['default']['screenshot'] = array();
    $form['themekey_ui_themes']['default']['description'] = array(
      '#type' => 'item',
      '#markup' => t("don't switch the theme"),
    );

    $options = array('default' => '');
  }

  foreach ($themes as $info) {
    if (!array_key_exists($info->name, $theme_options)) {
      continue;
    }

    $options[$info->name] = '';

    $screenshot = NULL;
    $theme_key = $info->name;
    while ($theme_key) {
      if (file_exists($themes[$theme_key]->info['screenshot'])) {
        $screenshot = $themes[$theme_key]->info['screenshot'];
        break;
      }
      $theme_key = isset($themes[$theme_key]->info['base theme']) ? $themes[$theme_key]->info['base theme'] : NULL;
    }

    $screenshot = $screenshot ? theme('image', array('path' => $screenshot, 'width' => t('Screenshot for %theme theme', array('%theme' => $info->name)), 'height' => '', 'alt' => array('class' => 'screenshot'), 'title' => FALSE)) : t('no screenshot');

    $form['themekey_ui_themes'][$info->name]['screenshot'] = array('#markup' => $screenshot);
    $form['themekey_ui_themes'][$info->name]['description'] = array(
      '#type' => 'item',
      '#title' => check_plain($info->info['name']),
      '#markup' => dirname($info->filename),
    );
  }

  $form['themekey_ui_themes'][$element_key] = array(
    '#type' => 'radios',
    '#options' => $options,
    '#default_value' => $default,
  );

  if (!is_null($weight)) {
    $form['themekey_ui_themes']['#weight'] = $weight;
  }

  // REVIEW should we hardcode this here?
  // SET GROUP FOR VERTICAL TABS
  $form['themekey_ui_themes']['#group'] = 'additional_settings';
}


/**
 * Adds theme select box to url alias form
 *
 * @see path_admin_form()
 * @see themekey_ui_pathalias_submit()
 *
 * @ingroup forms
 */
function themekey_ui_pathalias(&$form) {
  if (!isset($form['alias'])) {
    return;
  }

  list($id, $theme) = themekey_ui_get_path_theme($form['alias']['#default_value']);

  themekey_ui_theme_select_form($form, t('Theme configuration'), t('Select a theme that will be used whenever content is requested using this path alias.'), $theme, -1);

  $form['themekey_ui_themes']['themekey_rule_id'] = array(
    '#type' => 'value',
    '#value' => $id,
  );

  $form['#submit'][] = 'themekey_ui_pathalias_submit';

}


/**
 * Form submission handler for themekey_ui_pathalias().
 *
 * @see themekey_ui_pathalias()
 */
function themekey_ui_pathalias_submit($form, &$form_state) {

  if ((empty($form_state['values']['themekey_ui_theme']) || 'default' == $form_state['values']['themekey_ui_theme']) && $form_state['values']['themekey_rule_id']) {
    themekey_ui_del_path_theme($form_state['values']['themekey_rule_id']);
  }
  elseif (!empty($form_state['values']['themekey_ui_theme']) && 'default' != $form_state['values']['themekey_ui_theme']) {
    themekey_ui_set_path_theme($form_state['values']['alias'], $form_state['values']['themekey_ui_theme'], $form_state['values']['themekey_rule_id']);
  }

  if ($form['source']['#default_value'] == $form_state['values']['source'] &&
      $form['alias']['#default_value'] == $form_state['values']['alias'] &&
      $form['themekey_ui_themes']['themekey_ui_theme']['#default_value'] != $form_state['values']['themekey_ui_theme']) {
    // only theme changed => clear page cache
    // REVIEW this might be a performance issue on large sites
    cache_clear_all('%' . $form_state['values']['alias'], 'cache_page', TRUE);
  }
}


/**
 * Formats a table with checkboxes used by ThemeKey UI settings form.
 *
 * @param $form
 *   array() containing form elements to be
 *   formatted as table
 *
 * @ingroup themeable
 */
function theme_themekey_ui_table($form) {

  $form = $form['form'];

  // TODO: Should this theme themekey_ui_table be declared in hook_theme()?
  $header = isset($form['#header']) ? $form['#header'] : array();
  $attributes = isset($form['#attributes']) ? $form['#attributes'] : array();

  $rows = array();
  foreach (element_children($form) as $key) {
    $row = array();
    foreach (element_children($form[$key]) as $item) {
      $row[] = drupal_render($form[$key][$item]);
    }
    $rows[] = $row;
  }

  if (empty($rows)) {
    $message = check_plain(isset($form['#empty']) ? $form['#empty'] : t('There are no items in the table.'));
    $rows[] = array(array(
        'data' => $message,
        'colspan' => count($header),
        'align' => 'center',
        'class' => 'message',
      ));
  }

  return count($rows) ? theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => $attributes)) : '';
}


/**
 * Theme the theme select form.
 *
 * @see theme_system_theme_select_form()
 *
 * @param $form
 *   An associative array containing the structure of the form.
 *
 * @ingroup themeable
 */
function theme_themekey_ui_theme_select_form($form) {
  $rows = array();

  foreach (element_children($form['form']) as $key) {
    $row = array();
    if (isset($form['form'][$key]['description']) && is_array($form['form'][$key]['description'])) {
      $row[] = drupal_render($form['form'][$key]['screenshot']);
      $row[] = drupal_render($form['form'][$key]['description']);
      $row[] = drupal_render($form['form'][$form['form']['#themekey_ui_theme_select_form_element_key']][$key]);
    }
    $rows[] = $row;
  }

  if (!empty($rows)) {
    $header = array(t('Screenshot'), t('Name'), t('Selected'));
    $output = theme('table', array('header' => $header, 'rows' => $rows));
    return $output;
  }
}
