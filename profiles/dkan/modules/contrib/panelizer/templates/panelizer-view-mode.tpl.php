<?php
/**
 * Default template for rendering a Panelizer-managed view mode.
 *
 * Available variables:
 * - $classes_array - An array of classes determined in
 *   template_preprocess_views_view().
 * - $title - The label/title for this entity.
 * - $title_element - HTML tag used by the title, defaults to 'h2'.
 * - $content - Rendered entity output for this view mode.
 * - $entity_url - The full URL for this entity.
 *
 * @see preprocess_panelizer_view_mode()
 */
?>

<div class="<?php print $classes; ?>"<?php print $attributes; ?>>
  <?php print render($title_prefix); ?>
  <?php if (!empty($title)): ?>
    <<?php print $title_element;?> <?php print $title_attributes; ?>>
      <?php if (!empty($entity_url)): ?>
        <a href="<?php print $entity_url; ?>"><?php print $title; ?></a>
      <?php else: ?>
        <?php print $title; ?>
      <?php endif; ?>
    </<?php print $title_element;?>>
  <?php endif; ?>
  <?php print render($title_suffix); ?>
  <?php print $content; ?>
</div>
