
/*********************/
 Embedded Media Field
/*********************/

Author: Aaron Winborn
Development Began 2007-06-13

Requires: Drupal 5, Content (CCK)
Optional: Views

This extensible module will create fields for node content types that can be used to display video, image, and audio files
from various third party providers. When entering the content, the user will simply paste the URL or embed code
from the third party, and the module will automatically determine which content provider is being used. When displaying
the content, the proper embedding format will be used.

The module is only an engine, and requires a supported module to function. These include 'Embedded Image Field', 'Embedded
Video Field' and 'Embedded Audio Field'. These modules are included in the contrib folder of the module, so they can be easily
activated from the module administration page.

*************************************

The Embedded Image Field module currently only supports Flickr images. It creates a field that you can paste the url from a
Flickr photo into, which will then display that photo in a configurable size, with a link either to the node or to the
original page from Flickr.

The Embedded Video Field module already supports YouTube, Google, Revver, MySpace, MetaCafe, JumpCut, BrightCove, SevenLoad,
iFilm, Blip.TV, and Live Video video formats. More are planned to be supported soon. An api allows other third party video
providers to be supported using simple include files and provided hooks. (Developers: examine the documentation of
/providers/youtube.inc for help in adding support for new providers).

The Embedded Audio Field module will support third party audio content, such as Last.FM. It is still under development.

The administer of a site may decide whether to allow all content providers, or only a certain number of them. They may
further be limited when configuring the field.

Read the README.txt files in the individual contrib folders for more information.

Questions can be directed to winborn at advomatic dot com
