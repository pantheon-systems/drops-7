<div class="issue-view-mode-embed node-view-mode-embed clearfix">
  <?php if(!empty($content['field_issue_image'][0])): ?>
    <?php print render($content['field_issue_image'][0]); ?>
  <?php endif; ?>
  <div class="issue-view-mode-embed-content node-view-mode-embed-content">
    <h3<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
    <div class="issue-summary"><?php print render($content['body']); ?></div>
  </div>
</div>
