#!/bin/bash

###
# Prepare a Pantheon site environment for the Behat test suite, by pushing the
# requested upstream branch to the environment.
###

set -ex

if [ -z "$TERMINUS_SITE" ] || [ -z "$TERMINUS_ENV" ]; then
	echo "TERMINUS_SITE and TERMINUS_ENV environment variables must be set"
	exit 1
fi

###
# Clean up old unused environments, and make sure the dev site is spun up.
###
terminus build:env:delete:ci -n "$TERMINUS_SITE" --keep=2 --yes
terminus env:wake -n "$TERMINUS_SITE.dev"

###
# Create a new environment for this particular test run.
###
terminus build:env:create -n "$TERMINUS_SITE.dev" "$TERMINUS_ENV" --yes
terminus drush -n "$TERMINUS_SITE.$TERMINUS_ENV" -- updatedb -y
