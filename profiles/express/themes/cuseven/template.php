<?php

/**
 * Implementation of hook_theme().
 */
function cuseven_theme() {
  $items = array();
  
  $items['node_form'] = array(
    'render element' => 'form',
    'path' => drupal_get_path('theme', 'cuseven') .'/templates',
    'template' => 'form-default',
    'preprocess functions' => array(
      'cuseven_preprocess_form_node',
      'cuseven_preprocess_form_buttons',
    ),
  );

  return $items;
}

/**
 * Helper function for cloning and drupal_render()'ing elements.
 */
function cuseven_render_clone($elements) {
  static $instance;
  if (!isset($instance)) {
    $instance = 1;
  }
  foreach (element_children($elements) as $key) {
    if (isset($elements[$key]['#id'])) {
      $elements[$key]['#id'] = "{$elements[$key]['#id']}-{$instance}";
    }
  }
  $instance++;
  return drupal_render($elements);
}


/**
 * Preprocessor for handling form button for most forms.
 */
function cuseven_preprocess_form_buttons(&$vars) {
  if (!empty($vars['form']['actions'])) {
    $vars['actions'] = $vars['form']['actions'];
    unset($vars['form']['actions']);
  }
}

/**
 * Preprocessor for theme('node_form').
 */
function cuseven_preprocess_form_node(&$vars) {
  if (isset($vars['form']['#node']->nid)) {
    $vars['node_id'] = array(
      '#type' => 'fieldset', 
      '#collapsible' => FALSE, 
      '#collapsed' => FALSE,
      '#title' => t('Node ID'),
    );
    $vars['form']['title']['#description'] = 'Drupal path: node/' . $vars['form']['#node']->nid;
  }
  $vars['sidebar'] = isset($vars['sidebar']) ? $vars['sidebar'] : array();
  // Support nodeformcols if present.
  if (module_exists('nodeformcols')) {
    $map = array(
      'nodeformcols_region_right' => 'sidebar',
      'nodeformcols_region_footer' => 'footer',
      'nodeformcols_region_main' => NULL,
    );
    foreach ($map as $region => $target) {
      if (isset($vars['form'][$region])) {
        if (isset($vars['form'][$region]['#prefix'], $vars['form'][$region]['#suffix'])) {
          unset($vars['form'][$region]['#prefix']);
          unset($vars['form'][$region]['#suffix']);
        }
        if (isset($vars['form'][$region]['actions'], $vars['form'][$region]['actions'])) {
          $vars['actions'] = $vars['form'][$region]['actions'];
          unset($vars['form'][$region]['actions']);
        }
        if (isset($target)) {
          $vars[$target] = $vars['form'][$region];
          unset($vars['form'][$region]);
        }
      }
    }
  }
  // Default to showing taxonomy in sidebar if nodeformcols is not present.
  elseif (isset($vars['form']['taxonomy']) && empty($vars['sidebar'])) {
    $vars['sidebar']['taxonomy'] = $vars['form']['taxonomy'];
    unset($vars['form']['taxonomy']);
  }
}