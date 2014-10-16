<div id="page" class="container <?php print $classes; ?>">

  <?php print render($page['top_menu']); ?>

  <header id="header" class="clearfix" role="banner">

    <?php if ($site_logo || $site_name || $site_slogan): ?>
      <!-- start: Branding -->
      <div id="branding" class="branding-elements clearfix">

        <?php if ($site_logo): ?>
          <div id="logo">
            <?php print $site_logo; ?>
          </div>
        <?php endif; ?>

        <?php if ($site_name || $site_slogan): ?>
          <!-- start: Site name and Slogan hgroup -->
          <div id="name-and-slogan"<?php print $hgroup_attributes; ?>>

            <?php if ($site_name): ?>
              <h1 id="site-name"<?php print $site_name_attributes; ?>><?php print $site_name; ?></h1>
            <?php endif; ?>

            <?php if ($site_slogan): ?>
              <h2 id="site-slogan"<?php print $site_slogan_attributes; ?>><?php print $site_slogan; ?></h2>
            <?php endif; ?>

          </div><!-- /end #name-and-slogan -->

        <?php endif; ?>

      </div><!-- /end #branding -->
    <?php endif; ?>

    <?php print render($page['header']); ?>
    <?php print render($page['menu_bar']); ?>

  </header>

  <div id="columns"<?php print $page['menu_bar'] ? 'class="no-menu-bar"' : '' ;?>>
    <div class="columns-inner clearfix">

      <?php print $messages; ?>
      <?php print render($page['help']); ?>
      <?php print render($page['secondary_content']); ?>

      <?php if (
        $page['three_33_top'] ||
        $page['three_33_first'] ||
        $page['three_33_second'] ||
        $page['three_33_third'] ||
        $page['three_33_bottom']
        ): ?>
        <!-- Three column 3x33 Gpanel -->
        <div class="at-panel gpanel panel-display three-3x33 clearfix">
          <?php print render($page['three_33_top']); ?>
          <?php print render($page['three_33_first']); ?>
          <?php print render($page['three_33_second']); ?>
          <?php print render($page['three_33_third']); ?>
          <?php print render($page['three_33_bottom']); ?>
        </div>
      <?php endif; ?>

      <?php if ($breadcrumb): print $breadcrumb; endif; ?>

      <div id="content-column">
        <div class="content-inner">

          <?php print render($page['highlighted']); ?>

          <<?php print $tag; ?> id="main-content" role="main">

            <?php print render($title_prefix); ?>
            <?php if ($title || $primary_local_tasks || $secondary_local_tasks || $action_links = render($action_links)): ?>
              <header id="main-content-header">

                <?php if ($title): ?>
                  <h1 id="page-title"<?php print $attributes; ?>>
                    <?php print $title; ?>
                  </h1>
                <?php endif; ?>

                <?php if ($primary_local_tasks || $secondary_local_tasks || $action_links): ?>
                  <div id="tasks">

                    <?php if ($primary_local_tasks): ?>
                      <ul class="tabs primary clearfix"><?php print render($primary_local_tasks); ?></ul>
                    <?php endif; ?>

                    <?php if ($secondary_local_tasks): ?>
                      <ul class="tabs secondary clearfix"><?php print render($secondary_local_tasks); ?></ul>
                    <?php endif; ?>

                    <?php if ($action_links = render($action_links)): ?>
                      <ul class="action-links clearfix"><?php print $action_links; ?></ul>
                    <?php endif; ?>

                  </div>
                <?php endif; ?>

              </header>
            <?php endif; ?>
            <?php print render($title_suffix); ?>

            <!-- region: Main Content -->
            <?php if ($content = render($page['content'])): ?>
              <div id="content">
                <?php print $content; ?>
              </div>
            <?php endif; ?>

            <?php print $feed_icons; ?>

          </<?php print $tag; ?>>

          <?php print render($page['content_aside']); ?>

        </div>
      </div>

      <?php print render($page['sidebar_first']); ?>
      <?php print render($page['sidebar_second']); ?>
      <?php print render($page['tertiary_content']); ?>

    </div>
  </div>

  <?php if (
    $page['footer'] ||
    $page['four_first'] ||
    $page['four_second'] ||
    $page['four_third'] ||
    $page['four_fourth']
    ): ?>
    <footer role="contentinfo">
      <?php if (
        $page['four_first'] ||
        $page['four_second'] ||
        $page['four_third'] ||
        $page['four_fourth']
        ): ?>
        <!-- Four column Gpanel -->
        <div class="at-panel gpanel panel-display four-4x25 clearfix">
          <div class="panel-row row-1 clearfix">
            <?php print render($page['four_first']); ?>
            <?php print render($page['four_second']); ?>
          </div>
          <div class="panel-row row-2 clearfix">
            <?php print render($page['four_third']); ?>
            <?php print render($page['four_fourth']); ?>
          </div>
        </div>
      <?php endif; ?>
      <?php if ($page['footer']): print render($page['footer']); endif; ?>
    </footer>
  <?php endif; ?>

</div><!-- //End #page, .container -->

<?php if ($collapsible = render($page['collapsible'])): ?>
  <section id="section-collapsible" class="section-collapsible clearfix">
    <h2 class="collapsible-toggle"><a href="#"><?php print t('Toggle collapsible region'); ?></a></h2>
    <?php print $collapsible; ?>
  </section>
<?php endif; ?>
