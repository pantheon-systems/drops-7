=====
Block Title Link

-----
Block Title Link is a simple module that creates a link field in the Block Admin page. It works by creating new template variables in the $block object. It then uses hook_preprocess_block to wrap a link around the block->subject variable. There is a also a configuration parameter on a per block basis that disables the link from rendering in the title. This is useful for using the link elsewhere on the block template. This module provides the following variables to the $block object:

$block->title_link: The path stored with each block.
$block->title_link_title: The title attribute of the link. By default this is rendered as a "<a> title" attribute.

