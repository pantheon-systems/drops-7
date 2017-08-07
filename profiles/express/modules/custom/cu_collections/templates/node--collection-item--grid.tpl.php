<div class="collection-item collection-item-visible collection-view-mode-grid <?php print $category_classes; ?>">
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
