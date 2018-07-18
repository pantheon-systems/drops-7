#!/usr/bin/env bash

# This function takes the output of the last command and exits if it failed.
earlyexit() {
  CODE="$?"
  if [ "${CODE}" != "0" ]; then
    echo ---
    echo Exiting on failure...
    echo ---
    exit 1
  fi
  echo Script succeeded.
}

findcommitmessage() {
  local FOUND="$(git log -2 --pretty=%B | awk '/./{line=$0} END{print line}' | grep '$1')"
  echo ${FOUND}
}

export -f earlyexit
export -f findcommitmessage
