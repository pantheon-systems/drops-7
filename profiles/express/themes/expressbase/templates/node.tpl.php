<?php if (!empty($content)): ?>
<div<?php print $attributes; ?>>
  <?php print render($content_sidebar_left); ?>
  <?php print render($content_sidebar_right); ?>
  <div<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links now so that we can render them later.
      hide($content['comments']);
      hide($content['links']);
      print render($content);
    ?>
  </div>
</div>
<?php endif; ?>
