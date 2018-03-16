<?php

/**
 * @file
 *
 * Filter to exclude specified facet items from being shown on the facet
 *
 * This is heavily inspired by the excellent blog post from Trellon
 * http://www.trellon.com/content/blog/apachesolr-and-facetapi
 */

/**
 * Plugin that excludes specified facet items.
 */
class FacetapiFilterExcludeItems extends FacetapiFilter {

  /**
   * Filters facet items.
   */
  public function execute(array $build) {
    $exclude_string = $this->settings->settings['exclude'];
    $exclude_array = drupal_explode_tags($exclude_string);
    // Exclude item if its markup or indexed_value is one of excluded items
    // or is matched by one of the excluded item treated as regexp if the
    // regex option is checked by the user.
    $filtered_build = array();
    foreach ($build as $key => $item) {
      $exclude = FALSE;
      foreach ($exclude_array as $exclude_item) {
        if (!empty($exclude_item)) {
          if (!$this->settings->settings['regex']) {
            $exclude |= (
              $item['#indexed_value'] == $exclude_item ||
              $item['#markup'] == $exclude_item
            );
          }
          else {
            $exclude_item = '/' . trim(str_replace('/', '\\/', $exclude_item)) . '/';
            $exclude |= (
              preg_match($exclude_item, $item['#indexed_value']) ||
              preg_match($exclude_item, $item['#markup'])
            );
          }
        }
      }
      if (!$exclude) {
        $filtered_build[$key] = $item;
      }
    }

    return $filtered_build;
  }

  /**
   * Adds settings to the filter form.
   */
  public function settingsForm(&$form, &$form_state) {
    $form['exclude'] = array(
      '#title' => t('Exclude items'),
      '#type' => 'textfield',
      '#description' => t("Comma separated list of titles or values that should be excluded, matching either an item's title or value."),
      '#default_value' => $this->settings->settings['exclude'],
    );
    $form['regex'] = array(
      '#title' => t('Regular expressions used'),
      '#type' => 'checkbox',
      '#description' => t('Interpret each exclude list item as a regular expression pattern.<br /><small>(Slashes are escaped automatically, patterns using a comma can be wrapped in "double quotes", and if such a pattern uses double quotes itself, just make them double-double-quotes (""))</small>.'),
      '#default_value' => $this->settings->settings['regex'],
    );
  }

  /**
   * Returns an array of default settings.
   */
  public function getDefaultSettings() {
    return array(
      'exclude' => '',
      'regex' => 0,
    );
  }
}
