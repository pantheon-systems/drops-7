<div class="cu-slider cu-content-slider-style<?php print render($content['field_slider_design_style'][0]['#markup']); ?> cu-content-slider cu-content-slider-image-size-<?php print render($content['field_slider_size'][0]['#markup']); ?> <?php if ($slider_ui) { print 'cu-slider-ui'; } ?>">
<?php
  print render($content['field_slider_slide']);
?>
</div>
