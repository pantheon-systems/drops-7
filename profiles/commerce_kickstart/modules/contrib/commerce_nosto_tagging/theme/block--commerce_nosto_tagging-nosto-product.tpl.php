<?php

/**
 * @file
 * Block template for Nosto product tagging.
 *
 * Available variables:
 * - $nosto_product: The product data object created in preprocessor.
 *
 * @see commerce_nosto_tagging_block_view()
 * @see commerce_nosto_tagging_theme()
 * @see commerce_nosto_tagging_preprocess_nosto_product()
 */
?>

<?php if(isset($nosto_product) && is_object($nosto_product)): ?>
  <div class="nosto_product" style="display:none">
    <span class="url"><?php print check_url($nosto_product->page_url); ?></span>
    <span class="product_id"><?php print (int) $nosto_product->product_id; ?></span>
    <span class="name"><?php print check_plain($nosto_product->name); ?></span>
    <?php if(!empty($nosto_product->image_url)): ?>
      <span class="image_url"><?php print check_url($nosto_product->image_url); ?></span>
    <?php endif; ?>
    <span class="price"><?php print $nosto_product->price; ?></span>
    <span class="price_currency_code"><?php print check_plain($nosto_product->price_currency_code); ?></span>
    <span class="availability"><?php print check_plain($nosto_product->availability); ?></span>
    <?php foreach($nosto_product->categories as $category): ?>
      <span class="category"><?php print check_plain($category); ?></span>
    <?php endforeach; ?>
    <?php if(!empty($nosto_product->description)): ?>
      <span class="description"><?php print $nosto_product->description; ?></span>
    <?php endif; ?>
    <span class="list_price"><?php print $nosto_product->list_price; ?></span>
    <?php if(!empty($nosto_product->brand)): ?>
      <span class="brand"><?php print check_plain($nosto_product->brand); ?></span>
    <?php endif; ?>
    <span class="date_published"><?php print check_plain($nosto_product->date_published); ?></span>
  </div>
<?php endif; ?>
