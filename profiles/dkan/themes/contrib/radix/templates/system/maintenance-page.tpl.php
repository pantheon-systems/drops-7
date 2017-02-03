<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
  <head>
    <title><?php print $head_title; ?></title>
    <?php print $head; ?>
    <?php print $styles; ?>
    <?php print $scripts; ?>
  </head>
  <body class="<?php print $classes; ?>">
    <?php if (!empty($site_name)): ?>
      <div class="jumbotron">
        <div class="container">
          <h1><?php print $site_name; ?></h1>
        </div>
      </div>
    <?php endif; ?>
    <main class="main container">
      <div class="row">
        <?php if (!empty($sidebar_first)): ?>
          <div class="col-md-3 sidebar hidden-sm hidden-xs">
            <?php print $sidebar_first; ?>
          </div>
        <?php endif ?>
        <div class="col-md-9">
          <?php if (!empty($title)): ?>
            <div class="page-header clearfix">
              <h1 class="pull-left"><?php print $title; ?></h1>
              <?php if (isset($steps)): ?>
                <h4 class="pull-right"><?php print $steps; ?></h4>
              <?php endif; ?>
            </div>
          <?php endif; ?>
          <?php print $content; ?>
        </div>
      </div>
    </main> <!-- /#main -->
  </body>
</html>