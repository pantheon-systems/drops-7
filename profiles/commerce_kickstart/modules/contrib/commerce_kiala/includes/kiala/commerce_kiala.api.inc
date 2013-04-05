<?php

/**
 * @file
 * Provides a base classes for all Kiala APIs.
 */

class CommerceKialaAPIQuery {
  public $settings_group = '';
  public $settings = array();
  public $options = array();

  public $requestPing = 0;
  protected $response;
  protected $processedResponse;

  const RESPONSE_CACHE_BIN = 'cache_commerce_kiala_responses';
  protected $responseCacheHandler;

  public $requiredQueryParams = array();
  public $commonQueryParams = array();


  public function __construct($settings_group, $options = array()) {
    $this->settings_group = $settings_group;
    $this->settings = commerce_kiala_settings(array(
      'include_passwords' => TRUE,
      'decrypt' => TRUE,
    ));

    if (!empty($options)) {
      $this->options = $options;
    }
  }

  public function getBaseURL() {
    if (!empty($this->settings[$this->settings_group . '_url'])) {
      return $this->settings[$this->settings_group . '_url'];
    }
  }

  public function getQueryParams() {
    $queryParams = array();

    if (!empty($this->options['query'])) {
      $queryParams += $this->options['query'];
    }

    if (!empty($this->commonQueryParams)) {
      $queryParams += array_intersect_key($this->settings, drupal_map_assoc($this->commonQueryParams));
    }

    if (!empty($this->settings[$this->settings_group . '_params'])) {
      $queryParams += $this->settings[$this->settings_group . '_params'];
    }

    return $queryParams;
  }

  public function getURL() {
    if ($url = $this->getBaseURL()) {
      if ($params = $this->getQueryParams()) {
        $url .= (strpos($url, '?') !== FALSE ? '&' : '?') . drupal_http_build_query($params);
      }

      return $url;
    }
  }

  public function request() {
    if (empty($this->options['allowMultipleRequests']) && $this->requestPing > 0) {
      return $this;
    }

    // check query parameter requirements
    if (!empty($this->requiredQueryParams)) {
      $request_params = $this->getQueryParams();
      foreach ($this->requiredQueryParams as $param_key) {
        if (empty($request_params[$param_key])) {
          return $this;
        }
      }
    }

    // build full url
    $request_url = $this->getURL();
    if (empty($request_url)) {
      watchdog('commerce_kiala', 'Missing API url for @group', array('@group' => $this->settings_group), WATCHDOG_ERROR);
      return $this;
    }

    // Extract request options, @see drupal_http_request()
    $request_options = array();
    if (!empty($this->options)) {
      $request_options = array_intersect_key($this->options, array(
        'headers' => array(),
        'method' => 'GET',
        'data' => NULL,
        'max_redirects' => 3,
        'timeout' => 30.0,
        'context' => NULL,
      ));
    }

    // reset response
    $this->response = NULL;

    // check cache
    $cacheHandler = $this->getResponseCacheHandler();
    if (!empty($cacheHandler->cache)) {
      $this->response = $cacheHandler->cache;
    }
    else {
      // perform request
      $response_object = NULL;
      try {
        $response_object = drupal_http_request($request_url, $request_options);
        $response_object->code = isset($response_object->code) ? $response_object->code : 200;
        $this->requestPing++;
      }
      catch (Exception $e) {
        watchdog('commerce_kiala', $e->getMessage(), array(), WATCHDOG_ERROR);
      }

      // set response and update cache
      if (isset($response_object) && !empty($response_object->data) && $response_object->code == '200') {
        $this->response = $response_object->data;
        $cacheHandler->cache = $response_object->data;
      }
    }

    return $this;
  }

  public function getResponse() {
    if (!isset($this->response)) {
      $this->request();
    }

    return $this->response;
  }

  public function getProcessedResponse() {
    if (!isset($this->processedResponse)) {
      $this->processResponse();
    }

    return $this->processedResponse;
  }

  protected function processResponse() {
  }

  public function getResponseCacheHandler() {
    if (!isset($this->responseCacheHandler)) {
      $this->responseCacheHandler = new CommerceKialaAPIQueryCache($this, self::RESPONSE_CACHE_BIN);
    }

    return $this->responseCacheHandler;
  }

  // -----------------------------------------------------------------------
  // Magic Methods

