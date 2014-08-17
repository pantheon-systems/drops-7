RESPONSIVE MENUS
----------------
Responsify your menus! Just give me a CSS or jQuery style selector
of your menu and I will make it mobile friendly (when the time is right).
https://drupal.org/project/responsive_menus


CONFIGURATION
-------------
For many use cases, the default settings will be enough.  Turn it on = done.
I am a huge fan of 0-config, too many things to configure in Drupal already.
That said, there are plenty of configuration options at:
/admin/config/user-interface/responsive_menus

Alternatively, you can use Responsive Menus as a Context reaction with all
the same options.

These options vary depending on the style chosen, but may include:
-CSS/jQuery selectors for which menus to responsify
-Text or HTML to use as the menu toggle button
-Screen width to respond to
-Open from Left or Right side of screen
-Remove other classes/id's when responded


GOOD TO KNOW
------------
The 'Simple' and 'MeanMenu' styles are included with this module, the others
must be downloaded.  The URL to download a library will be presented to you if
you try to choose any other style.

MeanMenu & codrops' Multi styles require jQuery 1.7+

Sidr & codrops integrate with the Libraries 2.x module in order to work.
-This can be bypassed using hook_responsive_menus_styles_alter.
-See API & HOOKS below.

Google Nexus style takes over all the time (not just small screen).
-May be updated later to have a mobile only option.


API & HOOKS
-----------
hook_responsive_menus_style_info()
- Declare your own style.
hook_responsive_menus_styles_alter(&$styles)
- Alter existing styles.
@see responsive_menus.api.php


AUTHOR INFO
-----------
Joshua Walker 'drastik'
http://drastikbydesign.com
https://drupal.org/user/433663
