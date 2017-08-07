<div class="page-view-mode-embed node-view-mode-embed clearfix">
  <?php if(!empty($content['field_photo'][0])): ?>
    <?php print render($content['field_photo'][0]); ?>
  <?php endif; ?>
  <div class="page-view-mode-embed-content node-view-mode-embed-content">
    <h3<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
    <div class="page-summary"><?php print render($content['body']); ?></div>
  </div>
</div>
