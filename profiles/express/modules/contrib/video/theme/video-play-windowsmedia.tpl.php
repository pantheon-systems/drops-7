<?php
/**
 * @file
 * Theme file to handle windows media output.
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
<object type="video/x-ms-wmv" data="<?php print $url; ?>" width="<?php print $width; ?>" height="<?php print $height; ?>">
  <param name="src" value="<?php print $url; ?>" valuetype="ref" type="<?php print $url; ?>">
  <param name="animationatStart" value="true">
  <param name="transparentatStart" value="true">
  <param name="autostart" value="<?php print $autoplay ? 'true' : 'false'; ?>">
  <param name="controller" value="1">
  <?php print t('No video? Get the !plugin', array('!plugin' => l(t('Windows Media plugin'), url('http://www.microsoft.com/windows/windowsmedia/player/download/')))); ?>
</object>