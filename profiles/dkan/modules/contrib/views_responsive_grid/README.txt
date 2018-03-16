CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Installation
 * How to use


INTRODUCTION
------------

Current Maintainer: Ian Whitcomb - http://drupal.org/user/771654

Views Responsive Grid provides a views plugin for displaying content in a
responsive(mobile friendly) grid layout. Rather than trying to force the
standard Views grid display to work for mobile this provides the same
functionality, but in DIVs instead of tables. Provided is also the ability to
specify a horizontal or vertical grid layout which will properly stack the
content on a mobile display.


INSTALLATION
------------

1. Download module and copy views_responsive_grid folder to sites/all/modules

2. Enable Views and Views Responsive Grid modules.


HOW TO USE
------------

After enabling the module, create a new view with the responsive grid display
format. Specify the number of columns, and the alignment of the grid.

You'll need to understand that the the module won't provide any default styling
to the grid so you may think it's not working, this is by design. In order for
the columns to work you'll need to specify the class name of your columns. For
example, if your theme utilizes a grid, like Twitter Bootstrap does, you would
specify "span3" as the column class(making sure to use the correct span size).
This will make sure your column adhere to the grid in your Bootstrap based
theme.
