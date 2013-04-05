<?php

$plugin = array(
  'title' => t('Test'),
  'description' => t('Notifier used for testing.'),
  'class' => 'MessageNotifierTest',
  // A notifier must define its own view modes.
  // Those view modes are later going to be rendered and sent.
  'view_modes' => array(
    'foo' => array('label' => t('Foo')),
    'bar' => array('label' => t('Bar')),
  ),
);
