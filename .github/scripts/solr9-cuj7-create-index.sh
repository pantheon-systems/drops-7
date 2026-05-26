#!/bin/bash
set -euo pipefail

# CUJ 7: Create and Configure a Search Index
# Tests that content can be indexed into Solr 9.
# Accepts MODULE env var: apachesolr or search_api_solr

SITE_ENV="${TERMINUS_SITE}.${MULTIDEV}"
MODULE="${MODULE:?MODULE env var must be set}"

step() { echo ""; echo ">>>>>>>>>> $1 <<<<<<<<<<"; echo ""; }

drush_ev() {
  terminus drush "$SITE_ENV" -- ev "$@" 2>&1
}

step "CUJ 7: Create and Configure Search Index ($MODULE)"

if [ "$MODULE" = "apachesolr" ]; then

  step "Step 1: Configure indexing and mark content"
  drush_ev "
    \$env_id = apachesolr_default_environment();
    module_load_include('inc', 'apachesolr', 'apachesolr.index');
    apachesolr_index_set_bundles(\$env_id, 'node', array('article', 'page'));
  " || true
  echo "Configured article and page bundles for indexing."
  terminus drush "$SITE_ENV" -- solr-mark-all

  step "Step 2: Index content"
  terminus drush "$SITE_ENV" -- solr-index

  step "Step 3: Verify index via search"
  VERIFY=$(drush_ev "
    \$results = node_search_execute('Solr');
    echo 'VERIFY_COUNT:' . count(\$results) . ' ';
  ") || true

  if echo "$VERIFY" | grep -qE "VERIFY_COUNT:[1-9]"; then
    COUNT=$(echo "$VERIFY" | grep -o 'VERIFY_COUNT:[0-9]*' | head -1 | cut -d: -f2)
    echo "Search verification: $COUNT results for 'Solr'."
  else
    echo "::error::Index verification failed: search returned 0 results after indexing"
    exit 1
  fi

elif [ "$MODULE" = "search_api_solr" ]; then

  step "Step 1: Create Search API index"
  INDEX_RESULT=$(drush_ev "
    \$index = entity_create('search_api_index', array(
      'name' => 'Site Content',
      'machine_name' => 'site_content',
      'server' => 'pantheon_solr9',
      'item_type' => 'node',
      'enabled' => 1,
      'options' => array(
        'index_directly' => 1,
        'fields' => array(
          'title' => array('type' => 'text'),
          'body:value' => array('type' => 'text'),
          'type' => array('type' => 'string'),
          'status' => array('type' => 'boolean'),
        ),
      ),
    ));
    \$index->save();
    echo \$index->machine_name ? 'INDEX_CREATED' : 'INDEX_FAILED';
  ") || true

  if echo "$INDEX_RESULT" | grep -q "INDEX_CREATED"; then
    echo "Search API index created."
  else
    echo "::error::Failed to create Search API index"
    exit 1
  fi

  step "Step 2: Index content"
  terminus drush "$SITE_ENV" -- search-api-index site_content

  step "Step 3: Verify index status"
  STATUS=$(terminus drush "$SITE_ENV" -- search-api-status site_content 2>&1)
  echo "$STATUS"

  if echo "$STATUS" | grep -q "100%"; then
    echo "Index is 100% complete."
  else
    if echo "$STATUS" | grep -qE "Indexed.*[1-9]"; then
      echo "Warning: Index not 100% but items were indexed."
    else
      echo "::error::No items indexed"
      exit 1
    fi
  fi

else
  echo "::error::Unknown module: $MODULE"
  exit 1
fi

step "CUJ 7 PASSED: $MODULE index created and content indexed"
