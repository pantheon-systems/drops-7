<?php

/**
* Implements hook_drush_cache_clear().
*/
function rave_alerts_drush_cache_clear(&$types) {
  $types['rave_alerts'] = '_rave_alerts_cache_clear';
}

/**
* Utility function that clears all the entries in our cache bin.
*/
function _rave_alerts_cache_clear() {
  cache_clear_all('*', 'cache_rave_alerts', true);
}
