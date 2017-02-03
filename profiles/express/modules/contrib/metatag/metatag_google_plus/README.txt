Metatag: Google+
----------------
This module adds support for meta tag configuration for Google+ Snippet [1].

The following Google+ tags are provided:

* itemprop:name
* itemprop:description
* itemprop:image

Also itemtype is provided to add schema in the HTML markup as follows:

<html itemscope itemtype="http://schema.org/Article">


Usage
--------------------------------------------------------------------------------
Page type (itemtype) provides default type options from the Google+ Snippet page
[1]; to add other types either install select_or_other module [2] or use the
Metatag hooks (see metatag.api.php).


Installation
--------------------------------------------------------------------------------
The $schemaorg variable must be appended to the <html> tag in the html.tpl.php
file being used on the site, and it must be added after the $rdf_namespaces
variable, e.g.:

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print
  $language->language; ?>" version="XHTML+RDFa 1.0" dir="<?php print
  $language->dir; ?>"<?php print $rdf_namespaces; ?><?php print $schemaorg; ?>>


Credits / Contact
--------------------------------------------------------------------------------
Originally developed by Eric Chen [3] and sponsored by Monkii [4].


References
--------------------------------------------------------------------------------
1: https://developers.google.com/+/web/snippet/
2. https://drupal.org/project/select_or_other
3: https://drupal.org/user/265729
4: http://monkii.com
