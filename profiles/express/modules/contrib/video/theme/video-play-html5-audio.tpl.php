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

$autoplayattr = $autoplay ? ' autoplay="autoplay"' : '';
$preload = $autobuffering ? 'auto' : 'metadata';
?>
<audio preload="<?php echo $preload; ?>" controls="controls""<?php echo $autoplayattr; ?>>
<?php
foreach ($files as $index => $file) {
  if (strncmp($file->filemime, 'audio/', 6) !== 0) {
    continue;
  }
?>
  <source src="<?php echo check_plain(file_create_url($file->uri)); ?>" type="<?php echo $file->filemime; ?>" />
<?php
}

echo theme('video_flv', array('item' => $item, 'width' => $width, 'height' => $height));
?>
</audio>
