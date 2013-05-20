#!/bin/bash
set -e

#
# Build the distribution using the same process used on Drupal.org
#
# Usage: scripts/build.sh [-y] <destination> from the profile main directory.
#

confirm () {
  read -r -p "${1:-Are you sure? [Y/n]} " response
  case $response in
    [yY][eE][sS]|[yY])
      true
      ;;
    *)
      false
      ;;
  esac
}

# Figure out directory real path.
realpath () {
  TARGET_FILE=$1

  cd `dirname $TARGET_FILE`
  TARGET_FILE=`basename $TARGET_FILE`

  while [ -L "$TARGET_FILE" ]
  do
    TARGET_FILE=`readlink $TARGET_FILE`
    cd `dirname $TARGET_FILE`
    TARGET_FILE=`basename $TARGET_FILE`
  done

  PHYS_DIR=`pwd -P`
  RESULT=$PHYS_DIR/$TARGET_FILE
  echo $RESULT
}

usage() {
  echo "Usage: build.sh [-y] <DESTINATION_PATH>" >&2
  echo "Use -y to skip deletion confirmation" >&2
  exit 1
}

DESTINATION=$1
ASK=true

while getopts ":y" opt; do
  case $opt in
    y)
      DESTINATION=$2
      ASK=false
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      usage
      ;;
  esac
done

if [ "x$DESTINATION" == "x" ]; then
  usage
fi

if [ ! -f drupal-org.make ]; then
  echo "[error] Run this script from the distribution base path."
  exit 1
fi

DESTINATION=$(realpath $DESTINATION)

case $OSTYPE in
  darwin*)
    TEMP_BUILD=`mktemp -d -t tmpdir`
    ;;
  *)
    TEMP_BUILD=`mktemp -d`
    ;;
esac
# Drush make expects destination to be empty.
rmdir $TEMP_BUILD

if [ -d $DESTINATION ]; then
  echo "Removing existing destination: $DESTINATION"
  if $ASK; then
    confirm && chmod -R 777 $DESTINATION && rm -rf $DESTINATION
    if [ -d $DESTINATION ]; then
      echo "Aborted."
      exit 1
    fi
  else
    chmod -R 777 $DESTINATION && rm -rf $DESTINATION
  fi
  echo "done"
fi

# Build the profile.
echo "Building the profile..."
drush make --no-cache --no-core --contrib-destination drupal-org.make tmp

# Build a drupal-org-core.make file if it doesn't exist.
if [ ! -f drupal-org-core.make ]; then
  cat >> drupal-org-core.make <<EOF
api = 2
core = 7.x
projects[drupal] = 7
EOF
fi

# Build the distribution and copy the profile in place.
echo "Building the distribution..."
drush make drupal-org-core.make $TEMP_BUILD
echo -n "Moving to destination... "
cp -r tmp $TEMP_BUILD/profiles/commerce_kickstart
rm -rf tmp
cp -r . $TEMP_BUILD/profiles/commerce_kickstart
mv $TEMP_BUILD $DESTINATION
echo "done"
