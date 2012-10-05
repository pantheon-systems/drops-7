<div<?php print $attributes; ?>>
  <div<?php print $content_attributes; ?>>
    <?php print $content; ?>
    <div id="date-widget">
      <div id="month"><?php print date('M'); ?>.</div>
      <div id="day"><?php print date('j'); ?></div>
      <div id="year"><?php print date('Y'); ?></div>
    </div>
  </div>
</div>