#!/bin/bash

###
# Execute the Behat test suite against a prepared Pantheon site environment.
###

set -ex

SELF_DIRNAME="`dirname -- "$0"`"

# Require a target site
if [ -z "$TERMINUS_SITE" ] || [ -z "$TERMINUS_ENV" ]; then
	echo "TERMINUS_SITE and TERMINUS_ENV environment variables must be set"
	exit 1
fi

# Check for rejection file and exit if one exists
REJ_FILES=(`find . -type f -name '*.rej'`)
if [ ${#REJ_FILES[@]} -gt 0 ]; then
	echo "Merge conflict detected"
	exit 1
fi

PATH=$PATH:~/.composer/vendor/bin

# Create a drush alias file so that Behat tests can be executed against Pantheon.
terminus aliases
# Drush Behat driver fails without this option.
echo "\$options['strict'] = 0;" >> ~/.drush/pantheon.aliases.drushrc.php

export BEHAT_PARAMS='{"extensions" : {"Behat\\MinkExtension" : {"base_url" : "http://'$TERMINUS_ENV'-'$TERMINUS_SITE'.pantheonsite.io/"}, "Drupal\\DrupalExtension" : {"drush" :   {  "alias":  "@pantheon.'$TERMINUS_SITE'.'$TERMINUS_ENV'" }}}}'

# We expect 'behat' to be in our PATH. Our container symlinks it at /usr/local/bin
cd $SELF_DIRNAME && behat --config=behat.yml $*
