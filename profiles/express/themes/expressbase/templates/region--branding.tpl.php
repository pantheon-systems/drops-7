<div id="print-header" class="clearfix">
  <div class="print-site-name">
    <?php print $site_name; ?>
  </div>
  <div class="print-site-logo">
    <?php print $print_logo; ?>
  </div>
</div>
<?php
  // Markup for site name
  $site_name_tag = drupal_is_front_page() ? 'h1' : 'div';
?>
<?php if (variable_get('custom_white_logo') && variable_get('custom_black_logo')): ?>
  <?php
    // Load logo files
    $custom_logo = array(
      'white' => file_load(variable_get('custom_white_logo')),
      'black' => file_load(variable_get('custom_black_logo')),
    );
    // Create img markup
    $custom_logo['white']->markup = '<img class="custom-logo custom-logo-white" src="' . file_create_url($custom_logo['white']->uri) . '" alt="' . check_plain($site_name) . ' logo" />';
    $custom_logo['black']->markup = '<img class="custom-logo custom-logo-black" src="' . file_create_url($custom_logo['black']->uri) . '" alt="' . check_plain($site_name) . ' logo" />';
  ?>
  <?php
    // Link images to <front> and add site name
    print l($custom_logo['white']->markup, '<front>', array('attributes' => array('rel' => 'home', 'title' => check_plain($site_name), 'class' => array('custom-logo-link')), 'html' => TRUE));
    print l($custom_logo['black']->markup, '<front>', array('attributes' => array('rel' => 'home', 'title' => check_plain($site_name), 'class' => array('custom-logo-link')), 'html' => TRUE));
  ?>
  <div class="element-invisible">
    <?php if ($site_name || $site_slogan): ?>
      <div class="header__name-and-slogan" id="name-and-slogan">
        <?php if ($site_name): ?>
          <<?php print $site_name_tag; ?> class="header__site-name" id="site-name">
            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" class="header__site-link" rel="home"><span><?php print $site_name; ?></span></a>
          </<?php print $site_name_tag; ?>>
        <?php endif; ?>

        <?php if ($site_slogan): ?>
          <div class="header__site-slogan" id="site-slogan"><?php print $site_slogan; ?></div>
        <?php endif; ?>
      </div>
    <?php endif; ?>
  </div>
<?php else: ?>

  <?php if ($logo): ?>
    <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" class="header__logo"><img src="<?php print $svg_logo; ?>" alt="<?php print t('Home'); ?>" class="header__logo-image" id="logo" /></a>
  <?php endif; ?>

  <?php if ($site_name || $site_slogan): ?>
    <div class="header__name-and-slogan" id="name-and-slogan">
      <?php if ($site_name): ?>
        <<?php print $site_name_tag; ?> class="header__site-name" id="site-name">
          <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" class="header__site-link" rel="home"><span><?php print $site_name; ?></span></a>
        </<?php print $site_name_tag; ?>>
      <?php endif; ?>

      <?php if ($site_slogan): ?>
        <div class="header__site-slogan" id="site-slogan"><?php print $site_slogan; ?></div>
      <?php endif; ?>
    </div>
  <?php endif; ?>
<?php endif; ?>
