<?php
  global $base_root;
  global $base_path;
  $url = $base_root . $base_path . 'node/' . $node->nid . '/attachment';
  $url = preg_replace("/^https:/i", "http:", $url);
?>

<?php if(!$page): ?>
  <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
  <p><?php print render($content['body']); ?></p>
  <p>File Alias URL: <a href="<?php print $url; ?>"><?php print $url; ?></a></p>
<?php else: ?>

  <p><?php print render($content['body']); ?></p>
  <div class="file-alias-url">
    Access the top file listed below with the following url:<br />
    <a href="<?php print $url; ?>"><?php print $url; ?></a>
  </div>
  <div class="file-uploads">
    <strong>Attached Files:</strong><br />
    <?php print render($content['field_file_attachment']); ?>
  </div>
  <?php if (!empty($content['image_styles'])): ?>
    <div class="file-uploads">
      <strong>Image URLs:</strong><br />
      <?php 
        print render($content['image_styles']);
      ?>
    </div>
  <?php endif; ?>
<?php endif; ?>
