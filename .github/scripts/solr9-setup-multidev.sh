#!/bin/bash
set -euo pipefail

# Create a Pantheon multidev with Solr 9 configured for D7 CUJ testing.
# Clones the fixture site's dev environment, sets search version to 9,
# pushes the current branch code, and installs the target search module.

MULTIDEV="$1"
TERMINUS_SITE="$2"
MODULE="${MODULE:?MODULE env var must be set (apachesolr or search_api_solr)}"

step() { echo ""; echo ">>>>>>>>>> $1 <<<<<<<<<<"; echo ""; }

step "Phase 1: Create multidev and configure Solr 9"

# Delete existing multidev if present (from a previous failed run)
if terminus multidev:list "$TERMINUS_SITE" --format=list 2>/dev/null | grep -q "^$MULTIDEV$"; then
  step "Deleting existing multidev: $MULTIDEV"
  terminus multidev:delete "$TERMINUS_SITE.$MULTIDEV" --delete-branch --yes
fi

step "Creating multidev $MULTIDEV from dev"
terminus multidev:create "$TERMINUS_SITE.dev" "$MULTIDEV"

step "Cloning Pantheon site repo"
GIT_URL=$(terminus connection:info "$TERMINUS_SITE.$MULTIDEV" --field=git_url)
GIT_SSH_COMMAND="ssh -o StrictHostKeyChecking=no" git clone "$GIT_URL" pantheon-site

cd pantheon-site
git checkout "$MULTIDEV"

step "Copying pantheon_apachesolr module"
rm -rf modules/pantheon/pantheon_apachesolr
cp -r "$GITHUB_WORKSPACE/modules/pantheon/pantheon_apachesolr" modules/pantheon/pantheon_apachesolr

step "Setting search version to 9 in pantheon.yml"
touch pantheon.yml
if grep -q "^search:" pantheon.yml; then
  sed -i "s/^\([[:space:]]*\)version: [0-9]*/\1version: 9/" pantheon.yml
else
  echo "search:" >> pantheon.yml
  echo "  version: 9" >> pantheon.yml
fi

echo "pantheon.yml contents:"
cat pantheon.yml

step "Pushing code to Pantheon"
git add -A
git commit -m "CI: Solr 9 CUJ test - $MODULE (run $GITHUB_RUN_NUMBER)" || echo "No changes to commit"
git push origin "$MULTIDEV" || echo "Nothing to push"

cd ..

step "Waiting for Pantheon workflow to complete"
terminus workflow:wait "$TERMINUS_SITE.$MULTIDEV" --max=600

SITE_ENV="$TERMINUS_SITE.$MULTIDEV"

step "Verifying environment"
terminus env:info "$SITE_ENV"

step "Checking Solr version"
SOLR_VER=$(terminus drush "$SITE_ENV" -- ev "echo \$_ENV['search_version'] ?? 'not_set';" 2>/dev/null | tr -d '[:space:]')
echo "Solr version: $SOLR_VER"
if [ "$SOLR_VER" != "9" ]; then
  echo "::error::Expected Solr version 9, got: $SOLR_VER"
  exit 1
fi

step "Enabling base modules (search, update, tag1_d7es, pantheon_apachesolr)"
terminus drush "$SITE_ENV" -- en search update tag1_d7es pantheon_apachesolr -y

step "Switching to SFTP mode"
terminus connection:set "$SITE_ENV" sftp

step "Downloading $MODULE via drush dl (Tag1 D7ES)"

if [ "$MODULE" = "apachesolr" ]; then
  terminus drush "$SITE_ENV" -- dl apachesolr-7.x-1.15 -y

  step "Enabling apachesolr and apachesolr_search"
  terminus drush "$SITE_ENV" -- en apachesolr apachesolr_search -y

elif [ "$MODULE" = "search_api_solr" ]; then
  terminus drush "$SITE_ENV" -- dl entity search_api -y
  terminus drush "$SITE_ENV" -- dl search_api_solr-7.x-1.19 -y

  step "Enabling entity, search_api, search_api_solr"
  terminus drush "$SITE_ENV" -- en entity search_api search_api_solr -y

else
  echo "::error::Unknown module: $MODULE"
  exit 1
fi

step "Committing SFTP changes and switching to git mode"
terminus env:commit "$SITE_ENV" --message="CI: Install $MODULE module" --force
terminus connection:set "$SITE_ENV" git

step "Verifying modules are enabled"
terminus drush "$SITE_ENV" -- pm-list --status=enabled --type=module | grep -iE "$MODULE|pantheon_apachesolr"

step "Phase 1 complete: Multidev $MULTIDEV ready with Solr 9 + $MODULE"
