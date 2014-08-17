<article class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>
  <div class="comment-author">
      <?php print $picture; ?>
      <?php print $author ?>
      <?php if ($signature): ?><?php print $signature; ?><?php endif; ?>
  </div>
  <div class="comment-content">
  <header class="clearfix">

    <?php print render($title_prefix); ?>
    <?php if ($title): ?>
      <h3<?php print $title_attributes; ?>>
        <?php print $title; ?>
        <?php if ($new): ?>
          <mark class="new label label-default"><?php print $new; ?></mark>
        <?php endif; ?>
      </h3>
    <?php elseif ($new): ?>
      <mark class="new label-default"><?php print $new; ?></mark>
    <?php endif; ?>
    <?php print render($title_suffix); ?>
  </header>
  <div class="clearfix">
    <?php print render($content['field_rating']); ?>
    <div class="submitted">
      <?php print '-  '.format_date($comment->created, 'custom', 'M d, Y'); ?>
      <?php // print $permalink; ?>
    </div>
  </div>
  <?php
    // We hide the comments and links now so that we can render them later.
    hide($content['links']);
    print render($content);
  ?>

  
    <footer class="clearfix">
      <?php print render($content['links']) ?>
    </footer>
  </div>
</article>