Service Links 2.x:
------------------
Author and mantainer: Fabio Mucciante aka TheCrow (since the 2.x branch)
Current co-mantainer: Simon Georges
Requirements:         Drupal 7
License:              GPL (see LICENSE.txt)

Introduction
------------
This module is the enhanced version of Service Links 1.x developed
by Fredrik Jonsson, rewritten and improved to fit the new purposes:
extend easily the number of services supported and provide APIs to
print links everywhere within any content.
At the address http://servicelinks.altervista.org/?q=service
a web interface helps to create a module including the services
not availables in the standard package.

Overview
---------
Service Links provide an amount of 70+ social networks
from around the World where submit the link of a given content,
below a short list:

* del.icio.us
* Digg
* Facebook
* Furl
* Google
* IceRocket
* LinkedIn
* MySpace
* Newsvine
* Reddit
* StumbleUpon
* Technorati
* Twitter
* Yahoo
* ...

The admin decides:
- the style to render the links: text, image, text + image
- to show links only for certain node types or some categories
- to add links within the content body, among the other links, or in a block
- what roles are allowed to see the selected links.

Within the 2.x branch has been introduced:
- modular management of services, grouped by different language area,
  through external modules implementing the hook_service_links()
- sorting of services through drag'n drop
- support for buttons which make use of Javascript without break the
  XHTML specifies to keep the module more 'accessible' as possible
- improved the use with not node pages
- support for other Drupal modules: Display Suite, Forward, Views, Short Url
- support for sprites to render the service images
- support for browser bookmarking (Chrome, Firefox, IE, Opera)
- two APIs to print easily the whole set of services or a customs subset of them
- configurable list of pages to show/hide on also through PHP code

A more detailed list of options and related explaining is available at the page:
http://servicelinks.altervista.org/?q=about

Installation and configuration
-------------------------------
1) Copy the whole 'service_links' folder under your 'modules' directory and then
   
2) Point your browser to administer >> modules', enable 'Service Links' and one
   of the 'XXX Services' provided, 'General Services' contain the most know social
   networks, and 'Widgets Services' the most used buttons

3) Go to 'administer >> access control' for allow users to watch the links.

4) At 'administer >> settings >> service links' select for what type of content
   enable Service Links and in 'Services' tab select the services to show.

More information
----------------

The file 'template.php' contains some examples about phptemplate variables

The file 'service_links.api.php' contains info about the hooks implemented

More info regarding installation and first configuration, set up of the available
options, either extension of the number of services and theming output are available
on the online documentation at the address:
http://servicelinks.altervista.org/?q=about

More services can be included and packed within an external module customizable
through a web interface available at the address:
http://servicelinks.altervista.org/?q=service
