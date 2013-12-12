<?php

/**
 * @file
 * Block template for Nosto completed order tagging.
 *
 * Available variables:
 * - $nosto_order: Object containing all data for the tagging.
 *
 * @see commerce_nosto_tagging_block_view()
 * @see commerce_nosto_tagging_theme()
 * @see commerce_nosto_tagging_preprocess_nosto_order()
 */
?>

<?php if (isset($nosto_order) && is_object($nosto_order)): ?>
  <div class="nosto_purchase_order" style="display:none">
    <span class="order_number"><?php print (int) $nosto_order->order_number; ?></span>
    <div class="buyer">
      <span class="email"><?php print check_plain($nosto_order->user->mail); ?></span>
      <span class="last_name"><?php print check_plain($nosto_order->user->name); ?></span>
    </div>
    <div class="purchased_items">
      <?php foreach ($nosto_order->purchased_items as $line_item): ?>
        <div class="line_item">
          <span class="product_id"><?php print (int) $line_item->product_id; ?></span>
          <span class="quantity"><?php print (int) $line_item->quantity; ?></span>
          <span class="name"><?php print check_plain($line_item->name); ?></span>
          <span class="unit_price"><?php print $line_item->unit_price; ?></span>
          <span class="price_currency_code"><?php print check_plain($line_item->price_currency_code); ?></span>
        </div>
      <?php endforeach; ?>
    </div>
  </div>
<?php endif; ?>
