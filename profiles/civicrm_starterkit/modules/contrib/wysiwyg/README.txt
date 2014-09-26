
-- SUMMARY --

Wysiwyg API allows to users of your site to use WYSIWYG/rich-text, and other
client-side editors for editing contents.  This module depends on third-party
editor libraries, most often based on JavaScript.

For a full description of the module, visit the project page:
  http://drupal.org/project/wysiwyg
To submit bug reports and feature suggestions, or to track changes:
  http://drupal.org/project/issues/wysiwyg


-- REQUIREMENTS --

* None.


-- INSTALLATION --

* Install as usual, see
  http://drupal.org/documentation/install/modules-themes/modules-7

* Go to Administration » Configuration » Content authoring » Wysiwyg,
  and follow the displayed installation instructions to download and install one
  of the supported editors.


-- CONFIGURATION --

* Go to Administration » Configuration » Content authoring » Text formats, and

  - either configure the Full HTML format, assign it to trusted roles, and
    disable "HTML filter", "Line break converter", and (optionally) "URL filter".

  - or add a new text format, assign it to trusted roles, and ensure that above
    mentioned input filters are disabled.

* Setup editor profiles in Administration » Configuration » Content authoring
  » Wysiwyg.


-- CONTACT --

Current maintainers:
* Daniel F. Kudwien (sun) - http://drupal.org/user/54136
* Henrik Danielsson (TwoD) - http://drupal.org/user/244227

This project has been sponsored by:
* UNLEASHED MIND
  Specialized in consulting and planning of Drupal powered sites, UNLEASHED
  MIND offers installation, development, theming, customization, and hosting
  to get you started. Visit http://www.unleashedmind.com for more information.

