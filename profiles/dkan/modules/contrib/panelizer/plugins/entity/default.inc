<?php
/**
 * @file
 * Definition of the default plugin.
 *
 * The default plugin contains the base class in which the majority of the
 * functionality is stored. It isn't used like a normal plugin, so we
 * have a couple of special directives.
 */

$plugin = array(
  'handler' => 'PanelizerEntityDefault',
  'no ui' => TRUE,
  'hooks' => array(),
);
