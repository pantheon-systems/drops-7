<article class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print $unpublished; ?>

  <?php print render($title_prefix); ?>

  <header<?php print $header_attributes; ?>>
  
    <?php print $picture; ?>

    <?php if ($title): ?>
      <h3<?php print $title_attributes; ?>><?php print $title ?></h3>
    <?php endif; ?>

    <?php if ($new): ?>
      <em class="new"><?php print $new ?></em>
    <?php endif; ?>
    
    <p class="submitted"><?php print $submitted; ?></p>

  </header>

  <?php print render($title_suffix); ?>

  <div<?php print $content_attributes; ?>>
    <?php
      hide($content['links']);
      print render($content);
    ?>

    <?php if ($signature): ?>
      <aside class="user-signature clearfix">
        <?php print $signature;  ?>
      </aside>
    <?php endif; ?>

  </div>

  <?php if ($links = render($content['links'])): ?>
    <nav<?php print $links_attributes; ?>><?php print $links; ?></nav>
  <?php endif; ?>

</article>
