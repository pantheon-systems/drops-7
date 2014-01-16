<?php

interface Services_Paymill_LoggingInterface
{

    /**
     * Logging for PaymentProcessor
     *
     * @param type $message
     * @param type $debugInfo
     */
    public function log($message, $debugInfo);

}
