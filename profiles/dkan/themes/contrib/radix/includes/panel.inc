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
 * Implements template_process_panels_add_content_modal().
 */
function radix_process_panels_add_content_modal(&$vars) {
  $content = !empty($vars['categories'][$vars['category']]['content']) ? $vars['categories'][$vars['category']]['content'] : array();

  // If no category is selected or the category is empty or our special empty
  // category render a 'header' that will appear instead of the columns.
  if (empty($vars['category']) || empty($content) || $vars['category'] == 'root') {
    // Nothing to do since there is no preview
  }
  else {
    $titles = array_keys($content);
    natcasesort($titles);
    $col_size = count($titles) / $vars['column_count'];

    // Zero out the existing column data
    $count = 0;
    foreach ($titles as $title) {
      $which = floor($count++ / $col_size) + 1;
      $vars['columns'][$which] = '';
    }

    // Read the column data with our preview functionality
    $count = 0;
    foreach ($titles as $title) {
      $which = floor($count++ / $col_size) + 1;

      // Add an 'Add' button to widget.
      $content[$title]['title'] = t('Add');
      $widget_title = theme('panels_add_content_link', array(
        'renderer' => $vars['renderer'], 
        'region' => $vars['region'], 
        'content_type' => $content[$title],
      ));

      // Render the widget preview.
      $widget_preview = (!empty($content[$title]['preview']) ? $content[$title]['preview'] : t('No Preview'));

      // Render the preview output as a fieldset.
      $preview_output = '<fieldset class="widget-preview panel panel-default">';
      $preview_output .= '<legend class="panel-heading">';
      $preview_output .= '<div class="panel-title fieldset-legend pull-left">' . $title . '</div>';
      $preview_output .= $widget_title;
      $preview_output .= '</legend>';
      $preview_output .= '<div class="panel-body">';
      $preview_output .= $widget_preview;
      $preview_output .= '</div>';
      $preview_output .= '</fieldset>';

      $vars['columns'][$which] .= $preview_output;
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

  // If title is Add, assume we are rendering an 'Add' button for preview;
  if ($vars['title'] == t('Add')) {
    $title = '<i class="fa fa-plus"></i> ' . $vars['title'];
    $button_classes = 'panels-modal-add-config btn btn-success btn-sm pull-right';
    $vars['button'] = ctools_ajax_text_button($title, $vars['url'], $vars['description'], $button_classes);
  }
}
