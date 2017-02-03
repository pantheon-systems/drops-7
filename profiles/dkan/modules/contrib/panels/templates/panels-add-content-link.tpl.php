<?php
/**
 * @file
 * Template to control the add content individual links in the add content modal.
 */
?>
<div class="content-type-button clearfix">
  <?php if (isset($icon_text_button)): ?>
    <?php print $icon_text_button; ?>
  <?php else: ?>
    <?php print $image_button; ?>
    <div><?php print $text_button; ?></div>
  <?php endif; ?>
</div>
