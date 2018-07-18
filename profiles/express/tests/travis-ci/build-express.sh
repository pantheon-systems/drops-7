#!/usr/bin/env bash

if [ "${BUNDLE_NAME}" != "null" ]; then
  cd ${ROOT_DIR}/drupal/sites/all/modules/${BUNDLE_NAME}
else
  cd ${ROOT_DIR}/drupal/profiles/express
fi

EXPRESS_COMMIT_HAS_BUILD="$(git log -2 --pretty=%B | awk '/./{line=$0} END{print line}' | grep '==build')"

# https://docs.travis-ci.com/user/caching/
# Travis takes the cache of the default branch if the PR branch doesn't have one.
# So, if this is a merge into dev, we need to delete the db export.
if [  "${TRAVIS_EVENT_TYPE}" == "push" ]; then
  echo Removing cached db on merge into dev...
  rm -f $HOME/cache/express.sql
fi

# Build Express if no db export or commit doesn't say "!===build".
if [ ! -f $HOME/cache/express.sql ] || [ "${EXPRESS_COMMIT_HAS_BUILD}" ]; then

  # Install site like normal.
  echo Installing Express...
  $HOME/.composer/vendor/bin/drush si express --db-url=mysql://root:@127.0.0.1/drupal --account-name=admin --account-pass=admin --site-mail=admin@example.com --site-name="Express" --yes
  earlyexit

  # Export db so it can be imported after every suite run.
  # Test runs that fill up the db with nodes can impact other tests.
  echo Exporting database...
  $HOME/.composer/vendor/bin/drush sql-dump --result-file=$HOME/cache/express.sql

else

  # Import db if it is already built.
  echo Importing Express database...
  $HOME/.composer/vendor/bin/drush sql-cli < $HOME/cache/express.sql
  earlyexit

  # Run any database updates.
  echo Running pending database updates...
  $HOME/.composer/vendor/bin/drush updb -y

fi

# Check and see if testing core module is enabled.
$HOME/.composer/vendor/bin/drush pm-info travis_hosting
$HOME/.composer/vendor/bin/drush pm-info ng_hosting
$HOME/.composer/vendor/bin/drush pm-info cu_core
$HOME/.composer/vendor/bin/drush pm-info cu_ldap
$HOME/.composer/vendor/bin/drush pm-info cu_local_users

exit 0
