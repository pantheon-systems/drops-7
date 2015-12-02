<?php

/**
 * @file
 * Renders the permalinks to a node's files.
 *
 * Available variables:
 *  
 *  $items: A link for each of the two attachment links.
 *
 * The following variables are provided for contextual information.
 * 
 *   $node: Node object the link information is being rendered for.
 *   $user: The user accessing the node.
 *
 * @see template_preprocess_attachment_links()
 * @see theme_item_list()
 */
?>

<div id="attachment-links">
  <?php print theme('item_list', array('items' => $items)); ?>
</div>
