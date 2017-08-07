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

<div class="<?php print $bundle['classes']; ?> col-lg-3 col-md-4 col-sm-6 col-xs-12">
  <div class="module-bundle">
    <h3 class="label"><?php print $bundle['title']; ?><?php if (isset($bundle['enabled'])) { print ' <span class="element-invisible">Enabled</span>'; } ?></h3>
    <div class="module-bundle-description">
      <?php print $bundle['description']; ?>
    </div>
    <div class="bundle-actions">
      <?php print $bundle['actions']; ?>
    </div>
  </div>
</div>
