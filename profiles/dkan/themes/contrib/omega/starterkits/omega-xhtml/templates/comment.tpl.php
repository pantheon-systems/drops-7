<div<?php print $attributes; ?>>

  <?php print $unpublished; ?>

  <div>
    <?php print render($title_prefix); ?>
    <?php if ($title): ?>
      <h3<?php print $title_attributes; ?>><?php print $title ?></h3>
    <?php endif; ?>
    <?php print render($title_suffix); ?>
    <?php if ($new): ?>
      <em class="new"><?php print $new ?></em>
    <?php endif; ?>
  </div>

  <?php print $picture; ?>

  <div class="comment-submitted">
   <?php
      print t('Submitted by !username on !datetime',
      array('!username' => $author, '!datetime' => '<time datetime="' . $datetime . '">' . $created . '</time>'));
    ?>
  </div>

  <div<?php print $content_attributes; ?>>
    <?php
      hide($content['links']);
      print render($content);
    ?>
  </div>

  <?php if ($signature): ?>
    <div class="user-signature"><?php print $signature ?></div>
  <?php endif; ?>

  <?php if (!empty($content['links'])): ?>
    <div class="links comment-links clearfix"><?php print render($content['links']); ?></div>
  <?php endif; ?>

</div>
