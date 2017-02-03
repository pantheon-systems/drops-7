<div<?php print $attributes; ?>>
  <div<?php print $content_attributes; ?>>
    

      <?php if ($main_menu): ?>
      
      <div id="tablet-menu-toggle">
        <a id="tablet-toggle" href="#zone-menu" title="Menu">Menu <i class="fa fa-reorder"></i></a>
      </div>
      
      
      <div class="navigation">
        <?php print theme('links__system_main_menu', array('links' => $main_menu, 'attributes' => array('id' => 'main-menu', 'class' => array('links', 'inline', 'clearfix')), 'heading' => array('text' => t('Main menu'),'level' => 'h2','class' => array('element-invisible')))); ?>
        
        <?php 
        if (theme_get_setting('use_action_menu')) {
          print theme('links__system_secondary_menu', array('links' => $secondary_menu, 'attributes' => array('id' => 'action-menu', 'class' => array('links', 'inline', 'clearfix')), 'heading' => array('text' => t('Secondary menu'),'level' => 'h2','class' => array('element-invisible')))); 
        }
        ?>
      </div>
      <?php endif; ?>
   

    <?php if (theme_get_setting('alpha_responsive') && isset($mobile_menu) && count($mobile_menu) > 0) : ?>
      <div id="mobile-menu" class="mobile-navigation">
        <?php print theme('links', array('links' => $mobile_menu, 'attributes' => array('id' => 'mobile-menu-nav', 'class' => array('links', 'inline', 'clearfix')), 'heading' => array('text' => t('Mobile menu'),'level' => 'h2','class' => array('element-invisible')))); ?>
      </div>
    <?php elseif (theme_get_setting('alpha_responsive')): ?>
      <div id="mobile-menu" class="mobile-navigation">
        <?php print theme('links__system_main_menu', array('links' => $main_menu, 'attributes' => array('id' => 'main-menu-mobile', 'class' => array('links', 'inline', 'clearfix')), 'heading' => array('text' => t('Main menu'),'level' => 'h2','class' => array('element-invisible')))); ?>
        <?php if ($secondary_menu): ?>
          <?php print theme('links__system_secondary_menu', array('links' => $secondary_menu, 'attributes' => array('id' => 'secondary-menu-mobile', 'class' => array('links', 'inline', 'clearfix')), 'heading' => array('text' => t('Secondary menu'),'level' => 'h2','class' => array('element-invisible')),'mobile'=>TRUE)); ?>
        <?php endif; ?>
      </div>
    <?php endif; ?>
  </div>
</div>
