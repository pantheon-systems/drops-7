<?php
// $Id$
/**
 * @file 
 *    default theme implementation for displaying a single synonym
 *
 * This template renders a single synonym result and is collected into
 * google-applinace-results.tpl.php. This and the parent template are
 * dependent on one another sharing the markup for synonym listings.
 *
 * @see template_preprocess_google_appliance_synonym()
 * @see google-appliance-results.tpl.php
 */
?>
<li class="synonym <?php print $classes; ?>" id="synonym-<?php print $synonym_idx; ?>"<?php print $attributes; ?>>
  <?php print render($link); ?>
</li>
