<?php if (isset($content['featured_app'])) { print drupal_render($content['featured_app']); } ?>
<?php if (isset($content['apps'])) : ?>
<div id="apps-list" class="clearfix">
  <?php print drupal_render($content['apps']) ?>
</div>
<?php endif; ?>
