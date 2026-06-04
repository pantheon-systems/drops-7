#!/bin/bash
# Shared helpers for solr9 CUJ scripts. Sourced, not executed.
# No set flags here: they would leak into the sourcing script's shell.

step() { echo ""; echo ">>>>>>>>>> $1 <<<<<<<<<<"; echo ""; }

drush_ev() {
  terminus drush "$SITE_ENV" -- ev "$@" 2>&1
}
