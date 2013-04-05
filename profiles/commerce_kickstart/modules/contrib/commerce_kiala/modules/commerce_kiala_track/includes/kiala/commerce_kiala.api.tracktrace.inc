<?php

/**
 * @file
 * Provides classes for Kiala Locate and Select Web Service.
 */

/**
 * Base class for Kiala Locate and Select Web Service
 */
class CommerceKialaAPIQueryTracktrace extends CommerceKialaAPIQuery {
  public $requiredQueryParams = array(
    'tracking_number',
    'language'
  );

  public $commonQueryParams = array(
    'country',
    'dspid'
  );
/*
  protected function processResponse() {
    $response = $this->getResponse();
    if (empty($response))) {
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

  public function _parseKP($kp) {

  }*/
}

class CommerceKialaAPIQueryTracktraceEmbed extends CommerceKialaAPIQuery {
  public $requiredQueryParams = array(
    'dspparcelid', // tracking number
    'language'
  );

  public $commonQueryParams = array(
    'countryid',
    'dspid'
  );

  function getUrl() {
    // If a tracking number is not found, display the normal search page
    if (empty($this->options['query']['dspparcelid'])) {
      return $this->getBaseURL();
    }

    return parent::getURL();
  }
}
