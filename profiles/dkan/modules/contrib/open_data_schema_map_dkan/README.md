Open Data Schema Map DKAN
=========================

**NOTICE:** This module has been moved into [DKAN core](https://github.com/NuCivic/dkan) for release 7.x-1.13. To maintain backward compatibility with DKAN 7.x-1.12 and subsequent patch releases this project will remain on Github but new features will be applied directly to the DKAN core folder `modules/dkan/open_data_schema_map_dkan`.

Default [Open Data Schema Map](https://github.com/NuCivic/open_data_schema_map) endpoints for DKAN. Includes CKAN and Project Open Data endpoints.

## Project Open Data

Provides default mappings between DKAN and POD's [data.json](https://project-open-data.cio.gov/v1.1/schema/).

### Notes

* The ["name" field on data.json's "publisher" object](https://project-open-data.cio.gov/v1.1/schema/#publisher) maps to a dataset's group (see [Organic Groups](https://www.drupal.org/project/og)) in DKAN. Note that while it is possible to assign a dataset to multiple groups in DKAN, data.json only allows for a single publisher. If a dataset belongs to multiple groups, only the first group will be exposed as the "publisher" in data.json

## CKAN

Provides endpoints for publishing via the [CKAN API](http://docs.ckan.org/en/latest/api/)

* ckan_package_show
* ckan_current_package_list_with_resources
* ckan_group_list
* ckan_group_package_show
* ckan_package_list
* ckan_package_show
