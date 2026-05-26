#!/bin/bash
set -euo pipefail

# CUJ 7: Create and Configure a Search Index
# Tests that content can be indexed into Solr 9.
# Accepts MODULE env var: apachesolr or search_api_solr

SITE_ENV="${TERMINUS_SITE}.${MULTIDEV}"
MODULE="${MODULE:?MODULE env var must be set}"

step() { echo ""; echo ">>>>>>>>>> $1 <<<<<<<<<<"; echo ""; }

drush_ev() {
  terminus drush "$SITE_ENV" -- ev "$@" 2>/dev/null | tail -1 | tr -d '[:space:]'
}

step "CUJ 7: Create and Configure Search Index ($MODULE)"

if [ "$MODULE" = "apachesolr" ]; then

  step "Step 1: Configure indexing and mark content"
  # apachesolr needs entity types and bundles configured before indexing
  drush_ev "
    \$env_id = apachesolr_default_environment();
    module_load_include('inc', 'apachesolr', 'apachesolr.index');
    apachesolr_index_set_bundles(\$env_id, 'node', array('article', 'page'));
  "
  echo "Configured article and page bundles for indexing."
  terminus drush "$SITE_ENV" -- solr-mark-all

  step "Step 2: Index content"
  terminus drush "$SITE_ENV" -- solr-index

  step "Step 3: Verify index count"
  COUNT=$(drush_ev "
    \$env_id = apachesolr_default_environment();
    try {
      \$solr = apachesolr_get_solr(\$env_id);
      \$response = \$solr->getLuke();
      echo \$response->index->numDocs;
    } catch (Exception \$e) {
      echo '0';
    }
  ")

  if [ -n "$COUNT" ] && [ "$COUNT" -gt 0 ] 2>/dev/null; then
    echo "Index contains $COUNT documents."
  else
    echo "::error::Index is empty or count failed: $COUNT"
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
  ")

  if [ "$INDEX_RESULT" = "INDEX_CREATED" ]; then
    echo "Search API index created."
  else
    echo "::error::Failed to create Search API index: $INDEX_RESULT"
    exit 1
  fi

  step "Step 2: Index content"
  terminus drush "$SITE_ENV" -- search-api-index site_content

  step "Step 3: Verify index status"
  terminus drush "$SITE_ENV" -- search-api-status site_content

  INDEXED=$(drush_ev "
    \$index = search_api_index_load('site_content');
    \$status = search_api_index_status(\$index);
    echo \$status['indexed'];
  ")

  if [ -n "$INDEXED" ] && [ "$INDEXED" -gt 0 ] 2>/dev/null; then
    echo "Indexed $INDEXED items."
  else
    echo "::error::No items indexed: $INDEXED"
    exit 1
  fi

else
  echo "::error::Unknown module: $MODULE"
  exit 1
fi

step "CUJ 7 PASSED: $MODULE index created and content indexed"
