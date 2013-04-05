<?php

/**
 * @file
 * Default implementation of the Kiala Point Serach Map.
 *
 * Available variables:
 * - $search_url: url with all parameters
 * - $search_map_width: map width in pixels
 * - $search_map_height: map height in pixels
 *
 * @see template_preprocess_commerce_kiala_point_search_map()
 */
?>
<?php if (!empty($search_map_url)): ?>
<div class="<?php print $classes; ?> clearfix">
  <iframe width="<?php print !empty($search_map_width) ? $search_map_width : '100%'; ?>" height="<?php print !empty($search_map_height) ? $search_map_height : '480'; ?>" src="<?php print $search_map_url; ?>"></iframe>
</div>
<?php endif; ?>
