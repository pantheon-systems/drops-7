<?php
/**
 * @file
 * Theme file to handle HTML5 output.
 *
 * Variables passed:
 * $width
 * $height
 * $files
 * $item
 * $autoplay
 * $autobuffering
 */

$poster = '';
if (!empty($item['thumbnailfile'])) {
  $poster = check_plain($item['thumbnailfile']->url);
}

$autoplayattr = $autoplay ? ' autoplay="autoplay"' : '';
$preload = $autobuffering ? 'auto' : 'metadata';

$codecs = array(
  'video/mp4' => 'avc1.42E01E, mp4a.40.2',
  'video/webm' => 'vp8, vorbis',
  'video/ogg' => 'theora, vorbis',
  'application/ogg' => 'theora, vorbis',
  'video/ogv' => 'theora, vorbis',
  'video/quicktime' => 'avc1.42E01E, mp4a.40.2',
);
?>
<video width="<?php echo $width; ?>" height="<?php echo $height; ?>" preload="<?php echo $preload; ?>" controls="controls" poster="<?php echo $poster; ?>"<?php echo $autoplayattr; ?>>
<?php
foreach ($files as $index => $file) {
  if (strncmp($file->filemime, 'video/', 6) !== 0) {
    continue;
  }

  $filepath = check_plain(file_create_url($file->uri));

  if ($file->filemime == 'video/quicktime') {
    $file->filemime = 'video/mp4';
  }

  $codecs = '';
  if (isset($codecs[$file->filemime])) {
    $codecs = '; codecs=&quot;' . $codecs[$file->filemime] . '&quot;';
  }
?>
  <source src="<?php echo $filepath; ?>" type="<?php echo $file->filemime . $codecs; ?>" />
<?php
}

echo theme('video_flv', array('item' => $item, 'width' => $width, 'height' => $height));
?>
</video>
