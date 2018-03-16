<?php

/**
 * @file
 * Template file for Radix Tabs.
 */
?>
<?php if (count($tabs)): ?>
  <!-- Nav tabs -->
  <ul class="nav nav-<?php print $type; ?>">
    <?php foreach ($tabs as $tab): ?>
      <li><?php print render($tab['tab']); ?></li>
    <?php endforeach; ?>
  </ul>

  <!-- Tab panes -->
  <div class="tab-content">
    <?php foreach ($tabs as $tab): ?>
      <?php if (!empty($tab['content'])): ?>
        <div class="tab-pane" id="<?php print $tab['id']; ?>"><?php print render($tab['content']); ?></div>
      <?php endif; ?>
    <?php endforeach; ?>
  </div>
<?php endif; ?>
