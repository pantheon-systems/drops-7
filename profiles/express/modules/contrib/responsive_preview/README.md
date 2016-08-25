# Responsive Theme Preview

The Responsive Preview module provides content and theme administrators with a
quick way to preview how their site's pages will appear at narrow and wide
dimensions. It provides a pre-configured list of devices and their
associated dimensions. Selecting a device from the drop down list launches a
preview overlay. The current page is displayed in the overlay at the dimensions
of the device. Although not a perfect reproduction of a device's page
rendering, it can provide a fast approximation of how a page's layout will
respond to various screen sizes.

## Dependencies

The module requires the [Libraries API](http://drupal.org/project/libraries)
module, as well as the [backbone](http://backbonejs.org/) and
[underscore](http://underscorejs.org/) Javascript libraries.

### Backbone

1. Download `backbone.js` from [GitHub](https://github.com/jashkenas/backbone).
2. Place the unzipped file in the `sites/all/libraries/backbone/` directory.
3. Optionally, also download the minified ("production") version, place it in
the same directory and name it `backbone-min.js`. The Navbar module will
automatically use the minified version if it's available, because it is more
efficient.

### Underscore

1. Download `underscore.js` from
[GitHub](https://github.com/jashkenas/underscore).
2. Place the unzipped file in the `sites/all/libraries/underscore/` directory.
3. Optionally, also download the minified ("production") version, place it in
the same directory and name it `underscore-min.js`. The Navbar module will
automatically use the minified version if it's available, because it is more
efficient.


## Installation

1. If not already installed, download and install the dependencies above.
2. Download the Responsive Preview module and follow the instruction for
[installing contributed modules](http://drupal.org/node/895232).

## Usage

The 7.x-1.x branch of Responsive Preview integrates with the
[Navbar](https://www.drupal.org/project/navbar) module. When you have Navbar
enabled, you will see an icon in the upper right corner of the bar (LTR) that,
when clicked, drops down to provide previewing options.

If you are not using the Navbar module, the Responsive Preview module provides
a block that exposes links to launch a device preview.
