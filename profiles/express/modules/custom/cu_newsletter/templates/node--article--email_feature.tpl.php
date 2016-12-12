<?php hide($content['field_tags']); ?>
<?php hide($content['article_tags']); ?>

<?php
  if (!empty($content['image'])) {
    $content['field_article_thumbnail'][0]['#image_style'] = 'email_feature_thumbnail_large';
  }
  ?>
<div class="article-view-mode-email-feature node-view-mode-email-feature clearfix <?php print $elements['#column_class']; ?>">
  <?php if(!empty($content['field_article_thumbnail'])): ?>
    <?php print render($content['field_article_thumbnail']); ?>
  <?php endif; ?>
  <div class="article-view-mode-email-feature-content node-view-mode-email-feature-content">
    <h3><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
    <?php if (isset($category_teaser_category_links)): ?>
      <div class="article-teaser-meta">
        <?php print $category_teaser_category_links; ?>
      </div>
    <?php endif; ?>
    <div class="article-summary"><?php print render($content['body']); ?></div>
  </div>
</div>
