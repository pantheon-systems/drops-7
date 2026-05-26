#!/bin/bash
set -euo pipefail

# CUJ 8: Validate That Solr 9 is Serving Search Results
# Tests that indexed content is searchable and results are correct.
# Accepts MODULE env var: apachesolr or search_api_solr

SITE_ENV="${TERMINUS_SITE}.${MULTIDEV}"
MODULE="${MODULE:?MODULE env var must be set}"

step() { echo ""; echo ">>>>>>>>>> $1 <<<<<<<<<<"; echo ""; }

drush_ev_extract() {
  local pattern="$1"
  shift
  terminus drush "$SITE_ENV" -- ev "$@" 2>&1 | grep -o "${pattern}[^ ]*" || true
}

step "CUJ 8: Validate Search Results ($MODULE)"

step "Step 1: Search for known term"

if [ "$MODULE" = "apachesolr" ]; then
  SEARCH_RESULT=$(drush_ev_extract "FOUND:" "
    \$env_id = apachesolr_default_environment();
    \$solr = apachesolr_get_solr(\$env_id);
    \$response = \$solr->search('content:Solr', array('rows' => 10));
    echo 'FOUND:' . \$response->response->numFound;
  ")
elif [ "$MODULE" = "search_api_solr" ]; then
  SEARCH_RESULT=$(drush_ev_extract "FOUND:" "
    \$index = search_api_index_load('site_content');
    \$query = new SearchApiQuery(\$index);
    \$query->keys('Solr');
    \$query->range(0, 10);
    \$results = \$query->execute();
    echo 'FOUND:' . \$results['result count'];
  ")
fi

COUNT="${SEARCH_RESULT#FOUND:}"
if [ -n "$COUNT" ] && [ "$COUNT" -gt 0 ] 2>/dev/null; then
  echo "Search for 'Solr' returned $COUNT results."
else
  echo "::error::Search for 'Solr' returned 0 results (expected > 0). Raw: $SEARCH_RESULT"
  exit 1
fi

step "Step 2: Search for non-existent term"

if [ "$MODULE" = "apachesolr" ]; then
  EMPTY_RESULT=$(drush_ev_extract "FOUND:" "
    \$env_id = apachesolr_default_environment();
    \$solr = apachesolr_get_solr(\$env_id);
    \$response = \$solr->search('content:xyznonexistent99', array('rows' => 1));
    echo 'FOUND:' . \$response->response->numFound;
  ")
elif [ "$MODULE" = "search_api_solr" ]; then
  EMPTY_RESULT=$(drush_ev_extract "FOUND:" "
    \$index = search_api_index_load('site_content');
    \$query = new SearchApiQuery(\$index);
    \$query->keys('xyznonexistent99');
    \$query->range(0, 1);
    \$results = \$query->execute();
    echo 'FOUND:' . \$results['result count'];
  ")
fi

if [ "$EMPTY_RESULT" = "FOUND:0" ]; then
  echo "Search for non-existent term correctly returned 0 results."
else
  echo "::warning::Expected FOUND:0 for non-existent term, got: $EMPTY_RESULT"
fi

step "Step 3: Check watchdog for Solr errors"

if [ "$MODULE" = "apachesolr" ]; then
  WD_TYPE="Apache Solr"
else
  WD_TYPE="search_api_solr"
fi

ERRORS=$(terminus drush "$SITE_ENV" -- watchdog-show "--type=$WD_TYPE" --count=10 2>&1) || true

if echo "$ERRORS" | grep -qiE "Unrecognized message type|No log messages"; then
  echo "No Solr log entries in watchdog (type '$WD_TYPE' not present)."
else
  REAL_ERRORS=$(echo "$ERRORS" | grep -i "error" | grep -vi "Unrecognized message type\|Exit:\|Command:" || true)
  if [ -n "$REAL_ERRORS" ]; then
    echo "::warning::Solr errors found in watchdog:"
    echo "$REAL_ERRORS"
  else
    echo "No Solr errors in watchdog."
  fi
fi

step "Step 4: Verify Solr ping"

PING=$(drush_ev_extract "PING_" "
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
  echo in_array(\$info['http_code'], array(200, 201, 202, 204)) ? 'PING_OK' : 'PING_FAILED:' . \$info['http_code'];
")

if [ "$PING" = "PING_OK" ]; then
  echo "Solr ping healthy."
else
  echo "::error::Solr ping failed: $PING"
  exit 1
fi

step "CUJ 8 PASSED: $MODULE search results validated"
