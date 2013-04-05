<?php

/**
 * @file
 * Default theme implementation to present the title on a product page.
 *
 * Available variables:
 * - $title: The title to render.
 *
 * Helper variables:
 * - $product: The fully loaded product object the title belongs to.
 */
?>
<?php if ($title): ?>
<div class="commerce-product-title">
  <?php print $title; ?>
</div>
<?php endif; ?>
