Commerce Paymill
================

Introduction
------------

Commerce Paymill is Drupal Commerce module that integrates
the [Paymill](https://paymill.com) payement gateway into your Drupal
Commerce shop.

All development happens on the 2.x branch. The 1.x branch is
*unmaintained* and will have no further releases.

Features
--------

1.  multiple currencies support.
2.  pre-authorization and capture — thus avoiding refund charges
    for you as a merchant in the case of a return by a customer, also
    allowing complete control of order balancing.\
3.  card on file functionality that allows for you securely to
    charge a client card without having to deal with the huge hassle of
    storing credit card numbers.

Note that to enable the card on file funcionality you need to install
the 2.x version of the commerce_cardonfile module.

Installation
------------

1.  Download and enable the module.

2.  Use the drush command to download the PHP library from
    [github](https://github.com/paymill/paymill-php).

        drush paymill <directory>

    where `directory` is the directory where the module should be
    installed. By default is `sites/all/modules`, if on a **multisite**
    install and you want to make it available for a given `sitename` do:

        drush paymill sites/<sitename>/libraries

3.  Get a [paymill](https://paymill.com) account and configure the
    payment rule at `admin/commerce/config/payment-methods`.

4.  Configure the payment rule (the `edit` link) with your keys.

5.  Done.

Roadmap
-------

1.  Release 2.1 will have all of the above and:

-   proxy support for sites that use a forward proxy to whitelist server
    to server calls as a security measure.
-   informative translatable error messages for the client when an error
    occurs.

2.  Release 2.2 adds extensive logging for security and analytics.

Development of the module is sponsored by
[CommerceGuys](http://commerceguys.com) and
[Paymill](https://paymill.com).
