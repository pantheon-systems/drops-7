<?php

/**
 * @file
 * Widget for facets rendered as UI slider with Min/Max.
 */
class SearchApiRangesWidgetUISlider extends FacetapiWidget {

  /**
   * Renders the links.
   */
  public function execute() {
    $element = &$this->build[$this->facet['field alias']];

    $theme_suffix = '';
    $theme_suffix .= '__' . preg_replace('/\W+/', '_', $this->facet->getAdapter()->getSearcher());
    $theme_suffix .= '__' . preg_replace('/\W+/', '_', $this->facet['field alias']);

    $element = array(
      '#theme' => 'search_api_ranges_slider' . $theme_suffix,
      '#slider' => $this->_buildUISliderForm(),
    );

    // Add js.
    drupal_add_library('system', 'ui.slider');
    drupal_add_js(drupal_get_path('module', 'search_api_ranges') . '/jquery.numeric.js');
    drupal_add_js(drupal_get_path('module', 'search_api_ranges') . '/search_api_ranges.js');
  }

  /**
   * Allows the widget to provide additional settings to the form.
   */
  function settingsForm(&$form, &$form_state) {
    $form['widget']['widget_settings']['links'][$this->id]['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => $this->settings->settings['name'],
      '#description' => t('The name of the range field.'),
      '#states' => array('visible' => array('select[name="widget"]' => array('value' => $this->id),),),
    );
    $form['widget']['widget_settings']['links'][$this->id]['prefix'] = array(
      '#type' => 'textfield',
      '#title' => t('Prefix'),
      '#default_value' => $this->settings->settings['prefix'],
      '#description' => t('Adds a prefix to the slider, e.g. $, #.'),
      '#states' => array('visible' => array('select[name="widget"]' => array('value' => $this->id),),),
    );
    $form['widget']['widget_settings']['links'][$this->id]['suffix'] = array(
      '#type' => 'textfield',
      '#title' => t('Suffix'),
      '#default_value' => $this->settings->settings['suffix'],
      '#description' => t('Adds a suffix to the slider, e.g. &euro;, pcs., etc.'),
      '#states' => array('visible' => array('select[name="widget"]' => array('value' => $this->id),),),
    );
    $form['widget']['widget_settings']['links'][$this->id]['auto-submit-delay'] = array(
      '#type' => 'textfield',
      '#title' => t('Auto Submit Delay'),
      '#default_value' => $this->settings->settings['auto-submit-delay'],
      '#description' => t('Automatically submit form after the user changes 
        releases slider handles. Enter a delay in milliseconds,
        i.e. 1000 for 1 second.'),
      '#states' => array('visible' => array('select[name="widget"]' => array('value' => $this->id),),),
    );
  }

  /**
   * Returns defaults for the settings form.
   */
  function getDefaultSettings() {
    return array(
      'name' => '',
      'prefix' => '',
      'suffix' => '',
      'auto-submit-delay' => 1500,
    );
  }

  /**
   * Builds a UI slider themed form.
   * Performs min/max queries through Search API.
   */
  public function _buildUISliderForm() {
    $slider = array();

    // Get Search API stuff
    $searcher = $this->facet->getAdapter()->getSearcher();
    $index_id = explode('@', $searcher);
    $index = search_api_index_load($index_id[1]);
    list($query, $results) = $this->facet->getAdapter()->getCurrentSearch();

    // Make a clone of the query, as to not alter the current search query
    $query = clone $query;

    // Prepare variables for min/max query
    $variables = array(
      'index' => $index,
      'range_field' => $this->facet['field alias'],
      'query' => $query,
    );

    // Query the min/max values for the range slider
    $min_value = search_api_ranges_minmax($variables, 'ASC');
    $max_value = search_api_ranges_minmax($variables, 'DESC');

    // Kill widget if there is nothing to do
    if (empty($min_value) && empty($max_value)) {
      return array();
    };

    // Calculate user input from/to values (different concept than min/max)
    foreach ($this->facet->getAdapter()->getAllActiveItems() as $key => $active_item) {
      if ($active_item['field alias'] == $this->facet['field alias']) {
        $values = explode(' ', substr($active_item['value'], 1, -1));
        $from_value = round($values[0]);
        $to_value = round($values[2]);
        break;
      };
    };

    // User from/to cannot exceed queried min/max, adjust if needed
    if (!isset($from_value) || $from_value < $min_value) {
      $from_value = $min_value;
    };
    if (!isset($to_value) || $max_value < $to_value) {
      $to_value = $max_value;
    };

    // Get facet path field/alias
    $range_field = $this->facet['field alias'];
    if (module_exists('facetapi_pretty_paths')) {
      $processor = new FacetapiUrlProcessorPrettyPaths($this->facet->getAdapter());
      $range_field = $processor->getFacetPrettyPathsAlias($this->facet->getFacet());
    };

    // Prepare the slider variables and return themed form
    // @see search-api-ranges-slider.tpl.php
    $variables = array(
      'range_field' => rawurlencode($range_field),
      'name' => $this->settings->settings['name'],
      'prefix' => $this->settings->settings['prefix'],
      'suffix' => $this->settings->settings['suffix'],
      'min' => $min_value,
      'max' => $max_value,
      'from' => $from_value,
      'to' => $to_value,
      'auto_submit_delay' => is_numeric($this->settings->settings['auto-submit-delay']) ? $this->settings->settings['auto-submit-delay'] : 0,
      'active_items' => $this->facet->getAdapter()->getAllActiveItems(),
      'target' => $this->facet->getAdapter()->getSearchPath(),
    );
    // We need to generate unique form IDs in case multiple forms get rendered
    // on the same page. search_api_ranges_forms() takes care of mapping them
    // back to the base form ID 'search_api_ranges_block_slider_view_form'.
    return drupal_get_form('search_api_ranges_block_slider_view_form_' . $range_field, $variables);
  }

}
