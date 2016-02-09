<div class="person-view-mode-sidebar clearfix node-view-mode-sidebar">
  <?php if(!empty($content['field_person_photo'][0])): ?>
    <?php print render($content['field_person_photo'][0]); ?>
  <?php endif; ?>
  <div class="person-view-mode-sidebar-content node-view-mode-sidebar-content">
    <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
  </div>
</div>
