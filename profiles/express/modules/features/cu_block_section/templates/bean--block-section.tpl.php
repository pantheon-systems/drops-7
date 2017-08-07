<div id="block-section-<?php print $bean->bid; ?>" class="<?php print $block_section_classes; ?> " <?php print $parallax_data; ?>>
  <div class="block-section-content-wrapper clearfix element-max-width-padding">
    <div class="clearfix block-section-content block-section-content-bg-<?php print $content['field_block_section_content_bg'][0]['#markup']; ?>">
      <?php print render($content['blocks']); ?>
    </div>
  </div>
</div>
<style>
  .block-bean-type-block-section {overflow:hidden;}
</style>
<?php if(isset($image_small) && isset($image_medium) && isset($image_large)): ?>
  <style>
    #block-section-<?php print $bean->bid; ?> {
      background-image:url(<?php print $image_small; ?>);
    }
    @media all and (min-width: 768px) and (max-width: 959px) {
      #block-section-<?php print $bean->bid; ?> {
        background-image:url(<?php print $image_medium; ?>);
      }
    }
    @media all and (min-width: 960px) {
      #block-section-<?php print $bean->bid; ?> {
        background-image:url(<?php print $image_large; ?>);
      }
    }
  </style>
<?php endif; ?>
