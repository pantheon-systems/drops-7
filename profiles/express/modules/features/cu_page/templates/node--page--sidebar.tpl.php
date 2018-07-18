<div class="page-view-mode-sidebar clearfix node-view-mode-sidebar">
  <?php if(!empty($content['field_photo'][0])): ?>
    <?php print render($content['field_photo'][0]); ?>
  <?php endif; ?>
  <div class="page-view-mode-sidebar-content node-view-mode-sidebar-content">
    <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
  </div>
</div>
