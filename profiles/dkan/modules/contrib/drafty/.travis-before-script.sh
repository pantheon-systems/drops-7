#!/bin/bash

set -e $DRUPAL_TI_DEBUG

# Ensure the right Drupal version is installed.
# Note: This function is re-entrant.
drupal_ti_ensure_drupal

# Add needed dependencies.
cd "$DRUPAL_TI_DRUPAL_DIR"

# These variables come from environments/drupal-*.sh
mkdir -p "$DRUPAL_TI_MODULES_PATH"
cd "$DRUPAL_TI_MODULES_PATH"

# Normal dependencies - needs to be downloaded manually as cps specifies
# drafty_enforce, which drush does not like.
drush dl -y drafty

# Test dependencies of drafty.
drush dl -y field_collection entity_translation-7.x-1.0-beta3 title-7.x-1.x-dev
