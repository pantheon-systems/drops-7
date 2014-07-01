<?php
/**
 * @file
 * bootstrap-dropdown.vars.php
 */

/**
 * Implements hook_preprocess_bootstrap_dropdown().
 */
function bootstrap_preprocess_bootstrap_dropdown(&$variables) {
  $element = &$variables['element'];

  // Provide defaults.
  $element += array(
    '#wrapper_attributes' => NULL,
    '#attributes' => NULL,
    '#alignment' => NULL,
    '#toggle' => NULL,
    '#items' => NULL,
  );

  // Dropdown vertical alignment.
  $element['#wrapper_attributes']['class'][] = 'dropdown';
  if ($element['#alignment'] === 'up' || (is_array($element['#alignment']) && in_array('up', $element['#alignment']))) {
    $element['#wrapper_attributes']['class'][] = 'dropup';
  }

  if (isset($element['#toggle'])) {
    if (is_string($element['#toggle'])) {
      $element['#toggle'] = array(
        '#theme' => 'link__bootstrap_dropdown__toggle',
        '#text' => $element['#toggle'],
        '#path' => '#',
        '#options' => array(
          'attributes' => array(),
          'html' => TRUE,
          'external' => TRUE,
        ),
      );
    }
    if (isset($element['#toggle']['#options']['attributes'])) {
      $element['#toggle']['#options']['attributes']['class'][] = 'dropdown-toggle';
      $element['#toggle']['#options']['attributes']['data-toggle'] = 'dropdown';
    }
    else {
      $element['#toggle']['#attributes']['class'][] = 'dropdown-toggle';
      $element['#toggle']['#attributes']['data-toggle'] = 'dropdown';
    }
  }

  // Menu items.
  $element['#attributes']['role'] = 'menu';
  $element['#attributes']['class'][] = 'dropdown-menu';
  if ($element['#alignment'] === 'right' || (is_array($element['#alignment']) && in_array('right', $element['#alignment']))) {
    $element['#attributes']['class'][] = 'dropdown-menu-right';
  }
}

/**
 * Implements hook_process_bootstrap_dropdown().
 */
function bootstrap_process_bootstrap_dropdown(&$variables) {
  $element = &$variables['element'];

  $items = array();
  foreach ($element['#items'] as $data) {
    $item_classes = array();

    // Dividers.
    if (empty($data)) {
      $data = '';
      $item_classes[] = 'divider';
    }
    // Headers (must be a string).
    elseif (is_array($data) && (!empty($data['header']) || !empty($data['#header']))) {
      $item_classes[] = 'dropdown-header';
    }
    // Disabled.
    elseif (is_array($data) && (!empty($data['disabled']) || !empty($data['#disabled']))) {
      $item_classes[] = 'disabled';
    }

    // Construct item_list item.
    $item = array(
      'data' => render($data),
      'role' => 'presentation',
    );
    if (!empty($item_classes)) {
      $item['class'] = $item_classes;
    }
    $items[] = $item;
  }

  // Create the dropdown.
  $variables['dropdown'] = array(
    '#theme_wrappers' => array('container'),
    '#attributes' => $element['#wrapper_attributes'],
    'toggle' => $element['#toggle'],
    'items' => array(
      '#theme' => 'item_list__bootstrap_dropdown',
      '#items' => $items,
      '#attributes' => $element['#attributes'],
    ),
  );
}
