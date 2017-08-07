<div class="cu-slider-slide cu-slider-style-4 clearfix">
  <div class="cu-slider-slide-row row">
    <div class="cu-slider-slide-image col-lg-8 col-md-8 col-sm-8 col-xs-12">
      <?php if (isset($slide_image)) { print $slide_image; } ?>
    </div>
    <div class="cu-slider-slide-content-wrapper col-lg-4 col-md-4 col-sm-4 col-xs-12">
      <div class="cu-slider-slide-content">
        <?php if (isset($slide_caption)) { print '<div class="cu-slider-caption">' . $slide_caption . '</div>'; } ?>
        <?php
          if (!empty($content['field_slider_teaser'])) {
            print '<div class="cu-slider-teaser">' . render($content['field_slider_teaser']) . '</div>';
          }
        ?>
      </div>
    </div>
  </div>
</div>
