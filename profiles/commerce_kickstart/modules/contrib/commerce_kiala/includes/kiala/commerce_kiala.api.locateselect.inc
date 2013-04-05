<?php

/**
 * @file
 * Provides classes for Kiala Locate and Select Web Service.
 */

/**
 * Base class for Kiala Locate and Select Web Service
 */
class CommerceKialaAPIQueryLocateSelectWS extends CommerceKialaAPIQuery {
  public $requiredQueryParams = array(
    'dspid',
    'country',
  );
  public $commonQueryParams = array(
    'dspid',
    'country',
  );

  protected function processResponse() {
    $response = $this->getResponse();
    if (empty($response)) {
      return $this;
    }

    $kplist = simplexml_load_string($response);
    $processed = array();
    if (!empty($kplist)) {
      foreach ($kplist as $kp) {
        $kp_data = $this->_parseKP($kp);
        if (isset($kp_data['shortId'])) {
          $processed[$kp_data['shortId']] = $kp_data;
        }
      }
    }

    $this->processedResponse = $processed;
    return $this;
  }

  /**
   * Parses a single KP point XML into an Array
   *
   * @ref http://locateandselect.kiala.com/info/ws
   * @ref http://locateandselect.kiala.com/schema/kplist-1.1.xsd
   */
  public function _parseKP(SimpleXMLElement $xml) {
    $return = array();

    if (!isset($xml['shortId'])) {
      return $return;
    }

    $shortID = (string)$xml['shortId'];
    if (empty($shortID)) {
      return $return;
    }

    // extract data from request to merge
    $queryParams = $this->getQueryParams();
    $addressRequest = array();
    if (isset($queryParams['country'])) {
      $addressRequest['country'] = $queryParams['country'];
    }

    // Parse point response
    $return['shortId'] = (string)$xml['shortId'];
    if (isset($xml->name)) {
      $return['name'] = strval($xml->name);
    }

    if (isset($xml->status)) {
      $return['status'] = array('value' => strval($xml->status));

      if (isset($xml->status['available'])) {
        $return['status']['available'] = intval($xml->status['available']);
      }
      if (isset($xml->status['code'])) {
        $return['status']['code'] = strval($xml->status['code']);
      }
      if (isset($xml->status['referenceDate'])) {
        $return['status']['referenceDate'] = strval($xml->status['referenceDate']);
      }
    }

    if (isset($xml->address)) {
      $return['address'] = (array) $xml->address;
      $return['address'] += $addressRequest;
    }

    if (isset($xml->remark)) {
      $return['remark'] = strval($xml->remark);
    }

    if (isset($xml->openingHours) && isset($xml->openingHours->day)) {
      $return['openingHours'] = array();
      foreach ($xml->openingHours->day as $day) {
        if (isset($day['name'])) {
          $day_name = drupal_strtolower(strval($day['name']));
          if ($day_name) {
            $return['openingHours'][$day_name] = array();
            if (isset($day->timespan)) {
              foreach ($day->timespan as $timespan) {
                $return['openingHours'][$day_name][] = (array) $timespan;
              }
            }
          }
        }
      }
    }

    if (isset($xml->picture) && isset($xml->picture['href'])) {
      $picture_uri = strval($xml->picture['href']);
      $return['picture'] = array('href' => $picture_uri);

      // save picture in managed files to allow derivative creation
      $existing_files = file_load_multiple(array(), array('uri' => $picture_uri));
      if (empty($existing_files)) {
        $file = remote_stream_wrapper_file_create_by_uri($picture_uri);
        if (!empty($file)) {
          file_save($file);
        }
      }
    }

    if (isset($xml->coordinate)) {
      $return['coordinate'] = (array) $xml->coordinate;
    }

    if (isset($xml->distance)) {
      $return['distance'] = array('value' => strval($xml->distance));
      if (isset($xml->distance['unit'])) {
        $return['distance']['unit'] = strval($xml->distance['unit']);
      }
    }

    if (isset($xml->label) && isset($xml->label->tag)) {
      $return['label'] = array();
      foreach ($xml->label->tag as $tag) {
        if (isset($tag['name'])) {
          $tag_name = strval($tag['name']);
          if ($tag_name) {
            $return['label'][$tag_name] = strval($tag);
          }
        }

      }
    }


    return $return;
  }
}

/**
 * Class for a Web Service Single KP Point Details
 */
class CommerceKialaAPIQueryLocateSelectWSPoint extends CommerceKialaAPIQueryLocateSelectWS {
  public function __construct($settings_group, $options = array()) {
    $this->requiredQueryParams[] = 'shortID';
    parent::__construct($settings_group, $options);
  }
}

/**
 * Class for Web Service KP Search
 */
class CommerceKialaAPIQueryLocateSelectWSSearch extends CommerceKialaAPIQueryLocateSelectWS {
  public function __construct($settings_group, $options = array()) {
    $this->requiredQueryParams[] = 'zip';
    parent::__construct($settings_group, $options);
  }
}


/**
 * Base class for Kiala Locate and Select Map Service
 */
class CommerceKialaAPIQueryLocateSelectMap extends CommerceKialaAPIQuery {
  public $requiredQueryParams = array(
    'dspid',
    'country',
  );
  public $commonQueryParams = array(
    'dspid',
    'country',
  );

  public function getQueryParams() {
    $queryParams = parent::getQueryParams();

    if (empty($queryParams['map'])) {
      if (!empty($this->settings[$this->settings_group . '_width'])) {
        if (!empty($this->settings[$this->settings_group . '_height'])) {
          $queryParams['map'] = $this->settings[$this->settings_group . '_width'] . 'x' .
                                $this->settings[$this->settings_group . '_height'];
        }
      }
    }

    return $queryParams;
  }

  public function request() {
    return $this;
  }
}

/**
 * Class for a Single KP Map Point Details
 */
class CommerceKialaAPIQueryLocateSelectMapPoint extends CommerceKialaAPIQueryLocateSelectMap {
  public function __construct($settings_group, $options = array()) {
    $this->requiredQueryParams[] = 'shortID';
    parent::__construct($settings_group, $options);
  }
}

/**
 * Class for KP Search Map
 */
class CommerceKialaAPIQueryLocateSelectMapSearch extends CommerceKialaAPIQueryLocateSelectMap {
  public function __construct($settings_group, $options = array()) {
    $this->requiredQueryParams[] = 'zip';
    parent::__construct($settings_group, $options);
  }
}
