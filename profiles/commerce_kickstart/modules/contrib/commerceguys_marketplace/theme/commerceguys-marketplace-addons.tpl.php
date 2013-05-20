<h3><?php print t('Featured add-ons'); ?></h3>

<div class="marketplace-addons marketplace-list clearfix">
  <?php foreach ($addons as $addon_row): ?>
    <div class="row">
    <?php foreach ($addon_row as $addon): ?>
      <div class="addon"><?php print $addon; ?></div>
    <?php endforeach; ?>
    </div>
  <?php endforeach; ?>
</div>
