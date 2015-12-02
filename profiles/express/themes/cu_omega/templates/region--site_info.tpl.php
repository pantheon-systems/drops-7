<?php global $base_url; ?>
<div class="container-12"><div class="grid-12 site-info-row">
  
<?php if (!empty($content)) { ?>
  <div class="grid-8 alpha">
    <?php print render($content); ?>
  </div>
  <div class="grid-4 omega">
<?php } else { ?>
  <div class="grid-12 centered alpha omega">
<?php } ?>
    <div class="block cu-boulder-block">
      <div class="block-inner">
        <p><a href="//www.colorado.edu"><img src="<?php print $base_url . '/' . drupal_get_path('theme', 'cu_omega'); ?>/images/beboulder/be-boulder-<?php print $beboulder['color']; ?>.png" alt="University of Colorado Boulder" class="beboulder"/></a></p>
                <p><strong><a href="http://www.colorado.edu">University of Colorado Boulder</a></strong><br />&copy; Regents of the University of Colorado<br />
        <span class="required-links"><a href="http://www.colorado.edu/policies/privacy-statement">Privacy</a> &bull; <a href="http://www.colorado.edu/about/legal-trademarks">Legal &amp; Trademarks</a> &bull; <a href="http://www.colorado.edu/accessibility">Accessibility</a></span></p>
      </div>
    </div>
  </div>
</div></div>