<?php
/**
 * @file
 * Template to control the add content modal.
 */
?>
<div class="panels-add-content-modal row">
  <div class="panels-section-column panels-section-column-categories col-md-2">
    <div class="inside">
      <div class="panels-categories-box">
        <h5><?php print t('Add existing content'); ?></h5>
        <ul class="nav nav-pills nav-stacked">
          <?php foreach ($categories_array as $category): ?>
            <li><?php print $category; ?></li>
          <?php endforeach; ?>
        </ul>
      </div>
      <div class="panels-root-content-box">
        <h5><?php print t('Add new content'); ?></h5>
        <ul class="nav nav-pills nav-stacked">
          <?php foreach ($root_content_array as $root_content): ?>
            <li><?php print $root_content; ?></li>
          <?php endforeach; ?>
        </ul>
      </div>
    </div>
  </div>

  <?php print $messages; ?>

  <?php if (!empty($header)): ?>
    <div class="panels-categories-description col-md-10">
      <div class="inner">
        <?php print $header; ?>
      </div>
    </div>
  <?php endif; ?>

  <?php if (!empty($columns)): ?>
  <div class="panels-section-columns col-md-10">
    <div class="row">
      <?php foreach ($columns as $column_id => $column): ?>
        <div class="panels-section-column col-md-6 col-sm-12 col-xs-12">
          <div class="inside">
            <?php print $column; ?>
          </div>
        </div>
      <?php endforeach; ?>
    </div>
  </div>
  <?php endif; ?>
</div>
