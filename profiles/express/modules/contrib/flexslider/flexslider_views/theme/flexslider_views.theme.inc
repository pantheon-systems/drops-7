<?php

/**
 * @file
 * Theme functions for FlexSlider Views
 *
 * @author Mathew Winstone <mwinstone@coldfrontlabs.ca>
 */

/**
 * Prepares variables for flexslider views templates.
 *
 * @see flexslider-views.tpl.php.
 */
function template_process_flexslider_views(&$variables) {
  // Only run the preprocessor if it is a view
  if (!empty($variables['view'])) {
    // Prepare all data to be passed to theme_flexslider().
    $items = array();
    $view = $variables['view'];
    $settings = array();
    $settings['optionset'] = $variables['options']['optionset'];

    $row_plugin = $view->style_plugin->row_plugin;

    foreach ($variables['rows'] as $id => $row) {
      // Render the row into a slide
      $view->row_index = $id;
      $item['slide'] = $row_plugin->render($row);
      $item['caption'] = (isset($row->caption) && !empty($row->caption)) ? $row->caption : NULL;
      $items[] = $item;
    }

    // Add the slide items to the variables
    $variables['items'] = $items;

    // Add the attributes
    $settings['attributes']['id'] = $variables['options']['id'];

    // Reset the options to the processed values
    $variables['settings'] = $settings;
  }
  return $variables;
}
