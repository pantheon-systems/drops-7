<?php
/**
 * @file
 * Nuboot's theme implementation to display a single Drupal page.
 */
?>
<div id="nav-wrapper">
  <header id="navbar" role="banner" class="<?php print $navbar_classes; ?>">
    <div class="container">
      <div class="navbar-header">
        <?php if ($logo): ?>
        <a class="logo navbar-btn pull-left" href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>">
          <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
        </a>
        <?php endif; ?>

        <?php if (!empty($site_name)): ?>
        <a class="name navbar-brand" href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>"><?php print $site_name; ?></a>
        <?php endif; ?>

        <!-- .btn-navbar is used as the toggle for collapsed navbar content -->
        <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
          <span class="sr-only">Toggle navigation</span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
          <span class="icon-bar"></span>
        </button>
      </div>

      <?php if (!empty($primary_nav) || !empty($secondary_nav) || !empty($page['navigation'])): ?>
        <div class="navbar-collapse collapse">
          <nav role="navigation">
            <?php if (!empty($primary_nav)): ?>
              <?php print render($primary_nav); ?>
            <?php endif; ?>

            <?php if (!empty($secondary_nav)): ?>
              <?php print render($secondary_nav); ?>
            <?php endif; ?>

            <!-- views exposed search -->
            <?php
            $block = block_load('dkan_sitewide', 'dkan_sitewide_search_bar');
            if($block):
              $search = _block_get_renderable_array(_block_render_blocks(array($block)));
              print render($search);
            endif;
            ?>
            <!-- EOF:views exposed search -->
            <?php if (!empty($page['navigation'])): ?>
              <?php print render($page['navigation']); ?>
            <?php endif; ?>
          </nav>
        </div>
      <?php endif; ?>
    </div>
  </header>
</div><!-- EOF:#nav-wrapper -->

<?php if ($is_front) : ?>
<!-- #jumbotron -->
<div id="jumbotron" class="clearfix">
  <div class="tint"></div>
  <div class="container">

      <!-- #jumbotron-inside -->
      <div id="jumbotron-inside" class="clearfix">
          <div class="row">
              <div class="col-md-12">
              <?php print render($page['jumbotron']); ?>
              </div>
          </div>
          <div class="row">
              <div class="col-md-6">
                <?php print render($page['preface_first']); ?>
              </div>
              <div class="col-md-6">
                <?php print render($page['preface_second']); ?>
              </div>
          </div>
      </div>
      <!-- EOF: #jumbotron-inside -->
  </div>
</div>
<!-- EOF:#jumbotron -->
<?php endif; ?>

<?php if ($page['highlighted']):?>
  <!-- #top-content -->
  <div id="top-content" class="clearfix">
      <div class="container">

          <!-- #top-content-inside -->
          <div id="top-content-inside" class="clearfix">
              <div class="row">
                  <div class="col-md-12">
                  <?php print render($page['highlighted']); ?>
                  </div>
              </div>
          </div>
          <!-- EOF:#top-content-inside -->

      </div>
  </div>
  <!-- EOF: #top-content -->
<?php endif; ?>

<div id="page-wrapper">
  <div class="main-container container">

    <header role="banner" id="page-header">
      <?php if (!empty($site_slogan)): ?>
        <p class="lead"><?php print $site_slogan; ?></p>
      <?php endif; ?>

      <?php print render($page['header']); ?>
    </header> <!-- /#page-header -->

    <div class="row">

      <?php if (!empty($page['sidebar_first'])): ?>
        <aside class="col-sm-3" role="complementary">
          <?php print render($page['sidebar_first']); ?>
        </aside>  <!-- /#sidebar-first -->
      <?php endif; ?>

      <section<?php print $content_column_class; ?>>
        <?php if (!empty($breadcrumb)): print $breadcrumb; endif;?>
        <?php print $messages; ?>
        <?php if (!empty($page['help'])): ?>
          <?php print render($page['help']); ?>
        <?php endif; ?>
        <a id="main-content"></a>
        <?php print render($title_prefix); ?>
        <?php if (!empty($title)): ?>
          <h1 class="page-header"><?php print $title; ?></h1>
        <?php endif; ?>
        <?php print render($title_suffix); ?>
        <?php if (!empty($tabs)): ?>
          <?php print render($tabs); ?>
        <?php endif; ?>
        <?php if (!empty($action_links)): ?>
          <ul class="action-links"><?php print render($action_links); ?></ul>
        <?php endif; ?>
        <?php print render($page['content']); ?>
      </section>

      <?php if (!empty($page['sidebar_second'])): ?>
        <aside class="col-sm-3" role="complementary">
          <?php print render($page['sidebar_second']); ?>
        </aside>  <!-- /#sidebar-second -->
      <?php endif; ?>

    </div>
  </div>

  <?php if ($page['bottom_content']):?>
    <!-- #bottom-content -->
    <div id="bottom-content" class="clearfix">
        <div class="container">

            <!-- #bottom-content-inside -->
            <div id="bottom-content-inside" class="clearfix">
              <div class="row">
                <div class="col-md-12">
                  <?php print render($page['bottom_content']); ?>
                </div>
              </div>
              <div class="row">
                <div class="col-md-6">
                  <?php print render($page['postscript_first']); ?>
                </div>
                <div class="col-md-6">
                  <?php print render($page['postscript_second']); ?>
                </div>
              </div>
            </div>
            <!-- EOF:#bottom-content-inside -->

        </div>
    </div>
    <!-- EOF: #bottom-content -->
  <?php endif; ?>
</div><!-- /#page-wrapper -->

<footer class="footer-wrapper">
  <div class="container">
    <!-- #footer-inside -->
    <div id="footer-inside" class="clearfix">
        <div class="row">
            <div class="col-md-3">
                <?php if ($page['footer_first']):?>
                <div class="footer-area">
                <?php print render($page['footer_first']); ?>
                </div>
                <?php endif; ?>
            </div>

            <div class="col-md-3">
                <?php if ($page['footer_second']):?>
                <div class="footer-area">
                <?php print render($page['footer_second']); ?>
                </div>
                <?php endif; ?>
            </div>

            <div class="col-md-3">
                <?php if ($page['footer_third']):?>
                <div class="footer-area">
                <?php print render($page['footer_third']); ?>
                </div>
                <?php endif; ?>
            </div>

            <div class="col-md-3">
                <?php if ($page['footer_fourth']):?>
                <div class="footer-area">
                <?php print render($page['footer_fourth']); ?>
                </div>
                <?php endif; ?>
            </div>
        </div>
        <?php print render($page['footer']); ?>
    </div>
    <!-- EOF: #footer-inside -->
  </div><!--/container -->
</footer>
