<?php
  $label = theme_get_setting('secondary_menu_label') ? theme_get_setting('secondary_menu_label') : '';
?>

<div<?php print $attributes; ?>>
  <div<?php print $content_attributes; ?>>
    <?php if (isset($secondary_menu)): ?>
    <div class="secondary-navigation-wrapper clearfix"><div class="secondary-navigation">
      <?php print theme('links__system_secondary_menu', array('links' => $secondary_menu, 'attributes' => array('id' => 'secondary-menu', 'class' => array('links', 'inline', 'clearfix')), 'heading' => array('text' => t($label),'level' => 'h2','class' => array('inline', 'secondary-menu-label')))); ?>
    </div></div>
    <?php endif; ?>
  </div>
</div>
