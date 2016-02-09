<div id="page" class="<?php print $classes; ?>">
  <?php if (!empty($page['alerts'])): ?>
  <div id="alerts-wide-wrapper" class="section-wrapper">
    <div id="alerts" class="clearfix">
      <?php print render($page['alerts']); ?>
    </div>
  </div>
  <?php endif; ?>

  <div id="header-wrapper" class="section-wrapper">
    <header class="header container-max clearfix" id="header" role="banner">
      <div id="branding" class="col-lg-8 col-md-8 col-sm-12 col-xs-12">
        <?php print render($page['branding']); ?>
        <div class="mobile-menu-toggle">
          <a id="toggle" href="#zone-menu" title="Menu">Menu <i class="fa fa-reorder fa-fw"></i></a>
        </div>
      </div>

      <div id="search"class="col-lg-4 col-md-4">
        <?php print render($page['search_box']); ?>
      </div>
    </header>
  </div>
  <?php if (theme_get_setting('use_action_menu') == FALSE): ?>
  <div id="secondary-menu-wrapper" class="section-wrapper">
    <div id="secondary-navigation" class="container-max">
      <div class="secondary-nav-inner col-lg-12 col-md-12 clearfix">
        <?php print render($page['secondary_menu']); ?>
      </div>
    </div>
  </div>
  <?php endif; ?>
  <div id="main-menu-wrapper" class="section-wrapper">
    <div id="navigation" class="container-max">
      <div class="nav-inner col-lg-12 col-md-12 clearfix">
        <?php print render($page['menu']); ?>
      </div>
    </div>
  </div>
  <div id="mobile-navigation-wrapper">


    <div id="mobile-navigation">
      <div id="mobile-search">
        <?php print render($page['search_box']); ?>
      </div>
      <nav id="mobile-menu" role="navigation">
      <?php if (isset($mobile_menu) && !empty($mobile_menu)): ?>
        <?php print theme('links', array('links' => $mobile_menu, 'attributes' => array('id' => 'main-menu-mobile', 'class' => array('links', 'clearfix')), 'heading' => array('text' => t('Mobile menu'),'level' => 'h2','class' => array('element-invisible')))); ?>

      <?php else: ?>
        <?php print theme('links', array('links' => $main_menu, 'attributes' => array('id' => 'main-menu-mobile', 'class' => array('links', 'clearfix')), 'heading' => array('text' => t('Mobile menu'),'level' => 'h2','class' => array('element-invisible')))); ?>
        <?php print theme('links', array('links' => $secondary_menu, 'attributes' => array('id' => 'secondary-menu-mobile', 'class' => array('links', 'clearfix')), )); ?>
        <?php print theme('links', array('links' => $footer_menu, 'attributes' => array('id' => 'footer-menu-mobile', 'class' => array('links', 'clearfix')), )); ?>
      <?php endif; ?>
      </nav>
    </div>
  </div>

  <?php if (!empty($page['intro'])): ?>
  <div id="intro-wide-wrapper" class="section-wrapper">
    <?php print render($page['intro']); ?>
  </div>
  <?php endif; ?>

  <?php if (!empty($page['slider'])): ?>
  <div id="slider-wrapper" class="section-wrapper <?php if (!empty($page['slider_sidebar'])) { print 'has-slider-sidebar'; } ?>">
    <div id="slider" class="clearfix container-max">
      <?php print render($page['slider']); ?>
    </div>
  </div>
  <?php endif; ?>

  <?php if (isset($title_image) && !drupal_is_front_page()): ?>
    <div id="page-title-image-wrapper" class="section-wrapper" style="background-image:url(<?php print $title_image; ?>);">
      <div id="page-title-image" class="element-max-width-padding">
        <div class="page-title-image-inner">
          <h1 id="page-title-image-title"><?php print drupal_get_title(); ?></h1>
          <div>
            <?php print $breadcrumb; ?>
          </div>
        </div>
      </div>
    </div>
    <div class="clear"></div>
  <?php else: ?>
    <div id="page-title-wrapper" class="section-wrapper <?php if (isset($title_hidden)) { print 'element-invisible'; } ?>">
      <div class="element-max-width-padding">
        <h1 id="page-title-image-title"><?php print drupal_get_title(); ?></h1>
        <?php print $breadcrumb; ?>
      </div>
    </div>
    <div class="clear"></div>
  <?php endif; ?>

  <div id="content-wrapper" class="section-wrapper">
    <div id="main" class="clearfix container-max">
      <div id="content" class="<?php print $main_content_classes; ?>" role="main">

        <a id="main-content"></a>
        <?php print $messages; ?>
        <div class="tabs"><?php print render($tabs); ?></div>
        <?php print render($page['help']); ?>
        <?php if ($action_links): ?>
          <ul class="action-links"><?php print render($action_links); ?></ul>
        <?php endif; ?>
        <?php print render($page['content']); ?>
        <?php print $feed_icons; ?>
      </div>

      <?php
        // Render the sidebars to see if there's anything in them.
        $sidebar_first  = render($page['sidebar_first']);
        $sidebar_second = render($page['sidebar_second']);
      ?>

      <?php
        // Order the sidebars depending on which is the primary sidebar
        if ($sidebar_first || $sidebar_second): ?>
        <aside class="sidebars">
          <?php if (theme_get_setting('primary_sidebar') == 'primary-sidebar-first'): ?>
            <div class="sidebar-first sidebar <?php print $sidebar_first_classes; ?>">
              <?php print $sidebar_first; ?>
            </div>
            <div class="sidebar-second sidebar <?php print $sidebar_second_classes; ?>">
              <?php print $sidebar_second; ?>
            </div>
          <?php else: ?>
            <div class="sidebar-second sidebar <?php print $sidebar_second_classes; ?>">
              <?php print $sidebar_second; ?>
            </div>
            <div class="sidebar-first sidebar <?php print $sidebar_first_classes; ?>">
              <?php print $sidebar_first; ?>
            </div>
          <?php endif; ?>
        </aside>
      <?php endif; ?>
    </div>
  </div>

  <?php if (!empty($page['feature_layout'])): ?>
    <div id="feature-layout-wrapper" class="section-wrapper">
      <?php print render($page['feature_layout']); ?>
    </div>
  <?php endif; ?>

  <?php if (!empty($page['wide_2'])): ?>
    <div id="post-wide-wrapper" class="section-wrapper">
      <?php print render($page['wide_2']); ?>
    </div>
  <?php endif; ?>

  <?php if (!empty($page['after_content'])): ?>
    <div id="after-content-wrapper" class="section-wrapper">
      <div id="after-content" class="container-max">
        <?php print render($page['after_content']); ?>
      </div>
    </div>
  <?php endif; ?>

  <?php if (!empty($page['lower'])): ?>
    <div id="after-content2-wrapper" class="section-wrapper">
      <div id="after-content-2" class="container-max">
        <?php print render($page['lower']); ?>
      </div>
    </div>
  <?php endif; ?>


  <div id="footer-section">
    <?php if (!empty($page['footer'])): ?>
      <div id="footer-wrapper" class="section-wrapper">
        <div id="footer" class="container-max">
          <div class="col-lg-12 col-md-12">
            <div class="row">
              <?php print render($page['footer']); ?>
            </div>
          </div>
        </div>
      </div>
    <?php endif; ?>

    <?php if (isset($footer_menu) && !empty($footer_menu)): ?>
      <div id="footer-menu-wrapper" class="section-wrapper <?php print $footer_menu_color; ?>">
        <div id="footer-navigation" class="container-max">
          <div class="nav-inner col-lg-12 col-md-12 clearfix">
            <nav id="footer-menu">
            <?php print theme('links__footer_menu', array('links' => $footer_menu, 'attributes' => array('id' => 'footer-menu-links', 'class' => array('links', 'inline-menu', 'clearfix')), 'heading' => array('text' => t('Footer menu'),'level' => 'h2','class' => array('element-invisible')))); ?>
            </nav>
          </div>
        </div>
      </div>
    <?php endif; ?>
    <div id="site-info-wrapper" class="section-wrapper">
      <div id="site-info" class="container-max">
        <?php print render($page['site_info']); ?>
      </div>
    </div>
  </div>
</div>
<?php print render($page['bottom']); ?>
