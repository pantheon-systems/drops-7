<?php
/**
 * @file
 * Menu Badges admin forms.
 */

/**
 * Admin form for adding badges to tabs.
 * @param $form
 * @param $form_state
 */
function menu_badges_tabs_admin_form($form, $form_state) {
  $badges = menu_badges_get_badges();
  $form['#tree'] = TRUE;
  $form['#attached']['css'] = array(drupal_get_path('module', 'menu_badges') . '/menu_badges.css');
  $form['search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search'),
  );
  $form['search']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#size' => 30,
  );
  $form['search']['path'] = array(
    '#type' => 'textfield',
    '#title' => t('Menu Path'),
    '#size' => 30,
  );
  $form['search']['type'] = array(
    '#type' => 'select',
    '#title' => t('Menu Type'),
    '#options' => array(
      '' => '',
      MENU_DEFAULT_LOCAL_TASK => t('Tab (MENU_DEFAULT_LOCAL_TASK)'),
      MENU_LOCAL_TASK => t('Tab (MENU_LOCAL_TASK)'),
      MENU_LOCAL_ACTION => t('Action Link (MENU_LOCAL_ACTION)'),
    ),
  );
  $form['search']['search'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  $query = db_select('menu_router', 'mr')
             ->fields('mr', array('path', 'type', 'title'));
  if (!empty($form_state['input']['search']['title'])) {
    $query->condition('title', '%' . $form_state['input']['search']['title'] . '%', 'LIKE');
  }
  if (!empty($form_state['input']['search']['path'])) {
    $query->condition('path', '%' . $form_state['input']['search']['path'] . '%', 'LIKE');
  }
  if (!empty($form_state['input']['search']['type'])) {
    $query->condition('type', $form_state['input']['search']['type'], '=');
  }
  else {
    $query->condition('type', array(MENU_DEFAULT_LOCAL_TASK, MENU_LOCAL_TASK, MENU_LOCAL_ACTION), 'IN');
  }
  $results = $query->extend('PagerDefault')->limit(25)->execute();
  $menu_badge_options = array('' => t('None')) + menu_badges_get_badge_options();
  $tab_badges = variable_get('menu_badges_tab_callbacks', array());
  $form['results'] = array();

  foreach ($results as $record) {
    $form['results'][$record->path] = array();
    $form['results'][$record->path]['path'] = array(
      '#type' => 'value',
      '#value' => $record->path,
    );
    $form['results'][$record->path]['title'] = array(
      '#type' => 'value',
      '#value' => $record->title,
    );
    $form['results'][$record->path]['type'] = array(
      '#type' => 'value',
      '#value' => $record->type,
    );
    $form['results'][$record->path]['menu_badges_callback'] = array(
      '#type' => 'select',
      '#options' => $menu_badge_options,
      '#default_value' => !empty($tab_badges[$record->path]) ? $tab_badges[$record->path]['key'] : '',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Submit handler for tabs admin form.
 * @param $form
 * @param $form_state
 */
function menu_badges_tabs_admin_form_submit(&$form, &$form_state) {

  if ($form_state['values']['op'] == $form_state['values']['search']['search']) {
    $form_state['rebuild'] = TRUE;
  }
  else {
    $tab_badges = variable_get('menu_badges_tab_callbacks', array());
    $badges = menu_badges_get_badges();
    foreach (array_keys($form_state['values']['results']) as $path) {
      if (!empty($form_state['values']['results'][$path]['menu_badges_callback'])) {
        $tab_badges[$path] = array(
          'callback' => $badges[$form_state['values']['results'][$path]['menu_badges_callback']]['callback'],
          'key' => $badges[$form_state['values']['results'][$path]['menu_badges_callback']]['key'],
        );
        if (!empty($badges[$form_state['values']['results'][$path]['menu_badges_callback']]['arguments'])) {
          $tab_badges[$path]['arguments'] = $badges[$form_state['values']['results'][$path]['menu_badges_callback']]['arguments'];
        }
      }
      else {
        unset($tab_badges[$path]);
      }
    }
    variable_set('menu_badges_tab_callbacks', $tab_badges);
    drupal_set_message(t('Tab menu badges saved.'));
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Form theming for tab admin form.
 * @param $variables
 */
function theme_menu_badges_tabs_admin_form($variables) {
  $form =& $variables['form'];

  $output = '';
  $output .= drupal_render($form['search']);
  $rows = array();
  $types = array(MENU_DEFAULT_LOCAL_TASK => t('Tab (MENU_DEFAULT_LOCAL_TASK)'), MENU_LOCAL_TASK => t('Tab (MENU_LOCAL_TASK)'), MENU_LOCAL_ACTION => t('Action Link (MENU_LOCAL_ACTION)'));
  foreach ($form['results'] as $path => $path_info) {
    if (is_array($path_info) && !empty($path_info['path']['#value'])) {
      $row = array();
      $row[] = $path_info['title']['#value'];
      $row[] = $path_info['path']['#value'];
      $row[] = $types[$path_info['type']['#value']];
      $row[] = drupal_render($form['results'][$path]['menu_badges_callback']);
      $rows[] = $row;
    }
  }
  $output .= theme('table', array('header' => array(t('Title'), t('Menu Path'), t('Menu Type'), t('Menu Badge')), 'rows' => $rows));
  $output .= theme('pager');
  $output .= drupal_render_children($form);

  return $output;
}
