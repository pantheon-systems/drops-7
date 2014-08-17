<!DOCTYPE html>
<html lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>"<?php print $rdf_namespaces;?>>
<head>
   <?php print $head; ?>
   <title><?php print $head_title; ?></title>
  <?php print $styles; ?>

  <?php if (theme_get_setting('responsive_respond','bizreview')): global $base_path; global $base_root; ?>
  <!-- Media Queries support for IE6-8 -->  
  <!--[if lt IE 9]>
    <script src="<?php print $base_root . $base_path . path_to_theme() ?>/js/respond.min.js"></script>
  <![endif]-->
  <?php endif; ?>
  
  <!-- HTML5 element support for IE6-8 -->
  <!--[if lt IE 9]>
    <script src="//html5shiv.googlecode.com/svn/trunk/html5.js"></script>
  <![endif]-->
  <?php print $scripts; ?>
  
 <link type="text/css" rel="stylesheet" media="all" href="<?php print base_path().path_to_theme(); ?>/fontawesome/css/font-awesome.min.css" />
 <link href='http://fonts.googleapis.com/css?family=Raleway:400,500,700,600,800,900,100,200,300' rel='stylesheet' type='text/css'>
   
</head>
<body class="<?php print $classes; ?>" <?php print $attributes;?>>
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>

  <?php print $page_top; ?>
  <?php print $page; ?>
  <?php print $page_bottom; ?>
</body>
</html>
