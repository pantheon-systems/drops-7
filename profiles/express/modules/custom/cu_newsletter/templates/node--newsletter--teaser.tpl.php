<?php
  $title_class = (empty($content['body']) && empty($content['field_newsletter_intro_image'])) ? 'node-title-only' : '';
  $heading_tag['#tag'] = !empty($heading_tag['#tag']) ? $heading_tag['#tag'] : 'h2';
?>
<div class="newsletter-view-mode-teaser node-view-mode-teaser clearfix">
  <<?php print $heading_tag['#tag']; ?> class="<?php print $title_class; ?>"><a href="<?php print $node_url; ?>"><?php print $title; ?></a></<?php print $heading_tag['#tag']; ?>>
  <?php
  $content['field_newsletter_intro_image'][0]['#image_style'] = 'email_large';
  print render($content['field_newsletter_intro_image']);
  ?>

  <?php print render($content['body']); ?>
</div>
