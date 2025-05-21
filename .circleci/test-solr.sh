#!/bin/bash
set -euo pipefail

export TERMINUS_HIDE_GIT_MODE_WARNING=1
# This script is used to run the Solr tests in a CircleCI environment.

main() {
    if [[ -z ${SITE_ID:-} || -z ${ENV:-} ]]; then
        echo "SITE_ID and ENV environment variables must be set"
        exit 1
    fi

    local TERMINUS_ENV="local"
    if [[ "${CI:-}" == "true" ]]; then 
        # GHA. Not the most robust.
        TERMINUS_ENV="ci-${GITHUB_RUN_ID}"
    fi

    local SITE_ENV="${SITE_ID}.${TERMINUS_ENV}"


    # Get the list of environment IDs
    envs=$(terminus env:list "$SITE_ID" --field=ID)
    # Create the environment if it doesn't exist
    if ! echo "$envs" | grep -qx "$TERMINUS_ENV"; then
        terminus env:create "$SITE_ID".dev "$TERMINUS_ENV"
    fi
    terminus env:wipe "$SITE_ENV" --yes

    terminus connection:set "$SITE_ENV" sftp
    # Install Drupal
    terminus drush "$SITE_ENV" -- site-install -y
    terminus env:commit "$SITE_ENV" --message="Install Drupal"
    sleep 2
    # Install additional modules
    terminus drush "$SITE_ENV" -- site-install -y
    terminus drush "$SITE_ENV" -- dl apachesolr
    terminus drush "$SITE_ENV" -- en apachesolr -y	
    terminus drush "$SITE_ENV" -- dl search_api
    terminus drush "$SITE_ENV" -- en search_api -y	
    terminus env:commit "$SITE_ENV" --message="Install apachesolr & search_api"
    terminus drush "$SITE_ENV" -- en pantheon_apachesolr -y	
    terminus drush "$SITE_ENV" -- cc all

    echo "==[ ApacheSolr: Clear and reindex ]=="
    terminus drush "$SITE_ENV" -- solr-delete-index -y
    terminus drush "$SITE_ENV" -- solr-mark-all
    terminus drush "$SITE_ENV" -- solr-index
    sleep 10  # adjust based on content size

    echo "==[ ApacheSolr: Run search ]=="
    if terminus drush "$SITE_ENV" -- solr-search "Hello" --pipe | grep -q .; then
        exit 1
    fi

    echo "==[ Search API: List indexes ]=="
    INDEXES=$(terminus drush "$SITE_ENV" -- search-api-list)
    if [ -z "$INDEXES" ]; then
        echo "No Search API indexes found: FAIL"
        exit 1
    fi
    FIRST_INDEX_ID=$(echo $INDEXES | awk '/^[[:space:]]*[0-9]+[[:space:]]+/' | head -n1 | awk '{print $1}')
    echo "==[ Search API: Clear and reindex index: $FIRST_INDEX_ID ]=="
    terminus drush "$SITE_ENV" -- search-api-clear "$FIRST_INDEX_ID"
    terminus drush "$SITE_ENV" -- search-api-index "$FIRST_INDEX_ID"
    sleep 5


    echo "==[ All Solr tests passed ✅ ]=="
}

main "$@"