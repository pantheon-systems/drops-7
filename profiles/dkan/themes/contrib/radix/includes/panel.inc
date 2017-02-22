<?php
/**
 * @file
 * Theme and preprocess functions for panes and panels.
 */

/**
 * Implements template_preprocess_panelizer_view_mode().
 */
function radix_preprocess_panelizer_view_mode(&$variables) {
  $element = $variables['element'];

  // Add some custom classes.
  $variables['classes_array'] = array();
  $variables['classes_array'][] = drupal_html_class($element['#entity_type'] . '-' . $element['#bundle']);
  $variables['classes_array'][] = drupal_html_class('view-mode-' . $element['#view_mode']);
}

/**
 * Implements theme_panels_default_style_render_region().
 */
function radix_panels_default_style_render_region($vars) {
  return implode("", $vars['panes']);
}

/**
 * Implements template_preprocess_panels_pane().
 */
function radix_preprocess_panels_pane(&$variables) {
  $pane = $variables['pane'];
  if (isset($pane->css['css_class'])) {
    // Check for Bootstrap panel classes.
    $css_classes = explode(" ", $pane->css['css_class']);
    $bootstrap_panel_classes = _radix_get_bootstrap_panel_classes();
    $panel_classes = array_intersect($css_classes, $bootstrap_panel_classes);

    if (count($panel_classes)) {
      // Use a different template.
      $variables['theme_hook_suggestions'][] = 'panels_pane__bootstrap_panel';

      // Add default classes to panel.
      $variables['classes_array'][] = 'panel';
      $variables['classes_array'] += $panel_classes;

      // Add default classes to pane title.
      $variables['title_attributes_array']['class'][] = 'panel-title';
    }
  }
}

/**
 * Implements template_preprocess_panels_add_content_modal().
 */
function radix_preprocess_panels_add_content_modal(&$variables) {
  // Process the list of categories.
  $variables['categories_array'] = array();
  foreach ($variables['categories'] as $key => $category_info) {
    // 'root' category is actually displayed under the categories, so
    // skip it.
    if ($key == 'root') {
      continue;
    }

    $class = 'panels-modal-add-category btn btn-sm btn-default';
    if ($key == $variables['category']) {
      $class .= ' active';
    }

    $url = $variables['renderer']->get_url('select-content', $variables['region'], $key);
    $variables['categories_array'][] = ctools_ajax_text_button($category_info['title'], $url, '', $class);
  }

  // Now render the top level buttons (aka the root category) if any.
  $variables['root_content'] = '';
  if (!empty($variables['categories']['root'])) {
    foreach ($variables['categories']['root']['content'] as $content_type) {
      $variables['root_content_array'][] = theme('panels_add_content_link', array('renderer' => $variables['renderer'], 'region' => $variables['region'], 'content_type' => $content_type));
    }
  }
}

/**
 * Implements template_preprocess_panels_add_content_link().
 */
function radix_preprocess_panels_add_content_link(&$vars) {
  $vars['title'] = filter_xss_admin($vars['content_type']['title']);
  $vars['description'] = isset($vars['content_type']['description']) ? $vars['content_type']['description'] : $vars['title'];
  $vars['icon'] = ctools_content_admin_icon($vars['content_type']);
  $vars['url'] = $vars['renderer']->get_url('add-pane', $vars['region'], $vars['content_type']['type_name'], $vars['content_type']['subtype_name']);

  $icon = theme('image', array('path' => $vars['icon']));
  $title = $icon . ' ' . $vars['title'];
  $button_classes = 'panels-modal-add-config btn btn-default btn-sm';
  $vars['button'] = ctools_ajax_text_button($title, $vars['url'], $vars['description'], $button_classes);

  // If title is Add, or if the Panopoly Magic add button variable is set,
  // assume we are rendering an 'Add' button for preview.
  if ($vars['title'] == t('Add') || !empty($vars['content_type']['panopoly_magic_add_button'])) {
    $title = '<i class="fa fa-plus"></i> ' . $vars['title'];
    $button_classes = 'panels-modal-add-config btn btn-success btn-sm pull-right';
    $vars['button'] = ctools_ajax_text_button($title, $vars['url'], $vars['description'], $button_classes);
  }
}
