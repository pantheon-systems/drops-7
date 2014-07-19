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
