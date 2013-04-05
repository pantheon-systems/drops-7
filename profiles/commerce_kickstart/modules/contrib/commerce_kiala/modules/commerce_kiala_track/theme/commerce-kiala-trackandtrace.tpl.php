<?php

/**
 * @file
 * Track and trace iframe template.
 *
 * Available variables:
 * - $search_url: url with all parameters
 * - $search_map_width: map width in pixels
 * - $search_map_height: map height in pixels
 *
 * @see template_preprocess_commerce_kiala_point_search_map()
 */
?>
<?php if (!empty($iframe_url)): ?>
<div class="<?php print $classes; ?> clearfix">
  <iframe width="<?php print !empty($iframe_width) ? $iframe_width : '100%'; ?>" height="<?php print !empty($iframe_height) ? $iframe_height : '480'; ?>" src="<?php print $iframe_url; ?>"></iframe>
</div>
<?php endif; ?>
