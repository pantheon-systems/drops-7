<div class="block-row-wrapper">
  <div class="block-row-inner block-row-columns-<?php print $column_count; ?> block-row-distribution-<?php print $content['field_block_row_distribution'][0]['#markup']; ?> clearfix">
  <?php
    print render($content['blocks']);
  ?>
  </div>
</div>
