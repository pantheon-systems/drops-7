<?php hide($content['field_tags']); ?>
<?php hide($content['article_tags']); ?>
<?php $tag = isset($heading_tag['#tag']) ? $heading_tag['#tag'] : 'h2' ?>

<?php if($view_mode == 'teaser'): ?>
  <div class="article-view-mode-teaser clearfix">
    <?php if(!empty($content['field_article_thumbnail'])): ?>
      <?php print render($content['field_article_thumbnail']); ?>
    <?php endif; ?>
    <div class="article-view-mode-teaser-content">
      <<?php print $tag; ?><?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></<?php print $tag; ?>>
      <p class="date"><?php print $ap_date_cu_medium_date; ?></p>
      <div class="article-summary"><?php print render($content['body']); ?></div>
    </div>
  </div>
<?php elseif($view_mode == 'sidebar'): ?>
  <div class="article-view-mode-sidebar clearfix">
    <?php if(!empty($content['field_article_thumbnail'])): ?>
      <?php print render($content['field_article_thumbnail']); ?>
    <?php endif; ?>
    <div class="article-view-mode-sidebar-content">
      <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
    </div>
  </div>
<?php elseif($view_mode == 'title'): ?>
  <div class="article-view-mode-title clearfix">
    <div class="article-view-mode-titles-content">
      <a href="<?php print $node_url; ?>"><?php print $title; ?></a>
    </div>
  </div>
<?php elseif($view_mode == 'grid'): ?>
  <?php print render($content['field_article_thumbnail']); ?>
  <h3><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
  <div class="article-grid-summary"><?php print render($content['body']); ?></div>
<?php else: ?>
  <p class="date"><?php print $ap_date_cu_medium_date; ?></p>
  <?php if (!empty($content['body'])): ?>
    <?php $bodyimg = strpos($content['body'][0]['#markup'], '<img'); ?>
    <?php if(!empty($content['field_image']) && ($bodyimg === FALSE)): ?>

      <div class="article-image">
        <?php print render($content['field_image']); ?>
      </div>
    <?php else: ?>
      <?php hide($content['field_image']); ?>
    <?php endif; ?>
  <?php endif; ?>
  <?php print render($content); ?>
  <?php if (!empty($content['field_tags']['#items'])): ?>
    <p class="tags"><i class="fa fa-tags"></i> Tags: <?php print render($content['article_tags']); ?></p>
  <?php endif; ?>
<?php endif; ?>
