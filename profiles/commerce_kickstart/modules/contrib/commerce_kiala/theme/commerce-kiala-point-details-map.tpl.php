<?php

/**
 * @file
 * Default implementation of the Kiala Points Details map.
 *
 * Available variables:
 * - $map_url: url with all parameters
 * - $map_width: map width in pixels
 * - $map_height: map height in pixels
 *
 * @see template_preprocess_commerce_kiala_point_search_map()
 */
?>
<?php if (!empty($map_url)): ?>
<div class="<?php print $classes; ?> clearfix">
  <a href="<?php print $map_url; ?>" target="_blank"><?php print t('View Full Page'); ?></a>
  <iframe width="<?php print !empty($map_width) ? $map_width : '100%'; ?>" height="<?php print !empty($map_height) ? $map_height : '480'; ?>" src="<?php print $map_url; ?>"></iframe>
</div>
<?php endif; ?>
