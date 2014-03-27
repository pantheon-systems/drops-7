<?php
/**
 * @file
 * Colorizer options.
 */

$info = array(
  // Available colors and color labels used in theme.
  'fields' => array(
    'top' => t('Toolbar top'),
    'bottom' => t('Toolbar bottom'),
    'toolbarbtn' => t('Toolbar button background'),
    'toolbarborder' => t('Toolbar button border'),
    'bg' => t('Main background'),
    'sidebar' => t('Footer text color'),
    'sidebarborders' => t('Footer link color'),
    'footer' => t('Footer background'),
    'titleslogan' => t('Title and slogan'),
    'text' => t('Text color'),
    'link' => t('Link color'),
    'linkactive' => t('Link (active)'),
    'linkhover' => t('Link (hover)'),
  ),
  // Pre-defined color schemes.
  'schemes' => array(
    'default' => array(
      'title' => t('DKAN (default)'),
      'base' => '#0062a0',
      'colors' => array(
        'top' => '#0062a0',
        'bottom' => '#d3ae7e',
        'toolbarbtn' => '#ffffff',
        'toolbarborder' => '#cccccc',
        'bg' => '#ffffff',
        'sidebar' => '#3b3b3b',
        'sidebarborders' => '#0062a0',
        'footer' => '#ffffff',
        'titleslogan' => '#fffeff',
        'text' => '#3b3b3b',
        'link' => '#0071B3',
        'linkactive' => '#23aeff',
        'linkhover' => '#018fe2',
      ),
    ),
    'bluelagoon' => array(
      'title' => t('Blue Lagoon'),
      'base' => '#48a9e4',
      'colors' => array(
        'top' => '#0779bf',
        'bottom' => '#23aeff',
        'toolbarbtn' => '#c6e5f8',
        'toolbarborder' => '#c6e5f8',
        'bg' => '#ffffff',
        'sidebar' => '#f6f6f2',
        'sidebarborders' => '#0071B3',
        'footer' => '#292929',
        'titleslogan' => '#fffeff',
        'text' => '#3b3b3b',
        'link' => '#0071B3',
      ),
    ),
    'firehouse' => array(
      'title' => t('Firehouse'),
      'base' => '#d6121f',
      'colors' => array(
        'top' => '#cd2d2d',
        'bottom' => '#951f1f',
        'toolbarbtn' => '#ffffff',
        'toolbarborder' => '#cccccc',
        'bg' => '#ffffff',
        'sidebar' => '#eeeeee',
        'sidebarborders' => '#d6121f',
        'footer' => '#1f1d1c',
        'titleslogan' => '#fffeff',
        'text' => '#3b3b3b',
        'link' => '#d6121f',
      ),
    ),
    'ice' => array(
      'title' => t('Ice'),
      'base' => '#019dbf',
      'colors' => array(
        'top' => '#d0d0d0',
        'bottom' => '#c2c4c5',
        'toolbarbtn' => '#ffffff',
        'toolbarborder' => '#dedede',
        'bg' => '#ffffff',
        'sidebar' => '#cccccc',
        'sidebarborders' => '#ffffff',
        'footer' => '#24272c',
        'titleslogan' => '#000000',
        'text' => '#4a4a4a',
        'link' => '#019dbf',
      ),
    ),
    'plum' => array(
      'title' => t('Plum'),
      'base' => '#9d408d',
      'colors' => array(
        'top' => '#4c1c58',
        'bottom' => '#593662',
        'toolbarbtn' => '#ffffff',
        'toolbarborder' => '#401050',
        'bg' => '#fffdf7',
        'sidebar' => '#edede7',
        'sidebarborders' => '#9d408d',
        'footer' => '#2c2c28',
        'titleslogan' => '#ffffff',
        'text' => '#301313',
        'link' => '#9d408d',
      ),
    ),
    'forest' => array(
      'title' => t('Forest'),
      'base' => '#0a9900',
      'colors' => array(
        'top' => '#1d6919',
        'bottom' => '#2db823',
        'toolbarbtn' => '#ffffff',
        'toolbarborder' => '#cccccc',
        'bg' => '#ffffff',
        'sidebar' => '#f1f4f0',
        'sidebarborders' => '#0a9900',
        'footer' => '#1f1d1c',
        'titleslogan' => '#fffeff',
        'text' => '#3b3b3b',
        'link' => '#0a9900',
      ),
    ),
    'slate' => array(
      'title' => t('Slate'),
      'base' => '#0073b6',
      'colors' => array(
        'top' => '#4a4a4a',
        'bottom' => '#4e4e4e',
        'toolbarbtn' => '#cccccc',
        'toolbarborder' => '#464646',
        'bg' => '#ffffff',
        'sidebar' => '#cccccc',
        'sidebarborders' => '#ffffff',
        'footer' => '#161617',
        'titleslogan' => '#ffffff',
        'text' => '#3b3b3b',
        'link' => '#0073b6',
      ),
    ),
  ),

  // Gradient definitions.
  'gradients' => array(
    array(
      // (x, y, width, height).
      'dimension' => array(0, 0, 0, 0),
      // Direction of gradient ('vertical' or 'horizontal').
      'direction' => 'vertical',
      // Keys of colors to use for the gradient.
      'colors' => array('top', 'bottom'),
    ),
  ),

);
