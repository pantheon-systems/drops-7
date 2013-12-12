<?php

namespace Paymill\Services;

use Paymill\Models\Response\Error;

/**
 * PaymillException
 */
class PaymillException extends \Exception
{

    private $_errorMessage;
    private $_responseCode;
    private $_httpStatusCode;

    /**
     *
     * @param Error $errorModel
     * @param string $message
     * @param int $code
     * @param Exception $previous
     */
    public function __construct($responseCode = null, $message = null, $code = null)
    {
        parent::__construct($message, $code, null);
        $this->_errorMessage = $message;
        $this->_responseCode = $responseCode;
        $this->_httpStatusCode = $code;
    }

    /**
     * @return string
     */
    public function getErrorMessage()
    {
        return $this->_errorMessage;
    }

    /**
     * @return string
     */
    public function getStatusCode()
    {
        return $this->_httpStatusCode;
    }

    /**
     * @return integer
     */
    public function getResponseCode()
    {
        return $this->_responseCode;
    }

}
