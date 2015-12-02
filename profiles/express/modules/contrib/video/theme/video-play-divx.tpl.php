<?php
/**
 * @file
 * Theme file to handle divx output.
 *
 * Variables passed:
 * $item
 * $width
 * $height
 * $autoplay
 * $autobuffering
 *
 * http://labs.divx.com/Webplayer
 * http://labs.divx.com/WebPlayerCodeGenerator
 */

$url = check_plain(file_create_url($item['playablefiles'][0]->uri));
$tnurl = '';
if (!empty($item['thumbnailfile'])) {
  $tnurl = check_plain($item['thumbnailfile']->url);
}
?>
<object classid="clsid:67DABFBF-D0AB-41fa-9C46-CC0F21721616" width="<?php print $width; ?>" height="<?php print $height; ?>" codebase="http://go.divx.com/plugin/DivXBrowserPlugin.cab">
  <param name="src" value="<?php print $url; ?>" />
  <param value="<?php print $autoplay ? 'true' : 'false'; ?>" name="autoPlay">
  <param name="pluginspage" value="http://go.divx.com/plugin/download/" />
  <param value="none" name="custommode">
  <param name="previewImage" value="<?php print $tnurl; ?>" />
  <object class="video-object" type="video/divx" data="<?php print $url; ?>" previewImage="<?php print $tnurl; ?>" width="<?php print $width; ?>" height="<?php print $height; ?>" autoplay="<?php print $autoplay ? 'true' : 'false'; ?>" mode="large" custommode="none">
    <?php print t('No video? Get the !plugin', array('!plugin' => l(t('DivX Web Player plugin'), url('http://go.divx.com/plugin/download/')))); ?>
  </object>
</object>