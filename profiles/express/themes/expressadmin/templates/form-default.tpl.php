<div class='form form-layout-default clearfix'>
  
  <?php print drupal_render_children($form); ?>
  <?php if (isset($sidebar)) { print drupal_render($sidebar); } ?>
  <?php print drupal_render($actions); ?>
  
  <?php if (!empty($footer)): ?>
    <div class='column-footer'><div class='column-wrapper clearfix'><?php print drupal_render($footer); ?></div></div>
  <?php endif; ?>
</div>