<?php
/**
 * @file custom Search API ranges Min/Max UI slider widget
 */
?>
<?php print drupal_render($form['text-range']); ?>
<div class="yui3-g">
  <div class="yui3-u range-box range-box-left">
    <?php print drupal_render($form['range-from']); ?>
  </div>
  <div class="yui3-u range-slider-box">
    <?php print drupal_render($form['range-slider']); ?>
  </div>
  <div class="yui3-u range-box range-box-right">
    <?php print drupal_render($form['range-to']); ?>
  </div>
</div>
<?php print drupal_render($form['submit']); ?>
<?php
  // Render required hidden fields.
  print drupal_render_children($form);
?>
