/* $Id: README.txt,v 1.2.2.1 2010/10/21 14:38:41 wesku Exp $ */

-- SUMMARY --

Add This module provides Drupal integration to addthis.com link sharing service.

Description from addthis.com: 
The AddThis button spreads your content across the Web by making it easier for your visitors to bookmark and
share it with other people, again... and again... and again. This simple yet powerful button is very easy to 
install and provides valuable Analytics about the bookmarking and sharing activity of your users. AddThis helps 
your visitors create a buzz for your site and increase its popularity and ranking.

AddThis is already on hundreds of thousands of websites including SAP, TIME Magazine, Oracle, Freewebs, 
Entertainment Weekly, Topix, Lonely Planet, MapQuest, MySpace, PGA Tour, Tower Records, Squidoo, Zappos, Funny 
or Die, FOX, ABC, CBS, Glamour, PostSecret, WebMD, American Idol, and ReadWriteWeb, just to name a few. Each 
month our button is displayed 20 billion times.

-- REQUIREMENTS --

None.

-- INSTALLATION --

Normal Drupal module installation, see http://drupal.org/node/70151 for further information.

For link sharing statistics registration at http://addthis.com/ is required, but the module will work even without registration.

-- CONFIGURATION --

There are two ways of using the module:

1) Display AddThis button in node links.
* Go to Configure / System / AddThis.
* Check Display on node pages.
* Optionally check Display in node teasers.
* If you want to limit AddThis visibility by content type, go to
  Structure / Content types and click the Edit link next to a content type.
  Visibility can be set for each content type in the "AddThis settings" section.

2) Use AddThis as a block.
* Go to Structure / Blocks and make the AddThis button block visible.

-- CUSTOMIZATION --

You have a number of options available at Configure / System / AddThis under Button image settings
and Widget settings. Image settings control the button image and widget controls the drop down and window that is opened when user clicks on a link sharing service. More information on how to customize your AddThis button can be found at http://addthis.com/help/customize/custom-button/
 
If configuration options are not flexible enough for you it is also possible to override theme_addthis_button in your own theme.

-- ROADMAP --

Future development of this module will include at least RSS support. CCK and Views integration may be included in future releases. Drupal 5.x version will no longer get any new features. Drupal 7.x version will be released soon after D7 code freeze.

-- CONTACT --

Current maintainers:
* Vesa Palmu (wesku) - http://drupal.org/user/75070

