<?php if(!drupal_is_front_page()): ?>
<div class="page-title-wrapper">
  <div class="page-title-section clearfix">
    <?php if (isset($title_image)): ?>
      <div id="page-title-image-wrapper" class="page-title-image-wrapper <?php print $title_image_wrapper_class; ?>" style="background-image:url(<?php print $title_image; ?>);">
        <div id="page-title-image" class="<?php print $title_image_title_class; ?>">
          <h1 id="page-title-image-title"><?php print drupal_get_title(); ?></h1>
        </div>
      </div>
    <?php else: ?>
      <div class="title-wrapper <?php print $title_image_title_class; ?>">
        <h1 class="title <?php if (strlen(drupal_get_title()) > 25) { print 'long-title'; } ?>" id="page-title"><?php print drupal_get_title(); ?></h1>
      </div>
    <?php endif; ?>

  </div>
</div>
<?php endif; ?>

<?php if ($wrapper): ?><div<?php print $attributes; ?>><?php endif; ?>
  <div<?php print $content_attributes; ?>>
    <?php if ($breadcrumb): ?>
      <div id="breadcrumb" class="grid-<?php print $columns; ?>"><?php print $breadcrumb; ?></div>
    <?php endif; ?>

    <?php if ($messages): ?>
      <div id="messages" class="grid-<?php print $columns; ?>"><?php print $messages; ?></div>
    <?php endif; ?>
    <?php print $content; ?>
  </div>
<?php if ($wrapper): ?></div><?php endif; ?>
