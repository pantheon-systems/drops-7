<div class="page-view-mode-teaser node-view-mode-teaser clearfix">
  <?php if(!empty($content['field_photo'][0])): ?>
    <?php print render($content['field_photo'][0]); ?>
  <?php endif; ?>
  <div class="page-view-mode-teaser-content node-view-mode-teaser-content">
    <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
    <div class="page-summary"><?php print render($content['body']); ?></div>
  </div>
</div>
