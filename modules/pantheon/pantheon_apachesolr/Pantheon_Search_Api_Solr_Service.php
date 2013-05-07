<?php

class PantheonSearchApiSolrConnection extends SearchApiSolrConnection {

  function __construct(array $options) {

    // Adding in custom settings for Pantheon
    $options['scheme'] = 'https';
    $options['host'] = (variable_get('pantheon_hyperion_host')) ? variable_get('pantheon_hyperion_host') : 'index.' . variable_get('pantheon_tier', 'live') . '.getpantheon.com';
    $options['path'] = 'sites/self/environments/' . variable_get('pantheon_environment', 'dev') . '/index';
    $options['port'] = 449;
    $this->context = stream_context_create(
      array(
        'ssl' => array(
           'local_cert' => '../certs/binding.pem',
         )
      )
    );

    // Adding in general settings for Search API
    $options += array(
      'scheme' => 'http',
      'host' => 'localhost',
      'port' => 8983,
      'path' => 'solr',
      'http_user' => NULL,
      'http_pass' => NULL,
      'http_method' => 'POST',
      'local_cert'=> NULL,
    );
    $this->options = $options;

    $path = '/' . trim($options['path'], '/') . '/';
    $this->base_url = $options['scheme'] . '://' . $options['host'] . ':' . $options['port'] . $path;

    // Make sure we always have a valid method set, default to POST.
    $this->method = $options['http_method'] == 'GET' ? 'GET' : 'POST';

    // Set HTTP Basic Authentication parameter, if login data was set.
    if (strlen($options['http_user']) && strlen($options['http_pass'])) {
      $this->http_auth = 'Basic ' . base64_encode($options['http_user'] . ':' . $options['http_pass']);
    }
  }

}

class PantheonSearchApiSolrService extends SearchApiSolrService {

  protected $connection_class = 'PantheonSearchApiSolrConnection';

}