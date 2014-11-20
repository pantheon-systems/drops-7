<?php
/* variables
$form - The full subscription form
$form['display'] - The subscription entity view
$form['subscribe'] - The checkbox
$form['groups'] - The groups checkboxes
*/
?>
<?php print render($form['display']); ?>
<?php print render($form['subscribe']); ?>
<?php print render($form['groups']); ?>
<?php print drupal_render_children($form); ?>