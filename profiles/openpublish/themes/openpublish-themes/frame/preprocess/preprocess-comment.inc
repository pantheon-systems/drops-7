<?php
function frame_alpha_preprocess_comment(&$vars) {
  $comment = $vars['comment'];
  $date_format = format_date($comment->created, $type = 'custom', $format = 'm/d/y');  
  $vars['submitted'] = t('!username | !datetime', array('!username' => $vars['author'], '!datetime' => $date_format));
}