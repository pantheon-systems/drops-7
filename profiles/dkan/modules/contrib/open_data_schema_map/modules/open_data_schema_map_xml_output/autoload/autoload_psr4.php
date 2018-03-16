<?php

$library_path=implode(
  '/',
  array(
    DRUPAL_ROOT,
    libraries_get_path('symfonyserializer'),
  )
);

return array(
    'Symfony\\Component\\Serializer\\' => array($library_path),
);
