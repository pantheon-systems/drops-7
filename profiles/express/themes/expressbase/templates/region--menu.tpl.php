<?php if ($main_menu): ?>
  <nav role="navigation" tabindex="-1" class="<?php print $classes; ?>">
    <?php if (theme_get_setting('use_action_menu') && !empty($secondary_menu)): ?>
      <?php print theme('links__system_secondary_menu', array(
            'links' => $secondary_menu,
            'attributes' => array(
              'class' => array('links', 'inline', 'clearfix', 'action-menu'),
            ),
          )); ?>
    <?php endif; ?>
    <?php
    // This code snippet is hard to modify. We recommend turning off the
    // "Main menu" on your sub-theme's settings form, deleting this PHP
    // code block, and, instead, using the "Menu block" module.
    // @see https://drupal.org/project/menu_block
    print theme('links__system_main_menu', array(
      'links' => $main_menu,
      'attributes' => array(
        'class' => array('links', 'inline', 'clearfix'),
        'id' => 'main-menu',
      ),
      'heading' => array(
        'text' => t('Main menu'),
        'level' => 'h2',
        'class' => array('element-invisible'),
      ),
    )); ?>

  </nav>
<?php endif; ?>
