<?php

/**
 * @file
 * Widgets for facets rendered as test link ranges.
 */

/**
 * Widget that renders facets as a series of text links.
 */
class SearchApiRangesWidgetLinks extends FacetapiWidgetLinks {

  /**
   * Renders the links.
   */
  public function execute() {
    $element = &$this->build[$this->facet['field alias']];

    // Get Search API stuff
    $searcher = $this->facet->getAdapter()->getSearcher();
    $index_id = explode('@', $searcher);
    $index = search_api_index_load($index_id[1]);
    list($query, $results) = $this->facet->getAdapter()->getCurrentSearch();

    // Prepare variables for min/max query
    $variables = array(
      'element' => $element,
      'index' => $index,
      'range_field' => $this->facet['field alias'],
      'query' => $query,
      'prefix' => $this->settings->settings['prefix'],
      'suffix' => $this->settings->settings['suffix'],
    );
 
    // Generate the ranges to the be used for the text links
    if(strlen($this->settings->settings['range_advanced']) == 0){
    	$element = search_api_ranges_generate_ranges_simple($variables, $this->settings->settings['range_simple']);
    }
    else{
		$element = search_api_ranges_generate_ranges_advanced($variables, $this->settings->settings['range_advanced']);
    }

    // Sets each item's theme hook, builds item list.
    $this->setThemeHooks($element);
    $element = array(
      '#theme' => 'item_list',
      '#items' => $this->buildListItems($element),
      '#attributes' => $this->build['#attributes'],
    );
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
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['prefix'] = array(
      '#type' => 'textfield',
      '#title' => t('Prefix'),
      '#default_value' => $this->settings->settings['prefix'],
      '#description' => t('Adds a prefix to the text links, e.g. $, #.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['suffix'] = array(
      '#type' => 'textfield',
      '#title' => t('Suffix'),
      '#default_value' => $this->settings->settings['suffix'],
      '#description' => t('Adds a suffix to the text links, e.g. &euro;, pcs., etc.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['range_simple'] = array(
      '#type' => 'textfield',
      '#title' => t('Simple range'),
      '#default_value' => $this->settings->settings['range_simple'],
      '#description' => t('Add a fixed range to create "range facet". For example, "10" on a price field will create 10 - 20, 20 - 30 etc.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['range_advanced'] = array(
      '#type' => 'textarea',
      '#title' => t('Advanced range'),
      '#default_value' => $this->settings->settings['range_advanced'],
      '#description' => t('Add a pre-defined list of ranges to create "range facets". For examples, refer to README in search api range.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
  }

  /**
   * Returns defaults for the settings this widget provides.
   */
  function getDefaultSettings() {
    return array(
      'name' => '',
      'prefix' => '',
      'suffix' => '',
      'range_simple' => 10,
      'range_advanced' => '',
    );
  }

}
