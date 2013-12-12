<?php

/**
 * @file
 * Block template for Nosto category tagging.
 *
 * Available variables:
 * - $category: Full path to the current category.
 *
 * @see commerce_nosto_tagging_block_view()
 * @see commerce_nosto_tagging_theme()
 * @see commerce_nosto_tagging_preprocess_nosto_category()
 */
?>

<?php if (isset($category) && is_string($category)): ?>
  <div class="nosto_category" style="display:none"><?php print check_plain($category); ?></div>
<?php endif; ?>
