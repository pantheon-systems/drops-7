<?php
/**
 * @file
 * Theme and preprocess functions for comments
 */

/**
 * Implements template_preprocess_comment().
 */
function radix_preprocess_comment(&$variables) {
  $comment = $variables['comment'];

  // Add proper classes.
  $variables['title_attributes_array']['class'][] = 'media-heading';

  // Use icon for permalink.
  $uri = entity_uri('comment', $comment);
  $uri['options'] += array(
    'attributes' => array(
      'class' => 'permalink muted pull-right',
      'rel' => 'bookmark',
    ),
    'html' => TRUE,
  );
  $icon_html = '<i class="icon-comment"></i>';
  $variables['permalink'] = l($icon_html, $uri['path'], $uri['options']);

  // Use icon for new.
  $variables['new'] = !empty($comment->new) ? '<i class="icon-comment"></i>' : '';
}
