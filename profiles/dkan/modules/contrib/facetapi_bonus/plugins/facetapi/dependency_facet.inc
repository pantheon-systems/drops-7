<?php

/**
 * @file
 *
 * Performs a dependency check against other specified facets/facet values
 *
 * ToDos:
 * - remove depending facet from url if this dependency gets lost
 * - enable user to choose, how multiple dependencies to other facets are
 *   treated (are they met using AND, OR - or an arbitrary combination chain?)
 *
 */

/**
 * Adds a dependency on another facet being active.
 */
class FacetapiDependencyFacet extends FacetapiDependency {

  protected $defaultSettings;

  /**
   * Executes the dependency check.
   */
  public function execute() {
    // Show this facet, if it is active itself and forced deactivation isn't on.
    if (!$this->settings['force_deactivation'] && $this->activeItems[$this->facet['name']]) {
      return;
    }
    else {
      // Show this facet if no facet dependencies are configured.
      $facets = array_filter($this->settings['facets']);
      if (!count($facets)) {
        return;
      }
      else {
        // Check if any configured facet dependency is met.
        foreach ($facets as $facet_name) {
          if ($this->activeItems[$facet_name]) {
            // Show if dependency is met without specific dependency items.
            if (empty($this->settings['facet_items_' . $facet_name])) {
              return;
            }
            else {
              $items_string = trim($this->settings['facet_items_' . $facet_name]);
              $facet_items = drupal_explode_tags($items_string);
              // Show if dependency is met without specific dependency items.
              if (empty($items_string) || !count($facet_items)) {
                return;
              }
              // Show if dependency is met with specific dependency items.
              else {
                foreach ($facet_items as $facet_item) {
                  foreach ($this->activeItems[$facet_name] as $active_facet_item_key => $active_facet_item_data) {
                    if (!empty($facet_item)) {
                      if (!$this->settings['regex']) {
                        if ($facet_item == $active_facet_item_key) {
                          return;
                        }
                      }
                      else {
                        $facet_item = '/' . trim(str_replace('/', '\\/', $facet_item)) . '/';
                        if (preg_match($facet_item, $active_facet_item_key)) {
                          return;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }

        // Otherwise hide this facet.
        return FALSE;
      }
    }
  }

  /**
   * Adds dependency settings to the form.
   */
  public function settingsForm(&$form, &$form_state) {
    // Get enabled facets (minus this dependency's one).
    $facets = $this->getEnabledFacets();

    $form[$this->id]['facets'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Show this facet only for specific other facets'),
      '#default_value' => $this->settings['facets'],
      '#options' => array_map('check_plain', $facets),
      '#description' => t('Show this facet only if another one of these selected facet(s) is actively in use. If you select no other facets, the facet will be visible not depend on other facets.'),
    );

    $form[$this->id]['force_deactivation'] = array(
      '#type' => 'checkbox',
      '#title' => t('Force deactivation for unmet dependency.'),
      '#default_value' => $this->settings['force_deactivation'],
      '#description' => t('Deactivate this facet if dependency is not met anymore, even if this facet has had active items itself (recommended)'),
    );

    $form[$this->id]['regex'] = array(
      '#title' => t('Regular expressions used'),
      '#type' => 'checkbox',
      '#description' => t('Interpret each facet item of each list below as a regular expression pattern.<br /><small>(Slashes are escaped automatically, patterns using a comma can be wrapped in "double quotes", and if such a pattern uses double quotes itself, just make them double-double-quotes (""))</small>.'),
      '#default_value' => $this->settings['regex'],
    );

    // Add optional value field and extend default settings for each facet.
    foreach ($facets as $facet_name => $facet_title) {
      $id = 'facet_items_' . $facet_name;
      $checkboxid = '#edit-facets-' . str_replace('_', '-', drupal_strtolower($facet_name));
      $form[$this->id][$id] = array(
        '#title' => t('Facet items for %facet', array('%facet' => $facet_title)),
        '#type' => 'textfield',
        '#description' => t("Comma separated list of facet items to depend on, matching against an facet item's value."),
        '#default_value' => isset($this->settings[$id]) ? $this->settings[$id] : NULL,
        '#states' => array('visible' => array($checkboxid => array('checked' => TRUE))),
      );
      $this->defaultSettings[$id] = '';
    }
  }

  /**
   * Returns defaults for settings.
   *
   * Has to be dynamic since we want a detail settings field for each dependency
   * but the enabled facets are not known yet (afaik, hence the add. property)
   */
  public function getDefaultSettings() {
    if (!count($this->defaultSettings)) {
      $this->defaultSettings = array(
                                     'facets' => array(),
                                     'force_deactivation' => TRUE,
                                     'regex' => FALSE,
                                    );
    }
    return $this->defaultSettings;
  }

  /**
   * Determine enabled facets
   *
   * Returns an associative array facet name => title
   * of factes enabled for the given searcher
   */
  public function getEnabledFacets() {
    $facets = array();

    $enabled_facets = facetapi_get_enabled_facets($this->adapter->getSearcher());
    unset($enabled_facets[$this->facet['name']]);
    foreach ($enabled_facets as $facet_key => $facet_object) {
      $facets[$facet_key] = $facet_object['label'] . ' (' . $facet_object['name'] . ')';
    }

    return $facets;
  }

}
