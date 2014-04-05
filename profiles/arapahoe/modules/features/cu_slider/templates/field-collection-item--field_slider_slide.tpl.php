<?php
  global $cu_slider_image_size;
  $content['field_slider_image'][0]['#image_style'] = $cu_slider_image_size;
?>
<?php

  // prepare image
  if (!empty($content['field_slider_url'])) {
    if (!empty($content['field_slider_image'])) {
      $image = l(render($content['field_slider_image']), $content['field_slider_url'][0]['#markup'], array('html' => TRUE));
    }
    if (!empty($content['field_slider_caption'])) {
      $text = $content['field_slider_caption'][0]['#markup'] . '&nbsp;<i class="fa fa-external-link-square"></i>';
      $caption = l($text, $content['field_slider_url'][0]['#markup'], array('html' => TRUE));
    }
  } else {
    if (!empty($content['field_slider_image'])) {
      $image = render($content['field_slider_image']);
    }
    if (!empty($content['field_slider_caption'])) {
      $caption = render($content['field_slider_caption']);
    }
  }

  // prepare design
  if (!empty($content['field_slider_slide_layout'])) {
    $design = render($content['field_slider_slide_layout'][0]['#markup']);
  } else {
    $design = 'slide-layout-bottom';
  }

  // prepare background
  if (!empty($content['field_slider_show_background'])) {
    $background = render($content['field_slider_show_background'][0]['#markup']);
  } else {
    $background = 'slide-background-show';
  }

?>

<div class="cu-slider-slide <?php print $design; ?> <?php print $background; ?> clearfix">
  <?php if (isset($image)) { print $image; } ?>
  <?php if(!empty($content['field_slider_caption']) || !empty($content['field_slider_teaser'])): ?>
  <div class="cu-slider-slide-content-wrapper">
    <div class="cu-slider-slide-content-inner">
      <div class="cu-slider-slide-content">
        <?php if (isset($caption)) { print '<div class="cu-slider-caption">' . $caption . '</div>'; } ?>
        <?php
          if (!empty($content['field_slider_teaser'])) {
            print '<div class="cu-slider-teaser">' . render($content['field_slider_teaser']) . '</div>';
          }
        ?>
      </div>
    </div>
  </div>
  <?php endif; ?>
</div>
