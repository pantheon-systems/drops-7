<div class="cu-slider-slide cu-slider-style-1">
  <?php if (isset($slide_image)) { print $slide_image; } ?>
  <?php if(!empty($content['field_slider_caption']) || !empty($content['field_slider_teaser'])): ?>
    <div class="cu-slider-slide-content-wrapper">
      <div class="cu-slider-slide-content element-max-width-padding">
        <?php if (isset($slide_caption)) { print '<div class="cu-slider-caption">' . $slide_caption . '</div>'; } ?>
        <?php
          if (!empty($content['field_slider_teaser'])) {
            print '<div class="cu-slider-teaser">' . render($content['field_slider_teaser']) . '</div>';
          }
        ?>
      </div>
    </div>
  <?php endif; ?>
</div>
