<?php

/**
 * @file
 * Block template for "Nosto Elements".
 *
 * Available variables:
 * - $nosto_id: The id of the element that is configurable in the block settings.
 *
 * @see commerce_nosto_tagging_block_view()
 * @see commerce_nosto_tagging_theme()
 */
?>

<?php if (isset($nosto_id) && is_string($nosto_id)): ?>
  <div class="nosto_element" id="<?php print check_plain($nosto_id); ?>"></div>
<?php endif; ?>
