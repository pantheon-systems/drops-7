<?php
/**
 * @file
 * Theme file to handle flash output.
 *
 * Variables passed:
 * $item
 * $width
 * $height
 * $autoplay
 * $autobuffering
 */

$url = check_plain(file_create_url($item['playablefiles'][0]->uri));
$tnurl = '';
if (!empty($item['thumbnailfile'])) {
  $tnurl = check_plain($item['thumbnailfile']->url);
}
?>
<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="<?php print $width; ?>" height="<?php print $height; ?>" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0">
  <param name="movie" value="<?php print $url; ?>" />
  <param name="autoplay" value="<?php print $autoplay ? 'true' : 'false'; ?>" />
  <param name="wmode" value="transparent" />
  <object class="video-object" type="application/x-shockwave-flash" data="<?php print $url; ?>" width="<?php print $width; ?>" height="<?php print $height; ?>">
    <?php print t('No video? Get the !plugin', array('!plugin' => l(t('Adobe Flash plugin'), url('http://get.adobe.com/flashplayer/')))); ?>
  </object>
</object>