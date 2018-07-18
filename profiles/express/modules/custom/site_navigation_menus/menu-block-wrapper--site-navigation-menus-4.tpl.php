<?php

/**
 * @file
 * Programmatically create VCR content. Oh Yeah!
 *
 * Will create pages, menus and menu items, beans and more.
 * More power!
 */

?>
<button aria-haspopup="true" aria-expanded="false" aria-controls="section-navigation" aria-label="In this section" class="section-navigation-toggle"><strong><i class="fa fa-fw fa-reorder"></i> In This Section</strong></button>

<div id="section-navigation" style="display:none;">
  <div class="<?php print $classes; ?>">
    <?php print render($content); ?>
  </div>
</div>
