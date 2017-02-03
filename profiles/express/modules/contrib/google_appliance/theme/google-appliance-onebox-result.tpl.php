<?php

/**
 * @file
 * Default theme implementation for displaying a single onebox result.
 *
 * This template renders a single onebox result and is collected into
 * google-applinace-onebox-module.tpl.php. This and the parent template are
 * dependent on one another sharing the markup for results listings.
 *
 * Available variables:
 * - $abs_url: The absolute URL representing the onebox result.
 * - $title: The title of the onebox result.
 * - $title_linked: A pre-rendered HTML link whose URL is $abs_url above and
 *   whose link text is the $title above.
 * - $fields: An array of meta fields associated with this onebox result.
 *
 * @see https://developers.google.com/search-appliance/documentation/614/oneboxguide#providerresultsschema
 * @see template_preprocess_google_appliance_onebox_result()
 * @see google-appliance-onebox-module.tpl.php
 */
//dsm($variables);
?>
<?php if ($title_linked): ?>
  <h4><?php print $title_linked; ?></h4>
<?php endif; ?>
<ul>
  <?php foreach ($fields as $name => $value): ?>
    <li class="<?php print $name; ?>"><?php print $value; ?></li>
  <?php endforeach; ?>
</ul>
