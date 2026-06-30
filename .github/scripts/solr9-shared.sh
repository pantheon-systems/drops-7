#!/bin/bash
# Shared helpers for solr9 CUJ scripts. Sourced, not executed.
# No set flags here: they would leak into the sourcing script's shell.

step() { echo ""; echo ">>>>>>>>>> $1 <<<<<<<<<<"; echo ""; }

# Run "$@" up to RETRY_MAX times with exponential backoff (RETRY_DELAY base),
# returning on first success. The command's stdout+stderr go to this function's
# stdout so $(...) callers still capture output; retry diagnostics go to stderr.
retry() {
  local max="${RETRY_MAX:-3}" delay="${RETRY_DELAY:-5}" attempt=1 rc=0
  while true; do
    if "$@" 2>&1; then
      return 0
    fi
    rc=$?
    if [ "$attempt" -ge "$max" ]; then
      echo "retry: '$*' failed after $attempt attempts (last exit $rc)" >&2
      return "$rc"
    fi
    echo "retry: '$*' exit $rc (attempt $attempt/$max), retrying in ${delay}s" >&2
    sleep "$delay"
    attempt=$((attempt + 1))
    delay=$((delay * 2))
  done
}

drush_ev() {
  retry terminus drush "$SITE_ENV" -- ev "$@"
}
