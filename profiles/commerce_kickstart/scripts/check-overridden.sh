#!/bin/bash

: ${DRUSH:=drush}
: ${DRUSH_ARGS:=}

FEATURES="commerce_kickstart_block commerce_kickstart_blog commerce_kickstart_merchandising commerce_kickstart_product commerce_kickstart_slideshow commerce_kickstart_social commerce_kickstart_user"

${DRUSH} ${DRUSH_ARGS} en -y diff

OVERRIDDEN=0
for feature in ${FEATURES}; do
  echo "Checking $feature..."
  if ${DRUSH} ${DRUSH_ARGS} features-diff ${feature} 2>&1 | grep -v 'Feature is in its default state'; then
    OVERRIDDEN=1
  fi
done

exit ${OVERRIDDEN}
