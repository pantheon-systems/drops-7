<div class="content-grid-item">
  <div class="content-grid-image">
    <?php if (!empty($content['image'])): ?>
      <div class="content-grid-image-wrapper">
        <?php print $content['image']; ?>
      </div>
    <?php else: ?>
      <div class="content-grid-image-wrapper content-grid-image-placeholder">
        <?php print $content['place_holder']; ?>
      </div>
    <?php endif; ?>
    <div class="content-grid-content <?php if (empty($content['image'])) { print 'content-grid-content-has-placeholder'; } ?>">
      <?php print $content['title']; ?>
      <div class="content-grid-text">
        <?php // print $content['text']; ?>
      </div>
    </div>
  </div>
</div>
