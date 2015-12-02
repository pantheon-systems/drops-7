<?php
if (PHP_SAPI == 'cli') {
  $cu_hash = variable_get("cu_sid", FALSE);
  $cu_path = variable_get("cu_path", FALSE);

  if ($cu_path) {
    $pathologic_string = "/$cu_hash\r\n" . 
    "/$cu_path\r\n" .
    "http://www.colorado.edu/$cu_hash\r\n" . 
    "http://www.colorado.edu/$cu_path\r\n" .
    "https://www.colorado.edu/$cu_hash\r\n" . 
    "https://www.colorado.edu/$cu_path";

    $format = filter_format_load("wysiwyg");

    if (empty($format->filters)) {
      // Get the filters used by this format.
      $filters = filter_list_format($format->format);
      // Build the $format->filters array...
      $format->filters = array();
      foreach($filters as $name => $filter) {
        foreach($filter as $k => $v) {
          $format->filters[$name][$k] = $v;
        }
      }
    }

    $format->filters["pathologic"]["settings"]["local_paths"] = $pathologic_string;

    filter_format_save($format);
  }
}
