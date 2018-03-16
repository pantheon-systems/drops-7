<?php

/**
 * @file
 * Template file for Radix Grid.
 */
?>
<?php if (count($items)): ?>
  <div class="row">
    <?php foreach ($items as $item): ?>
      <div class="<?php print $item['classes']; ?>">
        <?php print render($item['content']); ?>
      </div>
    <?php endforeach; ?>
  </div>
<?php endif; ?>
