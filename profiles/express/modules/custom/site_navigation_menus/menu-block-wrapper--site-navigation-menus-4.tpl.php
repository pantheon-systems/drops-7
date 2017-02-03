<?php

/**
 * @file
 * Programmatically create VCR content. Oh Yeah!
 *
 * Will create pages, menus and menu items, beans and more.
 * More power!
 */

?>

<a href="#section-navigation" class="section-navigation-toggle"><strong><i class="fa fa-reorder"></i> In This Section</strong></a>
<div id="section-navigation" style="display:none;">
  <div class="<?php print $classes; ?>">
    <?php print render($content); ?>
  </div>
</div>
<script type="text/javascript">
  jQuery(document).ready(function () {
    jQuery("a.section-navigation-toggle").click(function () {
      jQuery("#section-navigation").fadeToggle();

      return false;
    });
  });
</script>
