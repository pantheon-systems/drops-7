#!/usr/bin/env bash

# Install latest Drush 8.
composer global require "drush/drush:8.*"
export PATH="$HOME/.composer/vendor/bin:$PATH"

# Build Behat dependencies.
cd $ROOT_DIR/express/tests/behat
composer install --prefer-dist --no-interaction
earlyexit

# Build Codebase.
cd $ROOT_DIR
drush dl drupal-7.59
mkdir drupal && mv drupal-7.59/* drupal/
mkdir profiles && mv express drupal/profiles/

# Harden Codebase.
cd $ROOT_DIR/drupal/modules
rm -rf php aggregator blog book color contact translation dashboard forum locale openid overlay poll rdf search statistics toolbar tracker trigger
earlyexit

# Setup files.
mkdir -p $ROOT_DIR/drupal/sites/default/files/styles/preview/public/gallery/ && chmod -R 777 $ROOT_DIR/drupal/sites
mkdir $ROOT_DIR/tmp && chmod -R 777 $ROOT_DIR/tmp

if [ "${BUNDLE_NAME}" != "null" ]; then

  # Move bundle to right place after build step.
  mkdir $ROOT_DIR/drupal/profiles/express/tests/behat/bundle_features
  cp -R $ROOT_DIR/$BUNDLE_NAME/tests/behat/features/* $ROOT_DIR/drupal/profiles/express/tests/behat/bundle_features
  mv $ROOT_DIR/$BUNDLE_NAME $ROOT_DIR/drupal/sites/all/modules/

fi

exit 0
