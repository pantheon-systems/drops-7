<?php $tag = isset($heading_tag['#tag']) ? $heading_tag['#tag'] : 'h3'; ?>
<div class="article-view-mode-embed node-view-mode-embed clearfix">
  <?php if(!empty($content['field_article_thumbnail'])): ?>
    <?php print render($content['field_article_thumbnail']); ?>
  <?php endif; ?>
  <div class="article-view-mode-embed-content node-view-mode-embed-content">
    <<?php print $tag; ?><?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></<?php print $tag; ?>>
    <p class="date"><?php print $ap_date_cu_medium_date; ?></p>
    <div class="article-summary">
      <?php print render($content['body']); ?>
      <p><?php print $more_link; ?></p>
    </div>
  </div>
</div>
