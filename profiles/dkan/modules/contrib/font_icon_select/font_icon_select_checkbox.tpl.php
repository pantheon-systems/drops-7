<?php
/**
 * @file
 * Default admin select option template for font icon select field.
 */
?>

<div class="font_icon_selection_outer_wrapper">
  <?php print $element['field'];?>
  <label class="selectionWrapper option" for="<?php print $element['element_id'] . '-' . $element['key'];?>">
    <div class="selectionOuter">
      <div class="selectionInner<?php print $element['checked'] ? ' checked' : '';?>">
        <div class="selection"><?php print $element['value'];?></div>
      </div>
    </div>
    <div class="label"><?php print $element['key'];?></div>
  </label>
</div>
