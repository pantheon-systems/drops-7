<?php
/**
 * @file field--fences-div-div-div.tpl.php
 * This file is identical to Drupal's default field markup, which uses
 * multiple nested <div> elements. Note: This template is never used; instead
 * Drupal core's theme_field() is used.
 *
 * @see http://developers.whatwg.org/grouping-content.html#the-div-element
 */
?>
<div class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <?php if (!$label_hidden): ?>
    <div class="field-label"<?php print $title_attributes; ?>><?php print $label ?>:&nbsp;</div>
  <?php endif; ?>
  <div class="field-items"<?php print $content_attributes; ?>>
    <?php foreach ($items as $delta => $item): ?>
      <div class="field-item <?php print $delta % 2 ? 'odd' : 'even'; ?>"<?php print $item_attributes[$delta]; ?>><?php print render($item); ?></div>
    <?php endforeach; ?>
  </div>
</div>
