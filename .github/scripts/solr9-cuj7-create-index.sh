#!/bin/bash
set -euo pipefail

# CUJ 7: Create and Configure a Search Index
# Tests that content can be indexed into Solr 9.
# Accepts MODULE env var: apachesolr or search_api_solr

SITE_ENV="${TERMINUS_SITE}.${MULTIDEV}"
MODULE="${MODULE:?MODULE env var must be set}"

echo "=== CUJ 7: Create and Configure Search Index ($MODULE) ==="

if [ "$MODULE" = "apachesolr" ]; then
  # Apache Solr Search: no explicit index creation needed.
  # Mark all content for reindexing and run indexer.

  echo "--- Step 1: Mark all content for reindexing ---"
  terminus drush "$SITE_ENV" -- solr-mark-all

  echo "--- Step 2: Index content ---"
  terminus drush "$SITE_ENV" -- solr-index

  echo "--- Step 3: Verify index count ---"
  INDEX_STATS=$(terminus drush "$SITE_ENV" -- ev "
    \$env_id = apachesolr_default_environment();
    \$env = apachesolr_environment_load(\$env_id);
    try {
      \$solr = apachesolr_get_solr(\$env_id);
      \$response = \$solr->getLuke();
      echo 'INDEX_COUNT:' . \$response->index->numDocs;
    } catch (Exception \$e) {
      echo 'INDEX_ERROR:' . \$e->getMessage();
    }
  " 2>&1)

  if echo "$INDEX_STATS" | grep -q "INDEX_COUNT:"; then
    COUNT=$(echo "$INDEX_STATS" | grep -o 'INDEX_COUNT:[0-9]*' | cut -d: -f2)
    if [ "$COUNT" -gt 0 ]; then
      echo "Index contains $COUNT documents."
    else
      echo "::error::Index is empty after indexing"
      exit 1
    fi
  else
    echo "::error::Failed to get index stats: $INDEX_STATS"
    exit 1
  fi

elif [ "$MODULE" = "search_api_solr" ]; then
  # Search API Solr: create index, add fields, index content.

  echo "--- Step 1: Create Search API index ---"
  INDEX_RESULT=$(terminus drush "$SITE_ENV" -- ev "
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
  " 2>&1)

  if echo "$INDEX_RESULT" | grep -q "INDEX_CREATED"; then
    echo "Search API index created."
  else
    echo "::error::Failed to create Search API index: $INDEX_RESULT"
    exit 1
  fi

  echo "--- Step 2: Index content ---"
  terminus drush "$SITE_ENV" -- search-api-index site_content

  echo "--- Step 3: Verify index status ---"
  STATUS=$(terminus drush "$SITE_ENV" -- search-api-status site_content 2>&1)
  echo "$STATUS"

  if echo "$STATUS" | grep -q "100%"; then
    echo "Index is 100% complete."
  else
    # Check if items were indexed even if not 100%
    INDEXED=$(echo "$STATUS" | grep -o 'Indexed[[:space:]]*[0-9]*' | grep -o '[0-9]*')
    if [ -n "$INDEXED" ] && [ "$INDEXED" -gt 0 ]; then
      echo "Warning: Index not 100% but $INDEXED items indexed."
    else
      echo "::error::No items indexed: $STATUS"
      exit 1
    fi
  fi

else
  echo "::error::Unknown module: $MODULE"
  exit 1
fi

echo "=== CUJ 7 PASSED: $MODULE index created and content indexed ==="
