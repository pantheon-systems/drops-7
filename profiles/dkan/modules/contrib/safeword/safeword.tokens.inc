<?php

/**
 * Implements hook_token_info().
 */
function safeword_token_info() {
  // Text module provides three different text field types.
  $info['types']['safeword-field'] = array(
    'name' => t('Safeword fields'),
    'description' => t('Tokens related to Safeword field instances.'),
    'needs-data' => 'safeword-field',
    'field' => TRUE,
    'module' => 'safeword',
  );
  $info['tokens']['safeword-field']['human'] = array(
    'name' => t('Human text'),
    'description' => t('The Safeword field human-readable text.'),
  );
  $info['tokens']['safeword-field']['machine'] = array(
    'name' => t('Machine text'),
    'description' => t('The Safeword field machine-safe text.'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function safeword_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  $sanitize = !empty($options['sanitize']);
  $language_code = isset($options['language']) ? $options['language']->language : NULL;

  if ($type == 'safeword-field') {
    $item = $data['item'];

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'human':
        case 'machine':
          $replacements[$original] = $sanitize ? check_plain($item[$name]) : $item[$name];
          break;
      }
    }
  }

  return $replacements;
}