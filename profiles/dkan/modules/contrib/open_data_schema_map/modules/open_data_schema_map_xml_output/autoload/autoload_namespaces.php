<?php

$library_path=implode(
  '/',
  array(
    DRUPAL_ROOT,
    libraries_get_path('symfonyserializer'),
  )
);

$vendorDir = $library_path;
$baseDir = $vendorDir;

return array();
