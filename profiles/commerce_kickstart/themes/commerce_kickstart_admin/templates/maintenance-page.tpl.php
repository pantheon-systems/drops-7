<!DOCTYPE html>
<html lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <title><?php print $head_title; ?></title>
    <?php print $head; ?>
    <?php print $styles; ?>
    <?php print $scripts; ?>
  </head>
  <body class="<?php print $classes; ?> <?php print drupal_html_class($title);?>">

  <?php print $page_top; ?>
  <div class="body-wrapper">
  <div class="page-outer">
  <div id="page">
    <div id="branding">
      <?php if ($logo): ?>
      <div class="logo-wrapper">
        <img id="logo" src="<?php print $logo ?>" alt="<?php print $site_name ?>" />
      </div>
      <?php endif; ?>
      <?php if ($title): ?><h1 class="page-title"><?php print $title; ?></h1><?php endif; ?>
    </div>

    <div class="page-wrapper clearfix">

      <?php if ($sidebar_first): ?>
        <div id="sidebar-first" class="sidebar">
          <?php print $sidebar_first ?>
        </div>
      <?php endif; ?>

      <div id="content" class="clearfix">
        <?php if ($messages): ?>
          <div id="console"><?php print $messages; ?></div>
        <?php endif; ?>
        <?php if ($help): ?>
          <div id="help">
            <?php print $help; ?>
          </div>
        <?php endif; ?>
        <?php print $content; ?>
      </div>
    </div>
  </div>
    <?php if (isset($footer)): ?>
      <div id="footer" class="footer-messages clearfix">
        <?php print render($footer); ?>
      </div>
    <?php endif; ?>
  <?php print $page_bottom; ?>
  </div>
  </div>
  </body>
</html>
