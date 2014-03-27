A simpler version of the Color module.

Compatible with the Color module *.inc data structure. Instead of replacing colors in css based on color value, a variable substitution is performed based on the color variable name. All image handling has been removed. The Colorizer config form is built to be embeddable into other forms. An colorizer_instance alter hook is added to allow the name of the current colorizer profile to be changed (for example, to implement OG-specific color schemes).

In addition, the Preview mechanism is completely changed. Rather than using separate html/css/js files for preview, the css of the live site is altered. This allows the live theme to be updated as colors are changed.

This module was developed for Open Atrium 2 by Phase2.

Usage
-----

1) Copy the sample.colorizer.inc file and place it somewhere in your theme directory.  This file has the same format as the color.inc file used in the Color module.  It defines the different pre-defined color schemes you want users to select from, and defines the color variables that can be used in your css file.

2) Create a *.css file and place it somewhere in your theme.  This css file is loaded last (weight 100) and should contain the rules used to override the colors on your site.  This is a *template* file and can contain variables to represent colors.  The names of the variables are the same as the key values in the $info['fields'] array in the colorize.inc file from step 1.

To reference a color variable in the template css file, preceed the variable name with an @ character.  For example, @text will be replaced with the current "text" color.

3) To configure the module, go to admin/appearance/colorizer.  Fill in the name of the *.inc file (from 1) and *.css file (from 2) (relative to theme path).  Then modify the colors as desired.

Color extrapolation
-------------------

If a 'base' color is added to the scheme in the *.inc file, this will be used to extrapolate missing colors.  For example, rather than specifying the linkactive and linkhover color in each color scheme, a base color can be used to simply change the hue of the linkactive and linkhover colors used in the default color scheme.  Simply define the base color (which set the Hue of the scheme) and then leave the specific color variables out of the scheme.
