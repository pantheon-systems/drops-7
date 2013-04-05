<?php

/**
 * @file
 * Token integration for the message example module.
 */

/**
 * Implements hook_token_info().
 */
function message_example_token_info() {
  $info['tokens']['comment']['comment-teaser'] = array(
    'name' => t('Comment teaser'),
    'description' => t('Display a comment teaser.'),
  );
  $info['tokens']['node']['node-teaser'] = array(
    'name' => t('Node teaser'),
    'description' => t('Display a node teaser.'),
  );

  return $info;
}

/**
 * Implements hook_tokens().
 */
function message_example_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();

  // Only handeling comment and nodes.
  if (!in_array($type, array('comment', 'node'))) {
    return;
  }

  foreach ($tokens as $name => $original) {
    // Only hadeling "node/ comment teaser" tokens.
    if (!in_array($name, array('comment-teaser', 'node-teaser'))) {
      continue;
    }

    $replacements[$original] = message_example_trim_body($type, $data[$type]);
  }

  return $replacements;
}


/**
 * Trim the body field for a node/ comment.
 *
 * @param $entity_type
 *   The entity type (node or comment).
 * @param $entity
 *   The entity object.
 *
 * @return
 *   Body field text trimmed to 50 chars, with a "...Read more" suffix.
 */
function message_example_trim_body($entity_type, $entity) {
  $wrapper = entity_metadata_wrapper($entity_type, $entity);

  // Select the right field acoring to the entity type.
  $field_name = $entity_type == 'node' ? 'body' : 'comment_body';
  $field_value = $wrapper->{$field_name}->value();

  if (!$field_value) {
    return;
  }

  // Get the safe value.
  $field_value = $wrapper->{$field_name}->value->value();

  $options = array(
    'max_length' => 50,
    'html' => TRUE,
  );
  $trim_text = views_trim_text($options, $field_value);

  // Checking if we need to add the "Read more" link.
  if ($field_value != $trim_text) {
    $uri = entity_uri($entity_type, $entity);
    // Add a "Read more" before the closing <p>.
    $trim_text = substr_replace($trim_text, '... ' . l(t('Read more'), $uri['path'], $uri['options']) . '</p>', -4);
  }

  return $trim_text;
}
