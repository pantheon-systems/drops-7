[![Build Status](https://travis-ci.org/NuCivic/dkan_datastore.png?branch=7.x-1.x)](https://travis-ci.org/NuCivic/dkan_datastore)

# DKAN Datastore

DKAN Datastore bundles a number of modules and configuration to allow users to upload CSV files, parse them and save them into the native database as flat tables, and query them through a public API.

DKAN Datastore is part of the DKAN distribution which makes it easy to create an Open Data Portal.

DKAN Datastore is part of the [DKAN](https://drupal.org/project/dkan "DKAN homepage") project which includes the [DKAN profile](https://drupal.org/project/dkan "DKAN homepage") which creates a standalone Open Data portal, and [DKAN Dataset](https://drupal.org/project/dkan_dataset "DKAN Datastore homepage").

DKAN Datastore is currently managed in code on Github but is mirrored on Drupal.org.

## INSTALLATION

This module REQUIRES implementers to use "drush make". If you only use "drush download" you will miss key dependencies for required modules and libraries.

The following will download the required libraries and patched modules:

```bash
drush dl dkan_datastore
cd dkan_datastore
drush make --no-core dkan_datastore.make
```

## Contributing

We are accepting issues in the dkan issue thread only -> https://github.com/NuCivic/dkan/issues -> Please label your issue as **"component: dkan_datastore"** after submitting so we can identify problems and feature requests faster.

If you can, please cross reference commits in this repo to the corresponding issue in the dkan issue thread. You can do that easily adding this text:

```
NuCivic/dkan#issue_id
``` 

to any commit message or comment replacing **issue_id** with the corresponding issue id.
