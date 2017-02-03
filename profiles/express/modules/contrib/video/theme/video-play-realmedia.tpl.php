<?php
/**
 * @file
 * Theme file to handle realmedia output.
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
<object classid="clsid:CFCDAA03-8BE4-11cf-B84B-0020AFBBCCFA" width="<?php print $width; ?>" height="<?php print $height; ?>">
  <param name="src" value="<?php print $url; ?>" />
  <param name="autostart" value="<?php print $autoplay ? 'true' : 'false'; ?>" />
  <param name="controls" value="imagewindow" />
  <param name="console" value="video" />
  <param name="loop" value="false" />
  <object class="video-object" type="audio/x-pn-realaudio-plugin" data="<?php print file_create_url($video->files->{$video->player}->uri); ?>" width="<?php print $width; ?>" height="<?php print $height; ?>">
    <?php print t('No video? Get the !plugin', array('!plugin' => l(t('Real Media plugin'), url('http://www.real.com/realplayer')))); ?>
  </object>
</object>