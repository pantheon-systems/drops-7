<?php
// $Id$
/**
 * @file 
 *    default theme implementation for displaying a single keymatch
 *
 * This template renders a single keymatch result and is collected into
 * google-applinace-results.tpl.php. This and the parent template are
 * dependent on one another sharing the markup for keymatch listings.
 *
 * @see template_preprocess_google_appliance_keymatch()
 * @see google-appliance-results.tpl.php
 */
?>
<li class="keymatch <?php print $classes; ?>" id="keymatch-<?php print $keymatch_idx; ?>"<?php print $attributes; ?>>
  <h3 class="keymatch-description"><a href="<?php print render($url); ?>"><?php print render($description); ?></a></h3>
  <div class="google-appliance-snippet-info">
    <p class="google-appliance-info"><?php print render($url); ?></p>
  </div>
</li>
