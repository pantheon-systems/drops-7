<?php

/**
 * @file
 * TODO: doco.
 */
?>

<?php if (!empty($footer_menu)): ?>
  <?php if ($wrapper): ?><div id="footer-links-wrapper" class="<?php print $footer_menu_color; ?>"><?php endif; ?>
    <div id="footer-links" class="container-12 clearfix">
      <div class="grid-12 clearfix">
        <?php print theme('links__footer_menu', array('links' => $footer_menu, 'attributes' => array('id' => 'footer-menu-links', 'class' => array('links', 'inline-menu', 'clearfix')), 'heading' => array('text' => t('Footer menu'),'level' => 'h2','class' => array('element-invisible')))); ?>
      </div>
    </div>
  <?php if ($wrapper): ?></div><?php endif; ?>
<?php endif; ?>

<?php if ($wrapper): ?><div<?php print $attributes; ?>><?php endif; ?>
  <div<?php print $content_attributes; ?>>
    <?php print $content; ?>
  </div>
<?php if ($wrapper): ?></div><?php endif; ?>
