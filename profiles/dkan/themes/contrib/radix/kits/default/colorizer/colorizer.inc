<?php

$info = array(
  // Available colors and color labels used in theme.
  'fields' => array(
    'primary' => t('Primary'),
    'navbar' => t('Navbar'),
    'link' => t('Link'),
    'linkhover' => t('Link Hover'),
  ),
  // Pre-defined color schemes.
  'schemes' => array(
    'default' => array(
      'title' => t('Default'),
      'colors' => array(
        'primary' => '#1abc9c',
        'navbar' => '#1abc9c',
        'link' => '#1abc9c',
        'linkhover' => '#7b7b7b',
      ),
    ),
    'asphalt' => array(
      'title' => t('Asphalt'),
      'colors' => array(
        'primary' => '#34495e',
        'navbar' => '#34495e',
        'link' => '#34495e',
        'linkhover' => '#2c3e50',
      ),
    ),
    'blue' => array(
      'title' => t('Blue'),
      'colors' => array(
        'primary' => '#3f9edd',
        'navbar' => '#3f9edd',
        'link' => '#3f9edd',
        'linkhover' => '#2980b9',
      ),
    ),
    'carrot' => array(
      'title' => t('Carrot'),
      'colors' => array(
        'primary' => '#e67e22',
        'navbar' => '#e67e22',
        'link' => '#e67e22',
        'linkhover' => '#d35400',
      ),
    ),
    'red' => array(
      'title' => t('Red'),
      'colors' => array(
        'primary' => '#c0392b',
        'navbar' => '#c0392b',
        'link' => '#c0392b',
        'linkhover' => '#e74c3c',
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
