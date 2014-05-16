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

    $range_field = $this->facet['field alias'];

    // Get facet path field/alias
    if (module_exists('facetapi_pretty_paths')) {
      $processor = new FacetapiUrlProcessorPrettyPaths($this->facet->getAdapter());
      $range_field = $processor->getFacetPrettyPathsAlias($this->facet->getFacet());
    };

    // Prepare variables for min/max query
    $variables = array(
      'element' => $element,
      'index' => $index,
      'range_field' => $range_field,
      'target' => $this->facet->getAdapter()->getSearchPath(),
      'query' => $query,
      'prefix' => $this->settings->settings['prefix'],
      'suffix' => $this->settings->settings['suffix'],
    );

    // Generate the ranges to the be used for the text links
    if (strlen($this->settings->settings['range_advanced']) == 0) {
      $element = search_api_ranges_generate_ranges_simple($variables, $this->settings->settings['range_simple']);
    }
    else {
      $element = search_api_ranges_generate_ranges_advanced($variables, $this->settings->settings['range_advanced']);
    }

    // Replace ':' with '/'
    if (module_exists('facetapi_pretty_paths')) {
      foreach($element as $key => $e){
        if (isset($e['#query']['f'])){
          $element[$key]['#query']['f'] = str_replace(':', '/', $e['#query']['f']);
			if (isset($e['#path']) && isset($element[$key]['#query']['f'][0]) && substr_count($e['#path'],'/'.$element[$key]['#query']['f'][0])>0 ){
				$element[$key]['#active'] = TRUE;
				$element[$key]['#path'] = str_replace('/'.$element[$key]['#query']['f'][0],'',$e['#path']);
				unset($element[$key]['#query']['f']);
			}
        }
      }
    }

    // Sets each item's theme hook, builds item list.
    $this->setThemeHooks($element);

    $items_build = $this->buildListItems($element);

    // Replace ?f[0]= with /
    if (module_exists('facetapi_pretty_paths')) {
      foreach($items_build as $key => $build){
        $items_build[$key]['data'] = str_replace('?f[0]=', '/', $build['data']);
      }
    }

    $element = array(
      '#theme' => 'item_list',
      '#items' => $items_build,
      '#attributes' => $this->build['#attributes'],
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
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
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
        'enabled' => array(
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
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['range_simple'] = array(
      '#type' => 'textfield',
      '#title' => t('Simple range'),
      '#default_value' => $this->settings->settings['range_simple'],
      '#description' => t('Add a fixed range to create "range facet". 
        For example, "10" on a price field will create 10 - 20, 20 - 30 etc.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['range_advanced'] = array(
      '#type' => 'textarea',
      '#title' => t('Advanced range'),
      '#default_value' => $this->settings->settings['range_advanced'],
      '#description' => t('Add a pre-defined list of ranges to create 
        "range facets". For examples, refer to README in search api range.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
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


/**
 * Widget that renders facets as a list of clickable checkboxes.
 *
 * This widget renders facets in the same way as SearchApiRangesWidgetLinks but uses
 * JavaScript to transform the links into checkboxes followed by the facet.
 */
class SearchApiRangesWidgetCheckboxLinks extends SearchApiRangesWidgetLinks {

  /**
   * Overrides SearchApiRangesWidgetLinks::init().
   *
   * Adds additional JavaScript settings and CSS.
   */
  public function init() {
    parent::init();
    $this->jsSettings['makeCheckboxes'] = 1;
    drupal_add_css(drupal_get_path('module', 'facetapi') . '/facetapi.css');
  }

  /**
   * Overrides SearchApiRangesWidgetLinks::getItemClasses().
   *
   * Sets the base class for checkbox facet items.
   */
  public function getItemClasses() {
    return array('facetapi-checkbox');
  }
}
