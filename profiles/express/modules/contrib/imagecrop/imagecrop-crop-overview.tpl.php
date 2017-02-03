<?php
/**
 * @file
 * Imagecrop crop overview template
 *
 */
?>
<div id="imagecrop-selection" class="clearfix">
  <?php print drupal_render($style_selection); ?>
  <div id="imagecrop-edit" class="form-item imagecrop-form-link">
    <?php print l(t('Edit this style'), $edit_url); ?>
  </div>
</div>

<div id="imagecrop-help">
  <?php print t("This is a preview from the crop settings for current style. Click 'Edit this style' to change the crop settings"); ?>
</div>

<div id="imagecrop-preview">
  <?php print $viewed_style; ?>
</div>
