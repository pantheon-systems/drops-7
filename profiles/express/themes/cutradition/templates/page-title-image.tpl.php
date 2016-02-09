<?php
  // Get width config
  $width = theme_get_setting('page_title_image_width') ? theme_get_setting('page_title_image_width') : 'page-title-image-width-content';

  if ($width == 'page-title-image-width-content'):
?>

  <div id="page-title-image-section" class="page-title-image-boxed ">
    <div id="page-title-image-wrapper" class="element-max-width-padding" style="background-image:url(<?php print $title_image; ?>);">
      <div id="page-title-image-overlay">
        <div id="page-title-image-inner" class="element-max-width-padding1">
          <h1 id="page-title-image-title"><?php print drupal_get_title(); ?></h1>
          <div class="breadcrumb-wrapper">
            <?php print theme('breadcrumb', array('breadcrumb'=>drupal_get_breadcrumb())); ?>
          </div>
        </div>
      </div>
    </div>
  </div>
<?php else: ?>
  <div id="page-title-image-section" class="page-title-image-wide">
    <div id="page-title-image-wrapper" class=" " style="background-image:url(<?php print $title_image; ?>);">
      <div id="page-title-image-overlay">
        <div id="page-title-image-inner" class="element-max-width-padding">
          <h1 id="page-title-image-title">
            <?php print drupal_get_title(); ?></h1>
            <div class="breadcrumb-wrapper">
              <?php print theme('breadcrumb', array('breadcrumb'=>drupal_get_breadcrumb())); ?>
            </div>
        </div>
      </div>
    </div>
  </div>
<?php endif; ?>
