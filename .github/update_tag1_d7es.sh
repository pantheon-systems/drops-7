#!/bin/bash
set -euo pipefail

main() {
  # Variables
  local MODULE_NAME="tag1_d7es"
  local PANTHEON_UPSTREAM_DIR="modules/pantheon/$MODULE_NAME"
  local PR_BASE_BRANCH="default"
  local CURRENT_VERSION
  local LATEST_VERSION
  local TEMP_BRANCH
  TEMP_BRANCH="update-$MODULE_NAME-$(date +%Y%m%d%H%M%S)"

  # With these credentials, commits will be pushed to 'master' and available on
  # the upstream immediately.
  local GIT_USER="bot@getpantheon.com"
  local GIT_NAME="Pantheon Automation"

  local RELEASE_HISTORY_URL="https://updates.drupal.org/release-history/${MODULE_NAME}/7.x"
  LATEST_VERSION=$(curl -s "$RELEASE_HISTORY_URL" \
    | grep -oE '<version>[0-9a-zA-Z\.\-]+' \
    | sed 's/<version>//' \
    | grep -vE '(-rc|-beta|-alpha)' \
    | sort -V | tail -n 1
  )

  CURRENT_VERSION=$(grep '^version' "${PANTHEON_UPSTREAM_DIR}/tag1_d7es.info" \
    | awk -F' = ' '{print $2}' | tr -d '"'
  )

  echo "Current: ${CURRENT_VERSION}"
  echo "Latest: ${LATEST_VERSION}"

  if [ "$LATEST_VERSION" == "$CURRENT_VERSION" ]; then
    echo "Already up to date."
    exit 0
  fi

  local PR_TITLE="Update $MODULE_NAME to version $LATEST_VERSION"
  local PR_EXISTS
  PR_EXISTS=$(gh pr list --state open --search "$LATEST_VERSION" --json title \
    | jq --arg pr_title "$PR_TITLE" '.[] | select(.title | contains($pr_title))'
  )

  if [ -n "$PR_EXISTS" ]; then
    echo "A PR for version $LATEST_VERSION already exists. Skipping PR creation."
    exit 0
  fi

  git checkout -b "$TEMP_BRANCH"

  local DRUPAL_ORG_FTP_GZ_URL="https://ftp.drupal.org/files/projects/$MODULE_NAME-$LATEST_VERSION.tar.gz"
  wget -qO- "$DRUPAL_ORG_FTP_GZ_URL" | tar -xz -C /tmp

  rsync -ar --delete "/tmp/$MODULE_NAME/" "$PANTHEON_UPSTREAM_DIR/"
  rm -rf "$PANTHEON_UPSTREAM_DIR/.github"
  rm -f "$PANTHEON_UPSTREAM_DIR/.gitlab-ci.yml"
  rm -rf "$PANTHEON_UPSTREAM_DIR/tests"

  git config user.email "${GIT_USER}"
  git config user.name "${GIT_NAME}"

  git add "$PANTHEON_UPSTREAM_DIR"
  git commit -m "Update $MODULE_NAME to version $LATEST_VERSION"
  git push origin "$TEMP_BRANCH"

  local PR_BODY="Updates the $MODULE_NAME module to version $LATEST_VERSION."
  gh pr create --title "$PR_TITLE" \
      --body "$PR_BODY" \
      --head "$TEMP_BRANCH" \
      --base "$PR_BASE_BRANCH"
}

main