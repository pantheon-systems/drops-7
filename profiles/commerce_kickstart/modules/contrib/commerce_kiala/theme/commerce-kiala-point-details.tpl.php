<?php

/**
 * @file
 * Default implementation of the shopping cart block template.
 *
 * Available variables:
 * - $point_details: A Kiala Point details array
 *    @ref http://locateandselect.kiala.com/info/ws
 *    - shortID
 *    - status
 *    - address
 *    - remark
 *    - openingHours
 *    - picture
 *    - coordinate
 *    - distance
 *    - label
 *
 * - $available: TRUE if point is available
 * - $status_message: The point's status message vale
 * - $picture_image: rendered image tag of the point's picture
 * - $show_more_link: TRUE to show view more link
 * - $more_link_text: more link text
 * - $more_link_url: full url to view more
 *
 * @see template_preprocess_commerce_kiala_point_details()
 */
?>
<?php if (!empty($point_details)):?>
<?php if (!empty($point_title)):?>
<div class="kiala-point-title"><?php print $point_title; ?></div>
<?php endif;?>
<div class="<?php print $classes; ?> clearfix">
  <?php if (!empty($picture_image)): ?>
    <div class="kiala-point-picture">
      <?php print $picture_image; ?>
    </div>
  <?php endif; ?>
  <div class="kiala-point-information">
  <strong class="kiala-point-name"><?php print $point_details['name']; ?></strong>
  <?php if (!empty($point_details['address'])):?>
    <div class="kiala-point-address">
        <div class="street">
          <?php print $point_details['address']['street'];?>
        </div>
        <div class="city-zip">
          <?php print $point_details['address']['city'];?>, <?php print $point_details['address']['zip'];?>
        </div>
    </div>
  <?php endif;?>
  <?php if (!empty($point_details['address']['locationHint'])): ?>
    <div class="kiala-point-location-hint">
      <?php print $point_details['address']['locationHint']; ?>
    </div>
  <?php endif; ?>
   <?php if (!empty($status_message)): ?>
    <div class="kiala-point-status">
      <?php print $status_message; ?>
    </div>
  <?php endif; ?>
   <?php if (!empty($show_more_link)): ?>
    <div class="more-link kiala-point-more-link">
      <?php print $more_link; ?>
    </div>
  <?php endif; ?>
  </div>
</div>
<?php endif; ?>
