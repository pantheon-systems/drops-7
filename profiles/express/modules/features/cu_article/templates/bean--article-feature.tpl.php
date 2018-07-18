<?php
  if ($content['#display'] == 'article-feature-inline-3') {
    $grid_classes['top'] = 'col-lg-8 col-md-8 col-sm-8 col-xs-12';
    $grid_classes['remaining'] = 'col-lg-4 col-md-4 col-sm-4 col-xs-12';
  }
  elseif ($content['#display'] == 'article-feature-inline-2') {
    $grid_classes['top'] = 'col-lg-6 col-md-6 col-sm-6 col-xs-12';
    $grid_classes['remaining'] = 'col-lg-6 col-md-6 col-sm-6 col-xs-12';
  }
  elseif ($content['#display'] == 'article-feature-stacked') {
    $grid_classes['top'] = 'col-lg-12 col-md-12 col-sm-12 col-xs-12';
    $grid_classes['remaining'] = 'col-lg-12 col-md-12 col-sm-12 col-xs-12';
  }
  ?>
<div class="article-feature-block <?php print $content['#display']; ?>">

  <div class="row clearfix">
    <div class="article-feature-block-top <?php print $grid_classes['top']; ?>">
      <?php print render($content['top_article']); ?>
    </div>
    <div class="article-feature-block-remaining <?php print $grid_classes['remaining']; ?>">
      <?php print render($content['articles']); ?>
    </div>
  </div>
  <?php if (!empty($content['more'])): ?>
    <div class="article-feature-block-more-link">
      <?php print render($content['more']); ?>
    </div>
  <?php endif; ?>
</div>
