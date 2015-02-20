<?php
/**
 * @file
 * Template file for field_slideshow_controls.
 */
?>
<div id="field-slideshow-<?php print $slideshow_id; ?>-controls" class="field-slideshow-controls">
  <a href="#" class="prev left carousel-control"><i class="fa fa-chevron-left fa-2x glyphicon-chevron-left"></i></a>
  <?php if (!empty($controls_pause)) : ?>
    <a href="#" class="play"><?php print t('Play'); ?></a>
    <a href="#" class="pause"><?php print t('Pause'); ?></a>
  <?php endif; ?>
  <a href="#" class="next right carousel-control"><i class="fa fa-chevron-right fa-2x glyphicon-chevron-right"></i></a>
</div>
