(function($) {

  /**
   * Make <a> tags hit /click service when clicked.
   * Hit /click service with ctype 'load'.
   *
   * Supported click types:
   * - c        - search result.
   * - cluster  - cluster label on results page.
   * - keymatch - keymatch on results page.
   * - logo     - hyperlinked logo.
   * - nav.next - navigation, next page.
   * - nav.page - navigation, specific page.
   * - nav.prev - navigation, previous page.
   * - onebox   - onebox on results page.
   * - sort     - sort link on results page.
   * - spell    - spelling suggestion.
   * - synonym  - related query on results page.
   * - load     - load results page.
   *
   * @see https://www.google.com/support/enterprise/static/gsa/docs/admin/70/gsa_doc_set/admin_searchexp/ce_improving_search.html#1034719
   */
  $(document).ready(function() {
    // Add a clickData function for clusters.
    Drupal.settings.google_appliance.clickTracking.clickData = function($element, clickType) {
      // Return the innerHTML as for cluster
      if (clickType === 'cd') {
        return $element.innerHTML;
      }
    };

    // Initialize the plugins based on Drupal settings.
    $('a').gsaClicks(Drupal.settings.google_appliance.clickTracking);
  });

})(jQuery);
