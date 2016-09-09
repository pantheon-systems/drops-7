<div class="issue-view-mode-sidebar clearfix node-view-mode-sidebar">
  <?php if(!empty($content['field_issue_image'][0])): ?>
    <?php print render($content['field_issue_image'][0]); ?>
  <?php endif; ?>
  <div class="issue-view-mode-sidebar-content node-view-mode-sidebar-content">
    <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
  </div>
</div>
