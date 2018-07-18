<div class="thumbnail-slider-wrapper">
  <div class="cu-slider-has-thumbnails cu-content-slider-style<?php print render($content['field_slider_design_style'][0]['#markup']); ?> cu-content-slider cu-content-slider-image-size-<?php print render($content['field_slider_size'][0]['#markup']); ?>" id="slider-<?php print $bean->bid; ?>">
  <?php
    print render($content['field_slider_slide']);
  ?>
  </div>
  <div class="cu-slider-thumbnails" data-slider-controls="slider-<?php print $bean->bid; ?>" id="slider-thumbnails-<?php print $bean->bid; ?>">
    <?php
      print render($content['field_slider_slide']);
    ?>
  </div>
</div>
