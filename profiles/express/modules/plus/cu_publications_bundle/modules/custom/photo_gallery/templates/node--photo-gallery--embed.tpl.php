<div class="gallery-view-mode-embed node-view-mode-embed clearfix">
  <div class="gallery-view-mode-embed-content node-view-mode-embed-content">
    <h3<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
    <div class="node-view-mode-embed-summary gallery-view-mode-embed-summary"><?php print render($content['body']); ?></div>
    <?php print render($content); ?>
  </div>
</div>
