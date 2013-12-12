PAYMILL-PHP
===========

[![Build Status](https://travis-ci.org/paymill/paymill-php.png)](https://travis-ci.org/paymill/paymill-php)
[![Latest Stable Version](https://poser.pugx.org/paymill/paymill/v/stable.png)](https://packagist.org/packages/paymill/paymill)
[![Total Downloads](https://poser.pugx.org/paymill/paymill/downloads.png)](https://packagist.org/packages/paymill/paymill)

How to test
-----------
There are different credit card numbers, frontend and backend error codes, which can be used for testing.
For more information, please read our testing reference.
https://www.paymill.com/en-gb/documentation-3/reference/testing/


Getting started with PAYMILL
----------------------------
If you don't already use Composer, then you probably should read the installation guide http://getcomposer.org/download/.

Please include this library via Composer in your composer.json and execute **composer update** to refresh the autoload.php.

```
{
    "require": {
        "paymill/paymill": "v3.0.0"
    }
}
```

1.  Instantiate the request class with the following parameters:
    $apiKey: First parameter is always your private API (test) Key

    ```php
        $request = new Paymill\Request($apiKey);
    ```
2.  Instantiate the model class with the parameters described in the API-reference:
    ```php
        $payment = new Paymill\Models\Request\Payment();
        $payment->setToken("098f6bcd4621d373cade4e832627b4f6");
    ```
3.  Use your desired function:

    ```php
        $response  = $request->create($payment);
        $paymentId = $response->getId();
    ```

    It recommend to wrap it into a "try/catch" to handle exceptions like this:
    ```php
        try{
            $response  = $request->create($payment);
            $paymentId = $response->getId();
        }catch(PaymillException $e){
            //Do something with the error informations below
            $e->getResponseCode();
            $e->getStatusCode();
            $e->getErrorMessage();
        }
    ```

Documentation
--------------

For further information, please refer to our official PHP library reference: https://www.paymill.com/en-gb/documentation-3/reference/api-reference/index.html
