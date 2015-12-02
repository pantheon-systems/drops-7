<?php

/**
 * @file
 * Default theme implementation for displaying a single onebox module,
 * consisting of several onebox results.
 *
 * This template collects each invocation of theme_google_appliance_onebox_result().
 * This and the child template are dependent on one another sharing the markup for the
 * onebox results listing.
 *
 * Available variables:
 * - $module_name: The name of the onebox module, from the search appliance.
 * - $provider: The name of the onebox, provided by the search appliance.
 * - $url_text: The URL for the onebox, provided by the search appliance.
 * - $url_link: Onebox link, provided by the search appliance.
 * - $image: Image provided by the search appliance.
 * - $description: Description of the onebox, provided by the search appliance.
 * - $results: An array of rendered onebox results. Note that the unrendered
 *   results are also available at $onebox['results'].
 *
 * @see https://developers.google.com/search-appliance/documentation/614/oneboxguide#providerresultsschema
 * @see template_preprocess_google_appliance_onebox()
 * @see google-appliance-results.tpl.php
 */
//dsm($variables);
?>
<?php foreach ($results as $result): ?>
  <?php print $result['rendered']; ?>
<?php endforeach; ?>
