Paymill-PHP
===========

[![Build Status](https://travis-ci.org/paymill/paymill-php.png?branch=master)](https://travis-ci.org/paymill/paymill-php)

Getting started with Paymill
----------------------------

1.  Include the required PHP file from the paymill PHP library. For example via: 

        require_once 'lib/Services/Paymill/Transactions.php';

2.  Instantiate the class, for example the Services_Paymill_Transactions class, with the following parameters:

    $apiKey: First parameter is always your private API (test) Key

    $apiEndpoint: Second parameter is to configure the API Endpoint (with ending /), e.g. "https://api.paymill.de/v2/"
	
        $transactionsObject = new Services_Paymill_Transactions($apiKey, $apiEndpoint);

3.  Make the appropriate call on the class instance. For additional parameters please refer to the API-Reference:

        $transactionsObject->create($params);

API versions
--------------

The master branch reflects the newest API version, which is v2 for now. In order to use an older version just checkout the corresponding tag.
	
For further information, please refer to our official PHP library reference: https://www.paymill.com/en-gb/documentation-3/reference/api-reference/index.html
