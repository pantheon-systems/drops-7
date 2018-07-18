<?php

/**
 * @file
 * TODO: doco.
 */
?>
<?php if (isset($title_image) && !drupal_is_front_page()): ?>
      <div id="page-title-image-wrapper" class="page-title-image-wrapper <?php print $title_image_wrapper_class; ?>" style="background-image:url(<?php print $title_image; ?>);">
        <div id="page-title-image" class="<?php print $title_image_title_class; ?>">
          <h1 id="page-title-image-title"><?php print drupal_get_title(); ?></h1>
        </div>
      </div>
      <div class="clear"></div>
    <?php endif; ?>
<?php if ($wrapper): ?><div<?php print $attributes; ?>><?php endif; ?>
  <div<?php print $content_attributes; ?>>
    <div id="breadcrumb-wrapper" class="grid-<?php print $columns; ?>">
    <?php if ($breadcrumb): ?>
      <div id="breadcrumb"><?php print $breadcrumb; ?></div>
    <?php endif; ?>
    </div>
    
    <?php if ($messages): ?>
      <div id="messages" class="grid-<?php print $columns; ?>"><?php print $messages; ?></div>
    <?php endif; ?>
    <div class="content-sidebar-wrapper clearfix"><?php print $content; ?></div>
  </div>
<?php if ($wrapper): ?></div><?php endif; ?>
