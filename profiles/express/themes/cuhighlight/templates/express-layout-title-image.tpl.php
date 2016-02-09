<div class="feature-layout-title-image" style="background-image:url(<?php print $img; ?>)">
  <div class="feature-layout-title-image-overlay">
    <div class="feature-layout-title-image-page-title element-max-width-padding">
      <h1><?php print $title; ?></h1>
      <?php print theme('breadcrumb', array('breadcrumb'=>drupal_get_breadcrumb())); ?>
    </div>
  </div>
</div>
