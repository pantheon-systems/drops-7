
Setting up your environment for testing
=======================================

With composer, setting up things is fairly easy.

1) Download and install the composer manager module: 
   https://drupal.org/project/composer_manager

2) Run the drush command: "drush composer-manager install".
   If you are already using composer_manager with other modules, simply run:
   "drush composer-manager update"
   This should set up all of your dependencies and all of the libraries necessary
   to run behat tests.
 
By default, composer_manager will place all of the libraries at /sites/all/vendor.
Inside of the vendor directory there is a bin directory and inside of it we can
find the behat executable.

to run ECK's behat tests, first we need to configure behat with your local settings.

In eck/tests/behat/ there is a default.local.yml file. That is a sample configuration
file, that should work well for most people. cp the configuration file into the
same directory, and rename it behat.yml. Inside of the file there are 3 comments
that let you know things that you might have to configure for the tests to run
in your machine.

After configuring everything, eck/tests/behat, simply execute 
<path-to-vendor-directory>/bin/behat -v

NOTES: 
------
When trying to get things to run in Ubuntu 13.10, I ran into a few problems
with phpunit.

Following the instructions in these 2 articles (Given that you get the same
errors I did), fixed the problem for me:

http://stackoverflow.com/a/5939737
http://stackoverflow.com/a/8079653