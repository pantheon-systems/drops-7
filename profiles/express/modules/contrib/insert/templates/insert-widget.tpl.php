<?php

/**
 * @file
 * Template file for the insert button.
 *
 * This button can have any appearance you like or even be a link, but must
 * include the class "insert-button", to which the insert JavaScript will be
 * attached.
 */
?>
<?php if (count($insert_styles) > 1): ?>
  <div class="insert form-item container-inline inline">
    <div class="insert-style-select">
      <label><?php print t('Style') ?>:</label>
      <select class="insert-style">
        <?php foreach ($insert_styles as $value => $style): ?>
          <option value="<?php print $value ?>"<?php print ($value == $default_style) ? 'selected="selected"' : '' ?>><?php print $style ?></option>
        <?php endforeach; ?>
      </select>
    </div>
<?php else: ?>
  <input type="hidden" class="insert-style" value="<?php print $default_style ?>" />
<?php endif; ?>

  <input type="submit" rel="<?php print $widget_type ?>" class="form-submit insert-button" onclick="return false;" value="<?php print t('Insert'); ?>" />

<?php if (count($insert_styles) > 1): ?>
  </div>
<?php endif; ?>