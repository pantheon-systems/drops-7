<?php print $breadcrumb; ?>
<div id="admin-top" class="clearfix">
  <?php if (isset($express_help)): ?>
    <div class="express-help-links">
      <?php print $express_help; ?>
    </div>
  <?php endif; ?>
  <?php print render($title_prefix); ?>
  <?php if ($title): ?>
    <?php $icon_class = strip_tags(strtolower(str_replace(' ','-', $title))); ?>
    <h1 class="page-title icon-<?php print $icon_class; ?>"><?php print $title; ?></h1>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

</div>
<div id="primary-tabs">
  <?php print render($primary_local_tasks); ?>
</div>
<div id="page">
  <?php if ($secondary_local_tasks): ?>
    <div class="tabs-secondary clearfix"><?php print render($secondary_local_tasks); ?></div>
  <?php endif; ?>

  <div id="content" class="clearfix">
    <div class="element-invisible"><a id="main-content"></a></div>
    <?php if ($messages): ?>
      <div id="console" class="clearfix"><?php print $messages; ?></div>
    <?php endif; ?>
    <?php if ($page['help']): ?>
      <div id="help">
        <?php print render($page['help']); ?>
      </div>
    <?php endif; ?>
    <?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>
    <?php print render($page['content']); ?>
  </div>

  <div id="footer">
    <?php print $feed_icons; ?>
  </div>

</div>
