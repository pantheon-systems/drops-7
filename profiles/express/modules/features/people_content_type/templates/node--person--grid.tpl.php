<a name="<?php print urlencode(strtolower($title)); ?>" id="<?php print urlencode(strtolower($title)); ?>"></a><div class="person-view-mode-grid grid-person clearfix col-lg-4 col-md-4 col-sm-6 col-xs-12">
  <?php if(!empty($content['field_person_photo'])): ?>
    <?php print render($content['field_person_photo']); ?>
  <?php else: ?>
    <?php global $base_url; ?>
    <a href="<?php print $node_url; ?>"><img src="<?php print $base_url; ?>/<?php print drupal_get_path('module', 'people_content_type'); ?>/images/avatar320.jpg" class="image-large_square_thumbnail" alt="<?php print $title; ?>" /></a>
  <?php endif; ?>
  <div class="person-grid-content-wrapper">
    <strong><a href="<?php print $node_url; ?>"><?php print $title; ?></a></strong>
    <div class="person-view-mode-grid-content">
      <div class="person-job-titles-grid"><?php print $content['job_titles']; ?></div>
      <div class="person-departments-grid"><?php print $content['departments']; ?></div>
    </div>
  </div>
</div>
