<?php

/**
 * Variables:
 *
 * $bundles
 * $bundle['title'] : Title of bundle
 * $bundle['description'] : Description of bundle
 * $bundle['actions'] : Actions of bundle (such as enable, etc)
 * $bundle['classes'] : CSS classes for bundle
 *
 */
?>

<div class="<?php print $bundle['classes']; ?>">
  <div class="module-bundle">
    <span class="label"><?php print $bundle['title']; ?></span>
    <div class="module-bundle-description">
      <?php print $bundle['description']; ?>
    </div>
    <div class="bundle-actions">
      <?php print $bundle['actions']; ?>
    </div>
  </div>
</div>
