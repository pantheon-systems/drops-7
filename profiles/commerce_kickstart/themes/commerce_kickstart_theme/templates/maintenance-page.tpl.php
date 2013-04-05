<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <title><?php print $head_title; ?></title>
    <?php print $head; ?>
    <?php print $styles; ?>
    <?php print $scripts; ?>
    <link rel="stylesheet" href="<?php print base_path() . path_to_theme();?>/css/global.css"/>
  </head>
  <body class="<?php print $classes; ?>">

  <?php print $page_top; ?>
  <div class="body-wrapper">
  <div id="branding">
    <?php if ($logo): ?>
    <div class="logo-wrapper">
      <img id="logo" src="<?php print $logo ?>" alt="<?php print $site_name ?>" />
    </div>
    <?php endif; ?>
  </div>

  <div id="page">
    <div class="page-wrapper clearfix">
      <div id="content" class="clearfix">
        <?php if ($title): ?><h1 class="page-title"><?php print $title; ?></h1><?php endif; ?>
        <?php print $content; ?>
        <?php if ($messages): ?>
        <div id="messages"><div class="section clearfix">
          <?php print $messages; ?>
        </div></div>
        <?php endif; ?>
      </div>
    </div>
  </div>
  </div>
  </body>
</html>
