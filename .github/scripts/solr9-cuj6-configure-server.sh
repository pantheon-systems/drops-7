#!/bin/bash
set -euo pipefail

# CUJ 6: Configure a Solr Server
# Tests that the search module can connect to Solr 9 after schema posting.
# Accepts MODULE env var: apachesolr or search_api_solr

SITE_ENV="${TERMINUS_SITE}.${MULTIDEV}"
MODULE="${MODULE:?MODULE env var must be set}"

echo "=== CUJ 6: Configure Solr Server ($MODULE) ==="

# Step 1: Post Solr 9 schema
echo "--- Step 1: Post Solr 9 schema ---"

if [ "$MODULE" = "apachesolr" ]; then
  SCHEMA_PATH="sites/all/modules/apachesolr/solr-conf/solr-9.x/schema.xml"
elif [ "$MODULE" = "search_api_solr" ]; then
  SCHEMA_PATH="sites/all/modules/search_api_solr/solr-conf/9.x/schema.xml"
else
  echo "::error::Unknown module: $MODULE"
  exit 1
fi

echo "Posting schema: $SCHEMA_PATH"

# Post schema with retry (Solr endpoint can return 502 transiently)
MAX_RETRIES=3
RETRY_DELAY=60
for attempt in $(seq 1 $MAX_RETRIES); do
  echo "Schema post attempt $attempt of $MAX_RETRIES..."
  RESULT=$(terminus drush "$SITE_ENV" -- ev "
    variable_set('pantheon_apachesolr_schema', '$SCHEMA_PATH');
    \$result = pantheon_apachesolr_post_schema_exec('$SCHEMA_PATH');
    echo \$result ? 'SCHEMA_POSTED' : 'SCHEMA_FAILED';
  " 2>&1) || true

  if echo "$RESULT" | grep -q "SCHEMA_POSTED"; then
    echo "Schema posted successfully."
    break
  fi

  if [ "$attempt" -lt "$MAX_RETRIES" ]; then
    echo "Schema post failed, retrying in ${RETRY_DELAY}s..."
    sleep "$RETRY_DELAY"
  else
    echo "::error::Schema post failed after $MAX_RETRIES attempts"
    echo "Last output: $RESULT"
    exit 1
  fi
done

# Step 2: Verify ping
echo "--- Step 2: Verify Solr ping ---"

PING_RESULT=$(terminus drush "$SITE_ENV" -- ev "
  \$host = PANTHEON_APACHESOLR_HOST;
  \$path = getenv('PANTHEON_INDEX_PATH') . getenv('PANTHEON_INDEX_CORE') . '/admin/ping';
  if ('$MODULE' === 'search_api_solr') {
    \$path .= '?q=id:1';
  }
  \$url = 'https://' . \$host . '/' . \$path;
  list(\$ch, \$opts) = pantheon_apachesolr_curl_setup(\$url, PANTHEON_APACHESOLR_PORT);
  \$opts = pantheon_apachesolr_curlopts(\$opts);
  \$opts[CURLOPT_CONNECTTIMEOUT] = 5;
  \$opts[CURLOPT_RETURNTRANSFER] = 1;
  curl_setopt_array(\$ch, \$opts);
  \$response = curl_exec(\$ch);
  \$info = curl_getinfo(\$ch);
  curl_close(\$ch);
  if (\$response !== FALSE && in_array(\$info['http_code'], array(200, 201, 202, 204))) {
    echo 'PING_OK';
  } else {
    echo 'PING_FAILED:' . \$info['http_code'];
  }
" 2>&1)

if echo "$PING_RESULT" | grep -q "PING_OK"; then
  echo "Solr ping successful."
else
  echo "::error::Solr ping failed: $PING_RESULT"
  exit 1
fi

# Step 3: Module-specific server verification
echo "--- Step 3: Module-specific verification ---"

if [ "$MODULE" = "apachesolr" ]; then
  # Apache Solr Search auto-configures. Verify default environment exists.
  ENV_CHECK=$(terminus drush "$SITE_ENV" -- ev "
    \$env = apachesolr_default_environment();
    echo \$env ? 'ENV_OK:' . \$env['url'] : 'ENV_MISSING';
  " 2>&1)

  if echo "$ENV_CHECK" | grep -q "ENV_OK"; then
    echo "Apache Solr default environment configured: $ENV_CHECK"
  else
    echo "::error::Apache Solr default environment not found: $ENV_CHECK"
    exit 1
  fi

elif [ "$MODULE" = "search_api_solr" ]; then
  # Search API Solr needs a server created programmatically
  echo "Creating Search API server..."
  SERVER_RESULT=$(terminus drush "$SITE_ENV" -- ev "
    \$server = entity_create('search_api_server', array(
      'name' => 'Pantheon Solr 9',
      'machine_name' => 'pantheon_solr9',
      'class' => 'PantheonApachesolrSearchApiSolrService',
      'enabled' => 1,
      'description' => 'Solr 9 server on Pantheon',
      'options' => array('clean_ids' => TRUE),
    ));
    \$server->save();
    echo \$server->machine_name ? 'SERVER_CREATED' : 'SERVER_FAILED';
  " 2>&1)

  if echo "$SERVER_RESULT" | grep -q "SERVER_CREATED"; then
    echo "Search API server created."
  else
    echo "::error::Failed to create Search API server: $SERVER_RESULT"
    exit 1
  fi

  # Verify server status
  STATUS=$(terminus drush "$SITE_ENV" -- ev "
    \$server = search_api_server_load('pantheon_solr9');
    if (\$server && \$server->ping()) {
      echo 'SERVER_CONNECTED';
    } else {
      echo 'SERVER_DISCONNECTED';
    }
  " 2>&1)

  if echo "$STATUS" | grep -q "SERVER_CONNECTED"; then
    echo "Search API server connected to Solr 9."
  else
    echo "::error::Search API server not connected: $STATUS"
    exit 1
  fi
fi

echo "=== CUJ 6 PASSED: $MODULE configured and connected to Solr 9 ==="
