<?php
/**
 * @file
 * Theme file to handle ogg theora output.
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
<applet code="com.fluendo.player.Cortado.class" archive="http://theora.org/cortado.jar" width="<?php print $width; ?>" height="<?php print $height; ?>">
  <param name="url" value="<?php print $url; ?>" />
  <param name="local" value="false" />
  <param name="mode" value="zero" />
  <param name="keepaspect" value="true" />
  <param name="video" value="true" />
  <param name="audio" value="true" />
  <param name="seekable" value="true" />
  <param name="bufferSize" value="200" />
  <param name="autoPlay" value="<?php print $autoplay ? 'true' : 'false'; ?>" />
  <param name="showStatus" value="auto" />
  <param name="showSpeaker" value="true" />
  <?php print t('No video? Get the !plugin', array('!plugin' => l(t('Cortado plugin'), url('http://www.theora.org/cortado/')))); ?>
</applet>