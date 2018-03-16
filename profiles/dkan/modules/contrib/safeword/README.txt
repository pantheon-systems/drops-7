********************************************************************
                     D R U P A L    M O D U L E
********************************************************************
Name: Safeword Module
Author: Jeff Eaton <www.angrylittletree.com>
Co-maintainer: Robert Castelo <www.codepositive.com>
Project:  http://drupal.org/project/safeword
Drupal: 7.x
********************************************************************
DESCRIPTION:

It's often useful to provide two versions of a given string: one that's
intended for human viewing and another that's intended for use in database
queries, URLs, and so on. In Drupal, this is generally known as a 'Name' and
'Machine name' pair. Drupal 7 even provides a prefab FormAPI element to simplify
the process of entering these matched pairs.

Safeword exposes a custom FieldAPI field type that stores two strings using the
name/machine name approach. It can be useful when generating PathAuto aliases,
exposing Views arguments, and so on.

One common use is to not give users permission to 'Create and edit URL aliases' and instead give
them access to a Safeword field which uses the node title as it's source. So that users can only
edit a limited part of the path, keeping safe the other parts that your site features may rely on.

Transliteration
If you install the Transliteration module an option will apear on
each field to automatically transliterate the machine-name,
converting non-Roman characters into Roman characters without
accents.

https://drupal.org/project/transliteration


********************************************************************
INSTALLATION:

Note: It is assumed that you have Drupal up and running.  Be sure to
check the Drupal web site if you need assistance.

1. Place the entire directory into your Drupal directory:
   sites/all/modules/


2. Enable the module by navigating to:

   administration > modules

  Click the 'Save configuration' button at the bottom to commit your
  changes.


********************************************************************
USAGE

Add a 'Name/Machine Name' field to a content type and set it's display format.

There are three display formats to choose from:

* 'Human-readable version'
   The text entered, without any modification.

* 'Machine-readable version'
   The text entered, modified to make it suitable as a machine name (spaces stripped out, etc..).
   suitable for use in URL paths.

* 'Machine-readable version wrapped in an acronym tag'
   The text entered, modified to make it suitable as a machine name (spaces stripped out, etc..)
   wrapped in an <acronym> tag.

This module also provides each of these options as a token that can be used in various places.

More info on the Token system:

http://drupal.org/documentation/modules/token

If you need users to enter a machine name for a path, a good Replacement pattern would be:

(--|<[^<>]+>|[^/a-z0-9-])+

This allows '/' as part of the machine name.

The 'Show the complete path' option will display the full path to the node being created/edited
next to the source field of the machine name.







