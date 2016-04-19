<?php

/**
 * @file
 * Template for a block.
 */
?>
<div class="<?php print $classes; ?>"<?php print $attributes; ?>>

  <?php print render($title_prefix); ?>
  <?php if ($block->subject): ?>
    <h4 class="block__title"<?php print $title_attributes; ?>><?php print $block->subject ?></h4>
  <?php endif;?>
  <?php print render($title_suffix); ?>

  <div class="block__content"<?php print $content_attributes; ?>>
    <?php print $content ?>
  </div>
</div>
