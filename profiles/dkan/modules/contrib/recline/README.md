# About this module

This module provides a file field which accepts csv file uploads and visualizes their
contents using Recline.js

## INSTALLATION


+ Download the Reline.js library from https://github.com/okfn/recline 
(zip file) and install in 'sites/all/libraries/'.
+ Enable recline module.

## Supported Backends and File Types

This creates grid, graph, and map data previews for CSV and XLS files based off of the following mechanisms.

It first checks to see if the DKAN Datastore module is installed, and if a datastore has been created for the file. If the datastore is available it uses that to visualize the data. This is extremely scalable since it only queries the first 50 rows of the table in the database. It has been tested with files up to 500 GB and a million+ rows.

If the datastore is not available it checks if the file is a CSV. If it is a CSV it tries to load the file into memory. If it takes longer than a second to load the file it instructs the user that the file is too large to preview. This keeps the page from freezing for larger files.

If the file is a XLS it uses the DataProxy services to preview the file since there is currently not a CSV backend for Recline. DataProxy parses the file and returns it as a data object which is previewed.

## Contributing

We are accepting issues in the dkan issue thread only -> https://github.com/NuCivic/dkan/issues -> Please label your issue as **"component: recline"** after submitting so we can identify problems and feature requests faster.

If you can, please cross reference commits in this repo to the corresponding issue in the dkan issue thread. You can do that easily adding this text:

```
NuCivic/dkan#issue_id
``` 

to any commit message or comment replacing **issue_id** with the corresponding issue id.

## Recline.js library workflow

There are clear indications on how/what to reference in the makefile itself:

```
# This should be pointing to the HEAD of the "dkan_integration" branch at the time of
# each release. The commit should have all the branches for PR's that we send against
# the okfn repo and the builded version matching the code(use ./make cat to build).
libraries[recline][download][revision] = "aa5eeac080099584792e70dff839f0e85ae7380a"
```

### Setup to work on recline.js

+ Clone recline.js repo locally
+ Add okfn as a remote
+ Fetch all tags and branches

```
git clone git@github.com:NuCivic/recline.js.git
cd recline.js
git remote add okfn git@github.com:okfn/recline.git 
git fetch --all
```

### Build a PR against okfn

All the fixes we provided should be PR's against the okfn repository. Setup for work based on the latest `okfn/master`:

```
git checkout -b "branch_name_after_fix"
git reset --hard okfn/master
```

when you are done build without minifying and push the branch to `NuCivic/recline.js`:

```
./make cat
git add .
git commit -m "Describe your commit"
git push origin branch_name_after_fix
```

Create a PR against `okfn/recline:master`

### Update dkan_integration branch to include your fix

```
git checkout dkan_integration
git merge origin branch_name_after_fix
```

### Create a QA site using this repo

+ Create a branch for this repo
+ Update the `recline.make` to reference the last commit on `NuCivic/recline.js:dkan_integration`
+ Commit the changes, push and build a QA Site
