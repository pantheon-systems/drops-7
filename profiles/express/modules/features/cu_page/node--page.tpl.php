<?php if (!$page): ?>
  <div class="page-view-mode-teaser clearfix">
    <?php if(!empty($content['field_photo'][0])): ?>
      <?php print render($content['field_photo'][0]); ?>
    <?php endif; ?>
    <div class="page-view-mode-teaser-content">
      <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
      <div class="page-summary"><?php print render($content['body']); ?></div>
    </div>
  </div>

<?php else: ?>
  <?php if (!empty($content)): ?>
  <div<?php print $attributes; ?>>
    <?php print $user_picture; ?>
    <?php print render($title_prefix); ?>
    <?php if (!$page && $title): ?>
    <div>
      <h2<?php print $title_attributes; ?>><a href="<?php print $node_url ?>" title="<?php print $title ?>"><?php print $title ?></a></h2>
    </div>
    <?php endif; ?>
    <?php print render($title_suffix); ?>  
    <?php print render($content_sidebar_left); ?>
    <?php print render($content_sidebar_right); ?>
    <div<?php print $content_attributes; ?>>
      <?php
        // We hide the comments and links now so that we can render them later.
        hide($content['comments']);
        hide($content['links']);
        print render($content);
      ?>
    </div>
    
    <div class="clearfix">
      <?php if (!empty($content['links'])): ?>
        <div class="links node-links clearfix"><?php print render($content['links']); ?></div>
      <?php endif; ?>
  
      <?php print render($content['comments']); ?>
    </div>
  </div>
  <?php endif; ?>
<?php endif; ?>