Manual Crop
===========

The Manual Crop module exposes a set of image style effects that allow you
to crop (and scale) an image after uploading.

Dependencies
------------
- Libraries 2.x
- jQuery plugins:
    - imagesLoaded:
        + Website: http://desandro.github.io/imagesloaded
        + Download: https://github.com/desandro/imagesloaded/archive/v2.1.2.tar.gz
    - imgAreaSelect:
        + Website: http://odyniec.net/projects/imgareaselect
        + Download: http://odyniec.net/projects/imgareaselect/jquery.imgareaselect-0.9.10.zip

Installation
------------
Start by downloading and installing the Libraries 2.x module.

Next download and extract the imagesLoaded plugin, rename the extracted folder to
"jquery.imagesloaded" and place it under "sites/all/libraries". The plugin should
now be located at "sites/all/libraries/jquery.imagesloaded/jquery.imagesloaded.min.js".

Please note that the 3.x version can also be used, but it depends on jQuery 1.5
which can only be obtained by installing the jQuery Update module.

Now download and extract the imgAreaSelect plugin, rename extracted folder to
"jquery.imgareaselect" and copy it into "sites/all/libraries". The plugin should
now be located at "sites/all/libraries/jquery.imgareaselect/scripts/jquery.imgareaselect.min.js".

When finished you can activate the module via the Modules page!

Configuration
-------------
After installing the module you need to configure your image styles before you
can start cropping. Go to Administration » Configuration » Media » Image styles
and click on the "edit" link for the styles that need a Manual Crop effect.

Add and configure one of the Manual Crop effects, you'll notice that the Manual
Crop effect will always become the first effect in the list. This is because
cropping should always be done first, otherwise the result will be unpredictable.

Next go to Administration » Structure » Content types and click on the "manage fields"
link (the Field UI module should be activated) for the content type that should
allow cropping. Now click on the "edit" link of the image field, so you can enable
and configure Manual Crop (open the "Manual Crop" fieldset) for the current field.

After saving the settings you should return to the content type overview and click
on "manage display" so you can set the (cropped) image style that should be used.
