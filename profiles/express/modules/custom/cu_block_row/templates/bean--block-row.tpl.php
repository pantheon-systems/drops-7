<div class="block-row-wrapper element-max-width-padding">
  <div class="block-row-inner block-row-columns-<?php print $column_count; ?> block-row-distribution-<?php print $content['field_block_row_distribution'][0]['#markup']; ?> <?php print $match_height; ?> clearfix">
  <?php
    print render($content['blocks']);
  ?>
  </div>
</div>
