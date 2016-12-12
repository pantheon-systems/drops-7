<?php
  if (!empty($content['image'])) {
    $content['image'][0]['#image_style'] = 'email_teaser_thumbnail';
  }
?>
<div class="article-view-mode-email-teaser node-view-mode-email-teaser view-mode-custom-content clearfix <?php print $content['column_class']; ?>">
  <?php if(!empty($content['image'])): ?>
    <?php if(!empty($content['link'])): ?>
      <a href="<?php print $content['link']; ?>"><?php print render($content['image']); ?></a>
    <?php else: ?>
      <?php print render($content['image']); ?>
    <?php endif; ?>
  <?php endif; ?>
  <div class="article-view-mode-email-teaser-content node-view-mode-email-teaser-content view-mode-custom-content-content">
    <h3><?php print render($content['title']); ?></h3>
    <div class="article-summary"><?php print render($content['body']); ?></div>
  </div>
</div>
