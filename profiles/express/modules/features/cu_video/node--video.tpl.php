<?php if(!$page): ?>
  <?php
    $video_thumb_path = $node->field_video[LANGUAGE_NONE][0]['thumbnail_path'];
    $video_thumb_url = url(image_style_url('thumbnail', $video_thumb_path), array('https'=>FALSE, 'absolute'=>TRUE,));
  ?>
  <div class="video-teaser clearfix">
    <a href="<?php print $node_url; ?>"><img src="<?php print $video_thumb_url; ?>" alt="" class="video-thumb" /></a>
    <div class="video-text">
      <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
      <?php print render($content['body']); ?>
    </div>
  </div>

<?php else: ?>
  <?php print render($content['body']); ?>
  <?php print render($content['field_video']); ?>
<?php endif; ?>
