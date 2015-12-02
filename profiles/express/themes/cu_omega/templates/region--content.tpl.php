<?php

/**
 * @file
 * TODO: doco.
 */
?>

<div<?php print $attributes; ?>>
  <div<?php print $content_attributes; ?>>
      <a id="main-content"></a>
      <?php print render($title_prefix); ?>
      <?php if (isset($title)): ?>
      <?php if ($title_hidden || isset($has_title_image)): ?><div class="element-hidden"><?php endif; ?>
      <h1 class="title" id="page-title"><?php print $title; ?></h1>
     <?php if ($title_hidden || isset($has_title_image)): ?></div><?php endif; ?>
      <?php endif; ?>
      <?php print render($title_suffix); ?>
   <?php if (isset($tabs) && !empty($tabs['#primary'])): ?><div class="tabs clearfix"><?php print render($tabs); ?></div><?php endif; ?>
   <?php if (isset($action_links) && !empty($action_links)): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>
    <?php print $content; ?>
    <?php if (isset($feed_icons)): ?><div class="feed-icon clearfix"><?php print $feed_icons; ?></div><?php endif; ?>
  </div>
</div>
