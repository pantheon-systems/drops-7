<?php hide($content['field_tags']); ?>
<?php hide($content['article_tags']); ?>
<?php hide($content['field_article_categories']); ?>
<?php hide($content['article_meta']); ?>

<?php $tag = isset($heading_tag['#tag']) ? $heading_tag['#tag'] : 'h2'; ?>
<?php if (!empty($author_meta)): ?>
  <div class="author-meta">
    <?php print join(' <span class="author-meta-separator">&bull;</span> ', $author_meta); ?>
  </div>
<?php endif; ?>
<?php if (!empty($content['body'])): ?>
  <?php $bodyimg = strpos($content['body'][0]['#markup'], '<img'); ?>
  <?php if (!empty($content['field_image']) && ($bodyimg === FALSE)): ?>

    <div class="article-image">
      <?php print render($content['field_image']); ?>
    </div>
  <?php else: ?>
    <?php hide($content['field_image']); ?>
  <?php endif; ?>
<?php endif; ?>
<?php print render($content); ?>
<?php print render($content_bottom); ?>
<?php
// Only display article meta elements if there is actually content
$article_meta = element_children($content['article_meta']);
if (!empty($article_meta)) {
  print render($content['article_meta']);
}
?>
