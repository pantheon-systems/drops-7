<?php
/**
 * @file
 * Imagecrop html file.
 */
header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML+RDFa 1.0//EN"
  "http://www.w3.org/MarkUp/DTD/xhtml-rdfa-1.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language; ?>" version="XHTML+RDFa 1.0" dir="<?php print $language->dir; ?>"<?php print $rdf_namespaces; ?>>

<head profile="<?php print $grddl_profile; ?>">
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>
  <?php print $styles; ?>
  <?php print $scripts; ?>
</head>
<body class="<?php print $classes; ?>" <?php print $attributes;?>>
  <?php if (variable_get('imagecrop_show_cancel_button', FALSE) && variable_get('imagecrop_popup', 'basic') != 'imagecrop_iframe'): ?>
  <a id="cancel-crop" href="#" title="<?php print t('Cancel cropping')?>"><?php print t('Cancel cropping')?></a>
  <?php endif; ?>
  <?php print $page; ?>
</body>
</html>