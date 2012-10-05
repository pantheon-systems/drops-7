<?php 
/**
 * @file
 * Alpha's theme implementation to display a zone.
 */
?>
<?php if ($wrapper): ?>
  <div<?php print $attributes; ?>>
    <div class="zone-wrapper-inner">
<?php endif; ?>

  <div<?php print $content_attributes; ?>>
    <?php print $content; ?>
  </div>
  
<?php if ($wrapper): ?>
    </div>
  </div>
<?php endif; ?>