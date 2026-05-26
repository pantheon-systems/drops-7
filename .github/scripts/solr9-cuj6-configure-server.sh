#!/bin/bash
set -euo pipefail

# CUJ 6: Configure a Solr Server
# Tests that the search module can connect to Solr 9 after schema posting.
# Accepts MODULE env var: apachesolr or search_api_solr

SITE_ENV="${TERMINUS_SITE}.${MULTIDEV}"
MODULE="${MODULE:?MODULE env var must be set}"

step() { echo ""; echo ">>>>>>>>>> $1 <<<<<<<<<<"; echo ""; }

# Helper: run drush ev, suppress terminus stderr noise, return clean stdout
drush_ev() {
  terminus drush "$SITE_ENV" -- ev "$@" 2>/dev/null | tr -d '[:space:]'
}

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
  ")

  if [ "$RESULT" = "SCHEMA_POSTED" ]; then
    echo "Schema posted successfully."
    break
  fi

  if [ "$attempt" -lt "$MAX_RETRIES" ]; then
    echo "Schema post failed, retrying in ${RETRY_DELAY}s..."
    sleep "$RETRY_DELAY"
  else
    echo "::error::Schema post failed after $MAX_RETRIES attempts. Result: $RESULT"
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
")

if [ "$PING_RESULT" = "PING_OK" ]; then
  echo "Solr ping successful."
else
  echo "::error::Solr ping failed: $PING_RESULT"
  exit 1
fi

step "Step 3: Module-specific verification"

# Get Solr version for reporting
SOLR_VER=$(drush_ev "echo \$_ENV['search_version'] ?? 'unknown';")

if [ "$MODULE" = "apachesolr" ]; then
  ENV_URL=$(drush_ev "
    \$env_id = apachesolr_default_environment();
    if (\$env_id) {
      \$env = apachesolr_environment_load(\$env_id);
      echo \$env['url'];
    } else {
      echo 'ENV_MISSING';
    }
  ")

  if [ "$ENV_URL" != "ENV_MISSING" ] && [ -n "$ENV_URL" ]; then
    echo "Apache Solr default environment configured (Solr $SOLR_VER)."
    echo "URL: $ENV_URL"
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
      'options' => array('clean_ids' => TRUE),
    ));
    \$server->save();
    echo \$server->machine_name ? 'SERVER_CREATED' : 'SERVER_FAILED';
  ")

  if [ "$SERVER_RESULT" = "SERVER_CREATED" ]; then
    echo "Search API server created."
  else
    echo "::error::Failed to create Search API server: $SERVER_RESULT"
    exit 1
  fi

  STATUS=$(drush_ev "
    \$server = search_api_server_load('pantheon_solr9');
    echo (\$server && \$server->ping()) ? 'SERVER_CONNECTED' : 'SERVER_DISCONNECTED';
  ")

  if [ "$STATUS" = "SERVER_CONNECTED" ]; then
    echo "Search API server connected (Solr $SOLR_VER)."
  else
    echo "::error::Search API server not connected: $STATUS"
    exit 1
  fi
fi

step "CUJ 6 PASSED: $MODULE configured and connected to Solr $SOLR_VER"
