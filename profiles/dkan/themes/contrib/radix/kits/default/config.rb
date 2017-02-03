# Require any additional compass plugins here.
require 'bootstrap-sass';
require "compass_radix";
require 'sass-globbing';
require File.join(File.dirname(__FILE__), 'extensions/css_splitter/css_splitter.rb');

# Set environment [development, production]
environment = :development

# Set this to the root of your project when deployed:
http_path = "/"
css_dir = "assets/stylesheets"
sass_dir = "assets/sass"
images_dir = "assets/images"
fonts_dir = "assets/fonts"
javascripts_dir = "assets/javascripts"
extensions_dir = "extensions"

# You can select your preferred output style here (can be overridden via the command line):
# output_style = :expanded or :nested or :compact or :compressed
output_style = (environment == :development) ? :expanded : :compressed

# To enable relative paths to assets via compass helper functions. Uncomment:
relative_assets = true

# To disable debugging comments that display the original location of your selectors. Uncomment:
line_comments = (environment == :development) ? true : false

# If you prefer the indented syntax, you might want to regenerate this
# project again passing --syntax sass, or you can uncomment this:
# preferred_syntax = :sass
# and then run:
# sass-convert -R --from scss --to sass sass scss && rm -rf sass && mv scss sass

# Split css files if it goes over IE's 4095 limit.
on_stylesheet_saved do |path|
  CssSplitter.split(path) unless path[/\d+$/]
end
