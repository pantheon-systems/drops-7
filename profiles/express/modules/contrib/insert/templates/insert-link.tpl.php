<?php

/**
 * @file
 * Template file for generic links inserted via the Insert module.
 *
 * Available variables:
 * - $item: The complete item being inserted.
 * - $url: The URL to the item.
 * - $class: A set of classes assigned to this item (if any).
 * - $name: The file name of the item being inserted.
 *
 * Modules may provide placeholders that will be replaced by user-entered values
 * when the item is inserted into a textarea. Generic links only support one
 * placeholder.
 *
 * Available placeholders:
 * - __description__: A description of the item.
 * - __filename__: The file name.
 * - __description_or_filename__: A description of the item if available,
 *   otherwise use the file name.
 */
?>
<a href="<?php print $url ?>"<?php print $class ? ' class="' . $class . '"' : '' ?> title="__description__">__description_or_filename__</a>