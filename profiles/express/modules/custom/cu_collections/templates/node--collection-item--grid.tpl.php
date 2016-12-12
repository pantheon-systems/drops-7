<div class="col-lg-4 col-md-4 col-sm-6 col-xs-12">
<div class="collection-view-mode-grid">
  <?php
    if (!empty($content['field_collection_thumbnail'])) {
      print render($content['field_collection_thumbnail']);
    }
  ?>

  <div class="collection-view-mode-grid-content node-view-mode-grid-content">
    <h4><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h4>
    <div class="collection-summary">
      <?php

        if (!empty($content['field_collection_preview'])) {
          print render($content['field_collection_preview']);
        }
        else {
          print render($content['field_collection_body']);
        }
      ?>
    </div>
  </div>
</div>
</div>
