Version 1.9, February 24, 2013

-- SUMMARY --

Bamboo is a responsive Drupal 7 theme designed and developed
by Danny Englander (Twitter: @highrockmedia). Based on the CSS
Skeleton Framework, it allows for a choice of backgrounds,
color palettes, sidebars and fonts in the theme's settings page.

Bamboo is aimed at users who want to get a nice looking theme up
and running fast, and may not want to take the time to create
a sub-theme and fuss about with regions, settings, media queries,
and other super technical things. It's also aimed at the casual
Drupal user who has some familiarity with building sites. This
is designed with this type of user in mind, so it may not be
for the developer per se who would most likely use
Omega or Zen. This theme also does not require any base theme. There will
be an online demo soon and I considering offering some downloadable
Features as well, i.e. "Slideshow", "Gallery", etc...

-- CONFIGURATION --

- Configure theme settings in Administration » Appearance » Settings » bamboo
or admin/appearance/settings/bamboo and choose various options
available.

- For drop down menus to work, you need to set a main menu item to
"expanded". Then its sub-menus will work as drop downs.
If you need help with this, please consult Drupal core
documentation.

-- THEME SETTINGS UI --

- Choice of fonts:
 (e.g, choose sans-serif or serif for headers and body seperately.)

- Choice of several backgrounds and textures

- Choice of three color palettes

- Toggle Breadcrumbs on or off

- Choice of Sidebar left or right (Note no sidebar will appear automtically,
you must assign blocks to that region.)

- Default logo changes for each color palette
You can also toggle this off and use your own logo.)
***  Note, it's hard to anticipate what effect
various shapes and sizes a custom uploaded logos will have on the
theme so unfortunately support cannot be offered through the issue
queue for things like this.

- Local CSS
Choose to enable local.css file within the theme folder.

- Custom Path CSS
 Define a custom path for your own css file to use with the theme.

- Tertiary Menus
There is a theme setting checkbox if you will be using tertiary menus.
Check this box if you have sub-sub or third level drop down
menus. The setting does not change your menus, it merely styles
the last secondary level menu to not have rounded corners. This setting
was added to preseve the original styling of the theme for those who
are not using tertiary drop down menus. Note that tertiary menus in
mobile are indented already.

- Main Menu block region
Use this region if you turn off "Main Menu in the theme settings
and use your own third party menu system such
as Menu Block or Superfish Module. You are responsible for any
styling and CSS for this. (Use local.css as mentioned above.)

- Node block region
Use this region to have a block region within a
node which will appear right after the content but before any node links
or comments. Useful for ads or otherwise.

- Pinch and Zoom for Touch friendly devices
- Option to choose whether to pinch and zoom on a touch sensitive device or not. Default
is off. Note, there is no support for layouts breaking or otherwise if you choose to
enable this option.



-- ADDITIONAL FEATURES --

- JQuery Image captions on default imagefield in Article content type

- Responsive for phone, tablet, and desktop using media queries

- Mobile friendly menu

- Drop down menus (for desktop)

- Tertiary drop down menu styling

-- REQUIREMENTS --

No base theme needed, simply use this theme on its own.

-- INSTALLATION --

Install as usual, see http://drupal.org/node/176045 for further information.

-- CUSTOMIZATION --

* As with any other Drupal theme you are able to customize just about every
aspect of this theme but some nice defaults are provided out of the box
with Bamboo.

- drupal.org theme guide is here : http://drupal.org/documentation/theme

-- UPGRADING --
Nothing too tricky here other than if you have a local.css file as
per the documentation. When upgrading, you must preserve local.css
somewhere, otherwise it could get overwritten with the upgrade.
After you upgrade, you can then drop local.css back in to the theme.
Of course if you have modified other files, they will all get overwritten.
In many cases, a subtheme is probably recommended then as
opposed to using local.css. You can create a sub-theme of your
own to put all your overrides in: "Creating a sub-theme"
- http://drupal.org/node/225125 A future version of this
theme may allow for a custom path for local.css to avoid upgrade snags.

-- NOTES --

- This theme supports CSS3 features, i.e. round corners for modern browsers.
- There is no support for IE8 and below though there is an IE8 stylesheet
in the theme and some defaults are provided that it should work ok.

- Inspiration for this theme comes from my surroundings in glorious sunny
Southern California!

If you required specific customizations that you are not able to do on your own,
I can offer paid support. Please email me: contact@highrockmedia.com or
through my website's contact form. http://highrockmedia.com/contact-us

Buy me a Latte
- Help support Bamboo and the Bamboo slideshow.
http://highrockmedia.com/buy-me-latte

------------------------
Danny Englander
Drupal Themer and Photographer
High Rock Media
San Diego, California
http://highrockmedia.com
http://highrockphoto.com
