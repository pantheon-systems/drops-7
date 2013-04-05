<?php
/**
 * @file
 * The commerce_search_api_fancy widget plugin class.
 */

/**
 * Widget that renders taxonomy terms as fancy attributes
 */
class CommerceSearchApiFancy extends FacetapiWidgetLinks {

  /**
   * Overrides FacetapiWidget:Links:settingsForm().
   */
  function settingsForm(&$form, &$form_state) {
    parent::settingsForm($form, $form_state);
    $form['widget']['widget_settings']['links'][$this->id]['display_count'] = array(
      '#type' => 'checkbox',
      '#title' => t('Display the number of matching items'),
      '#default_value' => !empty($this->settings->settings['display_count']) ? $this->settings->settings['display_count'] : FALSE,
      '#description' => t('If checked, the number of matching items will be output with the color square.'),
    );
  }

  /**
   * Overrides FacetapiWidgetLinks::buildListItems().
   *
   * Add our css file and call our theme function to theme taxonomy terms as
   * fancy attributes.
   */
  function buildListItems($build) {
    $facet_info = $this->facet->getFacet();
    if ($facet_info['field type'] == 'taxonomy_term' && module_exists('commerce_fancy_attributes')) {
      $terms = taxonomy_term_load_multiple(array_keys($build));
      $first_term = reset($terms);
      $bundle = $first_term->vocabulary_machine_name;
      // Loop over the field instances to find an instance that use the
      // fancy attributes formatter.
      foreach (field_info_instances('taxonomy_term', $bundle) as $field_name => $instance) {
        if ($instance['display']['add_to_cart_form']['type'] != 'commerce_fancy_attributes_color') {
          continue;
        }
        $found = TRUE;
      }
      // Check if we should display the facet count.
      $display_count = !empty($this->settings->settings['display_count']);
      // If we've found at least one field that uses the fancy attributes formatter.
      if (!empty($found)) {
        drupal_add_css(drupal_get_path('module', 'commerce_search_api') . '/commerce_search_api.css');
        foreach ($build as $value => &$item) {
          $item['#html'] = TRUE;
          $variables['hex'] = $terms[$value]->{$field_name}[LANGUAGE_NONE][0]['value'];
          if ($display_count) {
            $variables['title'] = $item['#count'];
          }
          $item['#markup'] = theme('commerce_search_api_fancy_attributes_color', $variables);
          $item['#count'] = NULL;
        }
      }
    }
    $items = parent::buildListItems($build);
    return $items;
  }

}
