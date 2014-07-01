[![Build Status](https://travis-ci.org/nuams/dkan-drops-7.png?branch=master)](https://travis-ci.org/nuams/dkan-drops-7)

# Dkan 7 for DROPs (Pantheon)

Fork based on [Pantheon DROPs](https://github.com/pantheon-systems/drops-7)

## How to update Drupal Drop from Pantheon Git Repository

Just for the first time, add the pantheon base drop repo as a remote
```bash
git remote add pantheon https://github.com/pantheon-systems/drops-7.git
```

Any time you want to integrate their changes into this repo

```bash
# Make a branch so we can test if their work pass our testing
git checkout -b updating_from_pantheon_drops
# Pull their master into your branch (solve conflicts if any)
git pull --rebase pantheon master:updating_from_pantheon_drops
# Push changes and wait for travis to run the build on the 'updating_from_pantheon_drops' branch.  
git push origin updating_from_pantheon_drops
```

Fix any issues with the build (if any) pushing commits. When everything is ok squash all your fix commits into one. Then:

```bash
# Checkout master
git checkout master
# Rebase changes from your branch
git rebase updating_from_pantheon_drops
# Push
git push origin master
# Delete integration branch
git push origin :updating_from_pantheon_drops
```

## How to update dkan profile

```bash
# Make a branch so we can test the travis build sep
git checkout -b rebuilding_dkan_profile
# Run dkan update script
cd scripts
./rebuild-dkan.sh
# Add, Commit, Push and check the travis build for the 'rebuilding_dkan_profile' branch
git add ../profiles/dkan -A
git commit -m "Rebuilding dkan"
git push origin rebuilding_dkan_profile
```

Fix any issues with the build (if any) pushing commits. When everything is ok squash all your fix commits into one. Then:

```bash
# Checkout master
git checkout master
# Rebase changes from your branch
git rebase rebuilding_dkan_profile
# Push
git push origin master
# Delete integration branch
git push origin :rebuilding_dkan_profile
```

