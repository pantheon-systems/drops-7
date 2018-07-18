<?php
  // Unique identifier for this video reveal block
  $bid = $content['field_video_reveal_url']['#object']->bid;
  // strip tags this text will all be linked.
  $text = strip_tags(render($content['field_video_reveal_text']), '<div><p><strong><em><h2><h3><h4><h5><h6><br>');
?>
<div class="video-reveal-wrapper" id="video-reveal-wrapper-<?php print $bid; ?>" style="background-image:url('<?php print trim(render($content['field_video_reveal_image'])); ?>'); ">
  <a href="#video-reveal-container-<?php print $bid; ?>" class="video-reveal-trigger" id="video-reveal-trigger-<?php print $bid; ?>">
    <div class="video-reveal-teaser">
      <div class="video-reveal-text element-max-width-padding">
          <h3 class="play-icon"><i class="fa fa-play-circle-o"></i><span class="element-invisible">Play Video</span></h3><br /><?php print $text; ?>
      </div>
    </div>
  </a>
</div>
<div class="video-reveal-container" id="video-reveal-container-<?php print $bid; ?>">
  <a href="#video-reveal-container-<?php print $bid; ?>" class="video-reveal-close" id="video-reveal-close-<?php print $bid; ?>"><i class="fa fa-times-circle"></i></a>
  <?php print render($content['field_video_reveal_url']); ?>
</div>
<script type="text/javascript">
  jQuery('a#video-reveal-trigger-<?php print $bid; ?>').click(function(ev){
    jQuery('#video-reveal-wrapper-<?php print $bid; ?> .video-reveal-teaser').hide();
    jQuery('#video-reveal-container-<?php print $bid; ?>').fadeIn();
    var iframeSrc = jQuery('#video-reveal-container-<?php print $bid; ?> iframe').attr('src');
    var iframeSrc2 = iframeSrc.replace("autoplay=0", "autoplay=1");
    jQuery('#video-reveal-container-<?php print $bid; ?> iframe').attr('src', iframeSrc2);
    return false;
  });
  jQuery('#video-reveal-close-<?php print $bid; ?>').click(function(){
    jQuery('#video-reveal-container-<?php print $bid; ?>').hide();
    jQuery('#video-reveal-wrapper-<?php print $bid; ?> .video-reveal-teaser').show();
    var iframeSrc = jQuery('#video-reveal-container-<?php print $bid; ?> iframe').attr('src');
    var iframeSrc2 = iframeSrc.replace("autoplay=1", "autoplay=0");
    jQuery('#video-reveal-container-<?php print $bid; ?> iframe').attr('src', iframeSrc2);
    return false;
  });
</script>
