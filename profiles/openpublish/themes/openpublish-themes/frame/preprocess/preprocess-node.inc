<?php

function frame_alpha_preprocess_node(&$vars) {
  $date_format = format_date($vars['created'], $type = 'custom', $format = 'm/d/y');
  if(module_exists('comment')) {
    // if we have comments enabled, append the comment count to the submitted data
    $comment_num = isset($vars['comment_count']) ? $vars['comment_count'] : FALSE;
    
    $comment_count = l(format_plural($comment_num, '1 comment', '@count comments'), 'node/' . $vars['nid'], array('fragment' => 'comments'));
    $vars['submitted'] = t('!username | !datetime | !comment_count', array('!username' => $vars['name'], '!datetime' => $date_format, '!comment_count' => $comment_count));
  }
  else {
    // otherwise, just format it up a bit nicer
    $vars['submitted'] = t('!username | !datetime', array('!username' => $vars['name'], '!datetime' => $date_format));
  }
}
