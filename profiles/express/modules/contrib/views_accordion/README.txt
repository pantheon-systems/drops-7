
/* DESCRIPTION */

Views Accordion provides a display style plugin for the Views module.
It will take the results and display them as a jQuery UI accordion. It supports
grouping of fields and ajax pagination.


/* INSTALATION */

1. Place the views_accordion module in your modules directory (usually under
   /sites/all/modules/).
2. Go to /admin/modules, and activate the module (you will find it under the
   Views section).


/* USING VIEWS ACCORDION MODULE */

Your view must meet the following requirements:
  * Row style must be set to Fields
  * Provide at least two fields to show.

Choose Views Accordion in the Style dialog within your view, which will prompt
you to configure the accodion.

*        IMPORTANT       *
The first field WILL be used as the header for each accordion section, all
others will be displayed when the header is clicked. The module creates an
accordion section per row of results from the view. If the first field includes
a link, this link will not function, (the js returns false) Nothing will break
though.
**************************


/* THEMING INFORMATION */

Files included:
  * views-acordion.css - Just some styles to fix default styling problems in
    bartik.
  * views-view-accordion.tpl.php - copy/paste into your theme directory -
    please the comments in this file for requirements/instructions.

Both files are commented to explain how things work. Do read them to speed
things up.


/* ABOUT THE AUTHOR */

Views Accordion was created by Manuel Garcia
http://drupal.org/user/213194
