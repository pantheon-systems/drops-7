Commerce Paymill
================

Introduction
------------

Commerce Paymill is [Drupal
Commerce](https://drupal.org/project/commerce) module that integrates
the [Paymill](https://paymill.com) payement gateway into your Drupal
Commerce shop.

All development happens on the 2.x branch. The 1.x branch is
**unmaintained** and will have no further releases.

Features
--------

1.  *SSL peer verification*; it's a sad fact that the vast majority of
    available payment modules do not do SSL peer validation, thus
    rendering the site vulnerable to Man in The Middle (MiTM) attacks.
    If you're not doing it, you're throwing away all the web-of-trust
    funcionality of the way SSL works on the web, the most critical part
    of the system.

2.  *multiple currencies* support.

3.  *pre-authorization* and *capture* — thus avoiding refund charges
    for you as a merchant in the case of a return by a customer, also
    allowing complete control of order balancing.

    N.B. Please bear in mind that currently the module doesn't support
    multiple captures. Multiple captures will arrive in a future
    release. Right now you can capture only and only *one* time.

4.  *card on file* functionality that allows for you securely to
    charge a client card without having to deal with the huge hassle of
    storing credit card numbers.

5.  *proxy* support allowing it to work out of the box on sites using
    exit proxies for security reasons.

Note that to enable the card on file funcionality you need to install
the *2.x* version of the
[`commerce_cardonfile`](https://drupal.org/project/commerce_cardonfile)
module.

Basic Installation
------------------

1.  Download and enable the module.

2.  Use the drush command to download the PHP library from
    [github](https://github.com/paymill/paymill-php).

        drush paymill <directory>

    where `directory` is the directory where the module should be
    installed. By default it installs on `sites/all/modules`, if on a
    *multisite* install and you want to make it available for a
    particular `sitename` do:

        drush paymill sites/<sitename>/libraries 

3.  Get a [paymill](https://paymill.com) account and configure the
    payment rule at `admin/commerce/config/payment-methods`.

4.  Configure the payment rule (the `edit` link) with your keys.

5.  Done.



Installation with exit proxy support
------------------------------------

A good practice for ecommerce sites keen on security is to use an exit
proxy with a white list to filter out all server to server
communication.

This exit proxy is usually a forward proxy like
[Polipo](https://en.wikipedia.org/wiki/Polipo) or
[Squid](https://en.wikipedia.org/wiki/Squid_(software)) configured with
a whitelist for allowable hosts to be requested by the server side
application being used.

Here are the steps to install the module when using a proxy:

1.  Download and enable the module.

2.  Use the drush command to download the PHP library from
    [github](https://github.com/paymill/paymill-php).

        drush paymill <directory>

    where `directory` is the directory where the module should be
    installed. By default it installs on `sites/all/modules`, if on a
    *multisite* install and you want to make it available for a
    particular `sitename` do:

        drush paymill sites/<sitename>/libraries 

3.  Get a [paymill](https://paymill.com) account and configure the
    payment rule at `admin/commerce/config/payment-methods`.

4.  Configure the payment rule (the `edit` link) with your keys.

5.  On the `Proxy settings` fieldset configure the proxy.

6.  Add the proxy host, it can be `localhost` any domain name that can
    be resolved or an IP address.

7.  Configure the port number, by default it uses `8080`.

8.  Is the proxy a [`SOCKS5`](https://en.wikipedia.org/wiki/SOCKS) or a
    HTTP proxy? Configure it.

9.  Does the proxy require authentication? If so then:

    9.1 Choose the authentication method. Defaul is `Basic Auth`. It
    supports `NTLM` also.

    9.2 Add the username.

    9.3 Add the password.

10. Done.


Roadmap
-------

1.  Release 3.0 adds multi-capture support.

2.  Release 4.0 allows for subscribing Paymill from the module's UI
    without the neeed to login into Paymill whatsoever.

Development of the module is sponsored by
[CommerceGuys](http://commerceguys.com) and
[Paymill](https://paymill.com).

