<?php
// $Id$
/**
 * @file
 *    default theme implementation for displaying a spelling suggestion
 *
 * This template renders a spelling suggestion and is collected into
 * google-applinace-results.tpl.php. This and the parent template are
 * dependent on one another sharing the markup for synonym listings.
 *
 * @see template_preprocess_google_appliance_spelling_suggestion()
 * @see google-appliance-results.tpl.php
 */
?>
<div class="spelling-suggestion <?php print $classes; ?>" <?php print $attributes; ?>>
  <?php print render($spelling_suggestion); ?>
</div>
