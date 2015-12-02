<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
        "http://www.w3.org/TR/html4/loose.dtd">
<?php
  global $base_url;
  $themepath = $base_url . '/' . drupal_get_path('theme', 'cu_omega');
?> 
<html>
<head>
	<title><?php print $head_title; ?></title>
	<link rel="stylesheet" href="<?php print $themepath; ?>/css/maintenance.css" />
</head>
<body>
 
  
<div class="container">
  <img src="<?php print $themepath; ?>/images/maintenance/cu-logo.png" alt="University of Colorado Boulder" />
  <div class="content">
    <?php print render($content); ?>
  </div>
  <div class="copyright">
    <p>&copy; Regents of the University of Colorado &bull; <a href="http://www.colorado.edu/about/privacy-statement">Privacy</a> &bull; <a href="http://www.colorado.edu/about/legal-trademarks">Legal &amp; Trademarks</a></p>
  </div>
</div> 
</body>
</html>

