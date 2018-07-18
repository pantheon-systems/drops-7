<?php

function hook_inactive_users(&$vars) {
  $vars['TABLE NAME'] = array('table' => 'TABLE NAME', 'column' => 'COLUMN NAME');

  return $vars;
}
