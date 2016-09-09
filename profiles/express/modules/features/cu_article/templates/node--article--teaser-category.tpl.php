<?php hide($content['field_tags']); ?>
<?php hide($content['article_tags']); ?>
<?php $tag = isset($heading_tag['#tag']) ? $heading_tag['#tag'] : 'h2'; ?>
<div class="article-view-mode-teaser node-view-mode-teaser clearfix">
  <?php if(!empty($content['field_article_thumbnail'])): ?>
    <?php print render($content['field_article_thumbnail']); ?>
  <?php endif; ?>
  <div class="article-view-mode-teaser-content node-view-mode-teaser-content">
    <<?php print $tag; ?><?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></<?php print $tag; ?>>
    <div class="article-teaser-meta">
      <?php if (isset($category_teaser_category_links)): ?>
        <?php print $category_teaser_category_links; ?>
      <?php endif; ?>
    </div>
    <div class="article-summary"><?php print render($content['body']); ?></div>
  </div>
</div>
