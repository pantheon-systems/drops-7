<?php if ($messages): ?>
  <div id="messages"><div class="section clearfix">
    <?php print $messages; ?>
  </div></div> <!-- /.section, /#messages -->
<?php endif; ?>

<?php
  require_once(drupal_get_path('module', 'cu_newsletter') . '/csstoinlinestyles/vendor/autoload.php');
  use \TijsVerkoyen\CssToInlineStyles\CssToInlineStyles;
  $cssToInlineStyles = new CssToInlineStyles();

  // Get design
  $newsletter_node = menu_get_object();
  $design = $newsletter_node->field_newsletter_type[LANGUAGE_NONE][0]['taxonomy_term']->field_newsletter_design[LANGUAGE_NONE][0]['value'];
  // Prepare CSS
  $css = array();
  // Load Framework CSS
  $css['framework'] = file_get_contents(drupal_get_path('theme', 'cuemail') . '/css/framework.css');
  // Load Global CSS
  $css['global'] = file_get_contents(drupal_get_path('theme', 'cuemail') . '/css/global.css');
  // Load Design CSS
  $css['design'] = file_get_contents(drupal_get_path('theme', 'cuemail') . '/css/' . $design . '.css');
  // Load Responsive CSS
  // Responsive is kept separate because it should not be inlined.
  $responsive = file_get_contents(drupal_get_path('theme', 'cuemail') . '/css/responsive.css');

  $styles = join(' ', $css);

  $html = '<style>' . $styles . '</style>' . '<style>' . $responsive . '</style>' . render($page['content']);
  //$html = str_replace("\xc2\xa0",' ',$html);
  $cssToInlineStyles->setHTML($html);
  $cssToInlineStyles->setCSS($styles);

?>
<style>
  <?php //print $css; ?>
</style>

<?php

  if (isset($_GET['debug'])) {
    $email = $html;
  }
  else {
    $email = $cssToInlineStyles->convert();
    $email = str_replace('src="https://', 'src="http://', $email);
    $email = str_replace('src="//', 'src="http://', $email);
    $email = str_replace('href="//', 'href="http://', $email);
  }

  print $email;





?>
<hr />
<textarea rows="50" style="width:100%;">
<?php
  //$html = render($page['content']);
  // fixing weird encoding of &nbsp that is inserted from wysiwyg
  if (isset($_GET['debug'])) {
    $compressed = $email;
  }
  else {
    $compressed = cuemail_html_compress($email);
  }

  print htmlentities($compressed, ENT_COMPAT | 'ENT_HTML401', 'UTF-8');
?>
</textarea>
