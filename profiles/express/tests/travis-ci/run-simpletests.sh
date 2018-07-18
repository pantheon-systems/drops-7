#!/usr/bin/env bash

#chmod +x $HOME/drupal/scripts/run-tests.sh
cd ../../../../../../
FOO=`pwd`
echo "${OUTPUT}"

OUTPUT=`php ./scripts/run-tests.sh --url http://127.0.0.1:8080 --php ~/.phpenv/versions/$(phpenv version-name)/bin/php "Embed Templates"`
echo "${OUTPUT}"

if [[ $OUTPUT == *fail,* ]]; then
  echo 'failed'
  exit 1
else
  echo 'success'
  exit 0
fi
