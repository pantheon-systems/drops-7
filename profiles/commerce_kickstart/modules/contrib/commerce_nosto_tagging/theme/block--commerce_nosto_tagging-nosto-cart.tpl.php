<?php

/**
 * @file
 * Block template for Nosto cart tagging.
 *
 * Available variables:
 * - $nosto_line_items: Array of line item objects.
 *
 * @see commerce_nosto_tagging_block_view()
 * @see commerce_nosto_tagging_theme()
 * @see commerce_nosto_tagging_preprocess_nosto_cart()
 */
?>

<?php if (isset($nosto_line_items) && is_array($nosto_line_items)): ?>
  <div class="nosto_cart" style="display:none">
    <?php foreach ($nosto_line_items as $nosto_line_item): ?>
      <div class="line_item">
        <span class="product_id"><?php print (int) $nosto_line_item->product_id; ?></span>
        <span class="quantity"><?php print (int) $nosto_line_item->quantity; ?></span>
        <span class="name"><?php print check_plain($nosto_line_item->title); ?></span>
        <span class="unit_price"><?php print $nosto_line_item->unit_price; ?></span>
        <span class="price_currency_code"><?php print check_plain($nosto_line_item->currency_code); ?></span>
      </div>
    <?php endforeach; ?>
  </div>
<?php endif; ?>
