<?php

namespace Paymill\Models\Response;

/**
 * Error
 *
 * @category   PayIntelligent
 * @copyright  Copyright (c) 2011 PayIntelligent GmbH (http://payintelligent.de)
 */
class Error
{

    /**
     * @var string
     */
    private $_errorMessage;
    /**
     * @var int
     */
    private $_responseCode;

    /**
     * @var int
     */
    private $_httpStatusCode;

    /**
     * Returns the error message stored in the model
     * @return string
     */
    public function getErrorMessage()
    {
        return $this->_errorMessage;
    }

    /**
     * Sets the error message stored in this model
     * @param string $errorMessage
     * @return \Paymill\Lib\Models\Response\Error
     */
    public function setErrorMessage($errorMessage)
    {
        $this->_errorMessage = $errorMessage;
        return $this;
    }

    /**
     * Returns the response code
     * @return int
     */
    public function getResponseCode()
    {
        return $this->_responseCode;
    }

    /**
     * Sets the response code
     * @param int $responseCode
     * @return \Paymill\Lib\Models\Response\Error
     */
    public function setResponseCode($responseCode)
    {
        $this->_responseCode = $responseCode;
        return $this;
    }

    /**
     * Returns the status code
     * @return int
     */
    public function getHttpStatusCode()
    {
        return $this->_httpStatusCode;
    }

    /**
     * Sets the status code
     * @param int $httpStatusCode
     * @return \Paymill\Lib\Models\Response\Error
     */
    public function setHttpStatusCode($httpStatusCode)
    {
        $this->_httpStatusCode = $httpStatusCode;
        return $this;
    }

}
