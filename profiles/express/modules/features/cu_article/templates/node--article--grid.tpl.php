<?php
  if (!empty($content['field_article_thumbnail'])) {
    print render($content['field_article_thumbnail']);
  }
?>
<h3><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
<?php if (isset($publish_date['#publish_date'])): ?>
  <p class="related-date">
    <?php print $publish_date['#publish_date']; ?>
  </p>
<?php endif; ?>
<div class="article-grid-summary">
  <?php print render($content['body']); ?>
  <?php if (isset($more_link)): ?>
    <p><?php print $more_link; ?></p>
  <?php endif; ?>
</div>
