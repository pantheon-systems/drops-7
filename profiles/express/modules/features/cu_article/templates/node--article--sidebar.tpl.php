<div class="article-view-mode-sidebar node-view-mode-sidebar clearfix">
  <?php if(!empty($content['field_article_thumbnail'])): ?>
    <?php print render($content['field_article_thumbnail']); ?>
  <?php endif; ?>
  <div class="article-view-mode-sidebar-content node-view-mode-sidebar-content">
    <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
  </div>
</div>
