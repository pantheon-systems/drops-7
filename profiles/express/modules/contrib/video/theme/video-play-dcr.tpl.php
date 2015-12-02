<?php
/**
 * @file
 * Theme file to handle director output.
 *
 * Variables passed:
 * $item
 * $width
 * $height
 * $autoplay
 * $autobuffering
 */

$url = check_plain(file_create_url($item['playablefiles'][0]->uri));
?>
<object classid="clsid:166B1BCA-3F9C-11CF-8075-444553540000" type="application/x-director" width="<?php print $width; ?>" height="<?php print $height; ?>" codebase="http://download.macromedia.com/pub/shockwave/cabs/director/sw.cab#version=10,0,0,0">
  <param name="src" value="<?php print $url; ?>" />
  <object class="video-object" type="application/x-director" data="<?php print $url; ?>" width="<?php print $width; ?>" height="<?php print $height; ?>" mode="zero">
    <?php print t('No video? Get the !plugin', array('!plugin' => l(t('Director plugin'), url('http://www.macromedia.com/shockwave/download/')))); ?>
  </object>
</object>