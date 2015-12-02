<?php

/**
 * @file
 * template to format ThemeKey Debug Messages
 */
?>
<table border="1" style="color:black;" bgcolor="white">
  <tr><th><?php print t('ThemeKey Debug Messages'); ?></th></tr>
  <?php foreach ($messages as $message) {?>
  <tr><td><?php print $message; ?></td></tr>
  <?php } ?>
</table>
