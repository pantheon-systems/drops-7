#!/bin/bash
set -euo pipefail

# CUJ 6: Configure a Solr Server
# Tests that the search module can connect to Solr 9 after schema posting.
# Accepts MODULE env var: apachesolr or search_api_solr

SITE_ENV="${TERMINUS_SITE}.${MULTIDEV}"
MODULE="${MODULE:?MODULE env var must be set}"

# shellcheck source=.github/scripts/solr9-shared.sh
source "$(dirname "$0")/solr9-shared.sh"

step "CUJ 6: Configure Solr Server ($MODULE)"

if [ "$MODULE" = "apachesolr" ]; then
  SCHEMA_PATH="sites/all/modules/apachesolr/solr-conf/solr-9.x/schema.xml"
elif [ "$MODULE" = "search_api_solr" ]; then
  SCHEMA_PATH="sites/all/modules/search_api_solr/solr-conf/9.x/schema.xml"
else
  echo "::error::Unknown module: $MODULE"
  exit 1
fi

step "Step 1: Post Solr 9 schema ($SCHEMA_PATH)"

MAX_RETRIES=3
RETRY_DELAY=60
for attempt in $(seq 1 $MAX_RETRIES); do
  echo "Schema post attempt $attempt of $MAX_RETRIES..."
  RESULT=$(drush_ev "
    variable_set('pantheon_apachesolr_schema', '$SCHEMA_PATH');
    \$result = pantheon_apachesolr_post_schema_exec('$SCHEMA_PATH');
    echo \$result ? 'SCHEMA_POSTED' : 'SCHEMA_FAILED';
  ") || true

  if echo "$RESULT" | grep -q "SCHEMA_POSTED"; then
    echo "Schema posted successfully."
    break
  fi

  if [ "$attempt" -lt "$MAX_RETRIES" ]; then
    echo "Schema post failed, retrying in ${RETRY_DELAY}s..."
    sleep "$RETRY_DELAY"
  else
    echo "::error::Schema post failed after $MAX_RETRIES attempts."
    exit 1
  fi
done

step "Step 2: Verify Solr ping"

PING_RESULT=$(drush_ev "
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
  echo (\$response !== FALSE && in_array(\$info['http_code'], array(200, 201, 202, 204))) ? 'PING_OK' : 'PING_FAILED:' . \$info['http_code'];
") || true

if echo "$PING_RESULT" | grep -q "PING_OK"; then
  echo "Solr ping successful."
else
  echo "::error::Solr ping failed: $PING_RESULT"
  exit 1
fi

step "Step 3: Module-specific verification"

if [ "$MODULE" = "apachesolr" ]; then
  ENV_CHECK=$(drush_ev "
    \$env_id = apachesolr_default_environment();
    if (\$env_id) {
      \$env = apachesolr_environment_load(\$env_id);
      echo 'ENV_OK:' . \$env['url'];
    } else {
      echo 'ENV_MISSING';
    }
  ") || true

  if echo "$ENV_CHECK" | grep -q "ENV_OK"; then
    echo "Apache Solr default environment configured."
  else
    echo "::error::Apache Solr default environment not found"
    exit 1
  fi

elif [ "$MODULE" = "search_api_solr" ]; then
  echo "Creating Search API server..."
  SERVER_RESULT=$(drush_ev "
    \$server = entity_create('search_api_server', array(
      'name' => 'Pantheon Solr 9',
      'machine_name' => 'pantheon_solr9',
      'class' => 'search_api_solr_service',
      'enabled' => 1,
      'description' => 'Solr 9 server on Pantheon',
      'options' => array(
        'clean_ids' => 1,
        'site_hash' => 1,
        'scheme' => 'http',
        'host' => 'localhost',
        'port' => 8983,
        'path' => '/solr',
        'http_user' => '',
        'http_pass' => '',
        'excerpt' => 0,
        'retrieve_data' => 0,
        'highlight_data' => 0,
        'skip_schema_check' => 0,
        'solr_version' => '',
        'http_method' => 'AUTO',
        'log_query' => 0,
        'log_response' => 0,
        'commits_disabled' => 0,
      ),
    ));
    \$server->save();
    echo \$server->machine_name ? 'SERVER_CREATED' : 'SERVER_FAILED';
  ") || true

  if echo "$SERVER_RESULT" | grep -q "SERVER_CREATED"; then
    echo "Search API server created."
  else
    echo "::error::Failed to create Search API server"
    exit 1
  fi

  STATUS=$(drush_ev "
    \$server = search_api_server_load('pantheon_solr9');
    echo (\$server && \$server->ping()) ? 'SERVER_CONNECTED' : 'SERVER_DISCONNECTED';
  ") || true

  if echo "$STATUS" | grep -q "SERVER_CONNECTED"; then
    echo "Search API server connected."
  else
    echo "::error::Search API server not connected"
    exit 1
  fi
fi

step "CUJ 6 PASSED: $MODULE configured and connected to Solr 9"
