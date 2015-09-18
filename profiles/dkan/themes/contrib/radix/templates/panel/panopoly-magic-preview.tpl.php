<?php
/**
 * @file
 * Template for the Panopoly Magic preview.
 */
?>
<fieldset class="panel panel-default <?php print $classes; ?>"<?php print $attributes; ?>>
  <legend class="panel-heading">
    <span class="panel-title fieldset-legend pull-left"><?php print $title; ?></span>
    <?php if (!empty($add_link)): ?>
      <?php print $add_link; ?>
    <?php endif; ?>
  </legend>
  <div class="panel-body">
    <?php print $preview; ?>
  </div>
</fieldset>
