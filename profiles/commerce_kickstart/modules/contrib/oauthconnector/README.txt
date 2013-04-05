/* $Id$ */

Description
===========
A module that makes it possible to log in through third party API:s
supporting OAuth. Makes it possible for other modules to define OAuth API:s
which users should be able to log in through and also exposes that ability to
the user who can add new API connections through an admin page.

Warning
=======
Oauth connections are not an easy subject. There are a lot of dependencies
and every provider has its own rules and requirements. This module is able
to make connections to all kinds of providers, including Facebook and Google.
This module does not solve your lack of knowledge of Oauth. Please try to
understand the API documentation of the providers and Oauth in general, before
filling the issue queue.
See: http://oauth.net/

Requirements
============
* connector
* oauth_common
* oauth2_common
* http_client
* http_client_oauth
* ctools
* API accounts with your oAuth providers (Facebook, Twitter, etc)

Installation
============
* Copy the module directory in to your Drupal
sites/all/modules directory as usual.

Usage
=====
Go to admin/structure/oauthconnector/list and create a new provider by using 
one of the presets the list, or by creating you own.  You will need an App Key
and a Secret Key for each provider (see list below). Under Advanced Settings, 
you can choose the scope (permission level), depending on the actions you are
going to be performing with each provider.  For example, if you would like to
get a user's email address from Facebook and publish to their wall, you would
want to enter "email,publish_stream" in the Scope field.

Once you have added a providers, users can add individual connections (for
example, bring up the Facebook Connect dialog) by clicking on the Connections 
tab on /user (user/1/connections) or by using the buttons in the block
'Connector'.

URLs to apply for App Keys (as of Mar. 2012)
============================================
* Facebook: https://graph.facebook.com/
* Twitter: https://api.twitter.com/
* Google: https://code.google.com/apis/console
* LinkedIn: https://www.linkedin.com/secure/developer
* Flickr: http://www.flickr.com/services

