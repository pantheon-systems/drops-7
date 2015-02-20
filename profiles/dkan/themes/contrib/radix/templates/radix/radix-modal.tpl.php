<?php

/**
 * @file
 * Template file for Radix modal.
 */
?>
<!-- Button trigger modal -->
<?php print render($trigger_button); ?>

<!-- Modal -->
<div class="modal fade" id="<?php print $modal_id; ?>" tabindex="-1" role="dialog" aria-labelledby="<?php print $modal_id; ?>" aria-hidden="true">
  <div class="modal-dialog">
    <div class="modal-content">
      <?php if ($header): ?>
        <div class="modal-header">
          <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
          <h4 class="modal-title" id="myModalLabel">
            <?php print $header; ?>
          </h4>
        </div>
      <?php endif; ?>
      <?php if ($content): ?>
        <div class="modal-body">
          <?php print render($content); ?>
        </div>
      <?php endif; ?>
      <?php if (count($buttons)): ?>
        <div class="modal-footer">
          <?php foreach ($buttons as $button): ?>
            <?php print $button; ?>
          <?php endforeach; ?>
        </div>
      <?php endif; ?>
    </div>
  </div>
</div>