  /**
   * Get / Set
   *  - method is preferred with lazy fallback to object property
   */
  public function __get($name) {
    $method = 'get' . ucfirst($name);
    if (method_exists($this, $method)) {
      return $this->{$method}();
    }

    // allow lazy get
    if (isset($this->{$name})) {
      return $this->{$name};
    }
    return NULL;
  }

  public function __set($name, $value) {
    $camel_name = ucfirst($name);
    $method = 'set' . $camel_name;
    if (method_exists($this, $method)) {
      $this->{$method}($value);
    }
    elseif (!method_exists($this, "get_$camel_name")) {
      // allow lazy setting for non read only properties
      $this->{$name} = $value;
    }
  }

  public function __isset($name) {
    $value = $this->__get($name);
    return isset($value);
  }

  public function __unset($name) {
    unset($this->{$name});
  }
}



// -----------------------------------------------------------------------
// Cache Handling for API Queries

class CommerceKialaAPIQueryCache {
  public $bin = '';
  public $apiHandler = NULL;
  protected $cache;

  public function __construct(CommerceKialaAPIQuery $apiHandler, $bin) {
    $this->apiHandler = $apiHandler;
    $this->bin = $bin;
  }

  public function getEnabled() {
    $lifetime = $this->getLifetime();
    return isset($lifetime) && $lifetime != 'none' && is_numeric($lifetime);
  }

  public function getBin() {
    return $this->bin;
  }

  public function getCID() {
    $group_part = !empty($this->apiHandler->settings_group) ? $this->apiHandler->settings_group : 'kiala';
    $date_part = format_date(REQUEST_TIME, 'custom', 'Ymd');
    $url = $this->apiHandler->getURL();
    if (!empty($url)) {
      $url_part = drupal_hash_base64($url);
      return "$group_part::$date_part::$url_part";
    }
  }

  public function getLifetime() {
    if (isset($this->apiHandler->settings[$this->apiHandler->settings_group . '_cache_lifetime'])) {
      return $this->apiHandler->settings[$this->apiHandler->settings_group . '_cache_lifetime'];
    }
  }

  public function getCache() {
    if (!isset($this->cache)) {
      $this->cache = FALSE;
      $bin = $this->getBin();
      $cid = $this->getCID();

      if (!empty($cid)) {
        $record = cache_get($cid, $bin);

        // If no data was retrieved, return FALSE.
        if (!empty($record)) {
          // Check expiration if not permanent or temporary
          $expire = $record->expire;
          if ($expire == CACHE_PERMANENT || $expire == CACHE_TEMPORARY ||
              $expire < 0 || $expire > REQUEST_TIME) {
            $this->cache = $record->data;
          }
        }
      }
    }

    return $this->cache;
  }

  public function setCache($data) {
    $bin = $this->getBin();
    $cid = $this->getCID();

    if (!empty($cid) && $this->getEnabled()) {
      $lifetime = $this->getLifetime();
      $expire = CACHE_TEMPORARY;
      if (isset($lifetime)) {
        if ($lifetime > 0) {
          $expire = REQUEST_TIME + $lifetime;
        }
        elseif ($lifetime == CACHE_TEMPORARY || $lifetime == CACHE_PERMANENT) {
          $expire = $lifetime;
        }
      }

      cache_set($cid, $data, $bin, $expire);
    }

    $this->cache = $data;
    return $this;
  }

  public function reset() {
    $this->cache = NULL;
    return $this;
  }

  public function clear() {
    $bin = $this->getBin();
    $cid = $this->getCID();
    if (!empty($cid)) {
      cache_clear_all($cid, $bin);
    }

    $this->reset();
    return $this;
  }

  // -----------------------------------------------------------------------
  // Magic Methods

  /**
   * Get / Set
   *  - method is preferred with lazy fallback to object property
   */
  public function __get($name) {
    $method = 'get' . ucfirst($name);
    if (method_exists($this, $method)) {
      return $this->{$method}();
    }

    // allow lazy get
    if (isset($this->{$name})) {
      return $this->{$name};
    }
    return NULL;
  }

  public function __set($name, $value) {
    $camel_name = ucfirst($name);
    $method = 'set' . $camel_name;
    if (method_exists($this, $method)) {
      $this->{$method}($value);
    }
    elseif (!method_exists($this, "get_$camel_name")) {
      // allow lazy setting for non read only properties
      $this->{$name} = $value;
    }
  }

  public function __isset($name) {
    $value = $this->__get($name);
    return isset($value);
  }

  public function __unset($name) {
    unset($this->{$name});
  }
}
