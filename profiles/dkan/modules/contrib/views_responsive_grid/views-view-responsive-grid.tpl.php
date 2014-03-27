<?php
/**
 * @file
 * Default simple view template to display a rows in a responsive grid.
 *
 * - $rows contains a nested array of rows. Each row contains an array of
 *   columns.
 * - $columns contains a nested array of columns. Each column contains an
 *   array of columns.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)) : ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
  <div class="<?php print $classes; ?>">
<?php if ($options['alignment'] == 'vertical') : ?>
<?php foreach ($columns as $column_id => $column) : ?>
  <div class="<?php print trim($column_classes[$column_id]); ?>">
  <?php foreach ($column as $item_id => $item) : ?>
    <div class="<?php print trim($item['classes']); ?>">
      <?php print $item['content']; ?>
    </div>
  <?php endforeach; ?>
  </div>
<?php endforeach; ?>
<?php else : ?>
  <?php foreach ($rows as $row_id => $row) : ?>
    <div class="<?php print trim($row_classes[$row_id]); ?>">
  <?php foreach ($row as $item_id => $item) : ?>
    <div class="<?php print trim($item['classes']); ?>">
      <?php print $item['content']; ?>
    </div>
  <?php endforeach; ?>
  </div>
  <?php endforeach; ?>
<?php endif; ?>
  </div>
