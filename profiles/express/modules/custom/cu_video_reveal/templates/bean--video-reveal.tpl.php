<?php
  $bid = $content['field_video_reveal_url']['#object']->bid;
?>
<div class="video-reveal-wrapper" id="video-reveal-wrapper-<?php print $bid; ?>">
  <div class="video-reveal-container" id="video-reveal-container-<?php print $bid; ?>">
    <a href="#video-reveal-container-<?php print $bid; ?>" class="video-reveal-close" id="video-reveal-close-<?php print $bid; ?>"><i class="fa fa-times-circle"></i></a>
    <?php print render($content['field_video_reveal_url']); ?>
  </div>
  
  <a href="#video-reveal-container-<?php print $bid; ?>" class="video-reveal-trigger" id="video-reveal-trigger-<?php print $bid; ?>"  style="background-image:url('<?php print trim(render($content['field_video_reveal_image'])); ?>'); ">
  
   <div class="video-reveal-teaser">
    <div class="video-reveal-text-wrapper">
      <div class="video-reveal-text"><h3><i class="fa fa-play-circle-o"></i><span class="element-invisible">Play Video</span></h3><br /><?php print render($content['field_video_reveal_text']); ?></div>
    </div>
   </div>
  </a>
</div>
<script type="text/javascript">
  jQuery('a#video-reveal-trigger-<?php print $bid; ?>').click(function(ev){    
    jQuery('#video-reveal-trigger-<?php print $bid; ?>').hide();
    jQuery('#video-reveal-container-<?php print $bid; ?>').fadeIn();
    jQuery('#video-reveal-container-<?php print $bid; ?>').css('position', 'static');
    jQuery('#video-reveal-wrapper-<?php print $bid; ?>').css('height', 'auto');
    
    var iframeSrc = jQuery('#video-reveal-container-<?php print $bid; ?> iframe').attr('src');
    var iframeSrc2 = iframeSrc.replace("autoplay=0", "autoplay=1");
    jQuery('#video-reveal-wrapper-<?php print $bid; ?> iframe').attr('src', iframeSrc2);
    return false;
  });
  jQuery('#video-reveal-close-<?php print $bid; ?>').click(function(){
    jQuery('#video-reveal-container-<?php print $bid; ?>').hide();
    jQuery('#video-reveal-trigger-<?php print $bid; ?>').show();
    var iframeSrc = jQuery('#video-reveal-container-<?php print $bid; ?> iframe').attr('src');
    var iframeSrc2 = iframeSrc.replace("autoplay=1", "autoplay=0");
    jQuery('#video-reveal-wrapper-<?php print $bid; ?> iframe').attr('src', iframeSrc2);
    return false;
  });
</script>