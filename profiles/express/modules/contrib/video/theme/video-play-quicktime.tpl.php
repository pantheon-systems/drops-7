<?php
/**
 * @file
 * Theme file to handle quicktime output.
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
<object classid="clsid:02BF25D5-8C17-4B23-BC80-D3488ABDDC6B" codebase="http://www.apple.com/qtactivex/qtplugin.cab#version=7,3,0,0"  width="<?php print $width; ?>" height="<?php print $height; ?>">
  <param name="src" value="<?php print $url; ?>" />
  <param name="controller" value="true" />
  <param name="scale" value="tofit" />
  <param name="autoplay" value="<?php print $autoplay ? 'true' : 'false'; ?>" />
  <param name="pluginurl" value="http://www.apple.com/quicktime/download/" />
  <embed src="<?php print $url; ?>"
         type="video/quicktime"
         pluginspage="http://www.apple.com/quicktime/download/"
         width="<?php print $width; ?>"
         height="<?php print $height; ?>"
         autostart="<?php print $autoplay ? 'true' : 'false'; ?>"
         controller="true" >
  </embed>
</object>