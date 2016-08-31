**DEPRECATED BRANCH**

Please note that the master branch of this module, which was the default branch for the first two years of its life, has been abandonded in favor of the 7.x-1.x brach, as we move into a noral release cycle for it. No further commits will be made or merged to this branch. Please update any makefiles that point to it; future releases of DKAN will point to specific versions.

[![Circle CI](https://circleci.com/gh/NuCivic/open_data_schema_map.svg?style=svg)](https://circleci.com/gh/NuCivic/open_data_schema_map)

Open Data Schema Map
====================

This module provides a flexible way to expose your Drupal content via APIs following specific Open Data schemas. Currently, the [CKAN](http://docs.ckan.org/en/ckan-1.8/domain-model-dataset.html) and [Project Open Data schemas](http://project-open-data.github.io/schema/) are provided, but new schemas can be easily added through your own modules. A user interface is in place to create endpoints and map fields from the chosen schema to Drupal content using tokens.

DKAN-specific implementation: https://github.com/NuCivic/open_data_schema_map_dkan

## Basic concepts

### Schema
A schema is a list of field definitions, usually representing a community specification for presenting machine-readable data. The core Open Data Schema Map module does not include any schemas; they are provided by additional modules. A schema module includes:

* a standard Drupal .module file -- with an implementation of ```hook_open_data_schema()``` to expose the schema to the core Open Data Schema Map module, plus _alter functions for any needed modifications of the UI form or the data output itself.
* the schema itself, expressed as a .json file. For instance, see the [Project Open Data schema file](https://github.com/NuCivic/open_data_schema_map/blob/master/modules/open_data_schema_pod/data/single_entry.json) to see how these schema are defined in JSON


### API
An API in this module is a configuration set that exposes a specific set of machine-readable data at a specific URL (known as the API's endpoint). This module allows you to create multiple APIs that you save as database records and/or export using [Features](http://drupal.org/project/features). An API record will contain:

* an endpoint URL
* a schema (chosen from the available schemas provided by the additional modules as described above)
* a mapping of fields defined in that schema to Drupal tokens (usually referencing fields from a node)
* optionally, one or more arguments passed through the URL to filter the result set

## Usage

### Installation

Enable the main _Open Data Schema Map_ module as usual, and additionally enable any schema modules you will need to create your API.

### Creating APIs

Navigate to admin/config/services/odsm and click "Add API."

![screen shot 2014-07-14 at 3 24 03 pm](https://cloud.githubusercontent.com/assets/309671/3575902/c7ff24e6-0b8c-11e4-92c3-9ba2e163bf56.png)

Give the API a title, machine name, choose which entity type (usually _node_) and bundle (in [DKAN](https://github.com/NuCivic/dkan), this is usually _Dataset_).

![screen shot 2014-07-14 at 3 46 39 pm](https://cloud.githubusercontent.com/assets/309671/3576163/b3e6ea90-0b8f-11e4-9d9e-33b4515310f0.png)

You will need to create the API record before adding arguments and mappings.

### Arguments

The results of the API call can be filtered by a particular field via arguments in the URL. To add an argument, first choose the schema field then, if you are filtering by a custom field API field (ie, a field whose machine name begins with "field\_"), identify the database column that would contain the actual argument value. Leave off the field name prefix; for instance, if filtering by a DKAN tag (a term reference field), the correct column is field_tags_tid, so you would enter "tid". Which Drupal field to use will be extrapolated from the token you map to that schema field.

![Screen Shot 2014-07-14 at 3.55.49 PM.png | uploaded via ZenHub](https://cloud.githubusercontent.com/assets/512243/5281816/992d1138-7ac6-11e4-8e7b-bcaefa733648.png)

### Field Mapping

The API form presents you with a field for each field in your schema. Map the fields using Drupal's token system. Note: using more than one token in a single field may produce unexpected results and is not recommended. 

#### Multi-value fields

For Drupal multi-value entity reference fields, the schema can use an array to instruct the API to iterate over each value and map the referenced data to multiple schema fields. For instance, in the CKAN schema, tags are described like this in schema_ckan.json:

```    
      "tags": {
      "title":"Tags",
      "description":"",
      "anyOf": [
        {
          "type": "array",                    
          "items": {
            "type": "object",
            "properties": {
              "id": {
                "title": "UUID",
                "type": "string"
              },
              "vocabulary_id": {
                "title": "Vocaulary ID",
                "type": "string"
              },
              "name": {
                "title": "Name",
                "type": "string"
              },
              "revision_timestamp": {
                "title": "Revision Timestamp",
                "type": "string"
              },
              "state": {
                "title": "state",
                "description": "",
                "type": "string",
                "enum": ["uncomplete", "complete", "active"]
              }
            }
          }
        }
      ]
    },
```

You can choose which of the available multivalue fields on your selected bundle to map to the "tags" array, exposing all of the referenced "tag" entities (taxonomy terms in this example) to use as the context for your token mappings on the schema fields within that array. First, simply choose the multivalue field, leaving the individual field mappings blank, and save the form.

![screen shot 2014-07-16 at 12 14 29 am](https://cloud.githubusercontent.com/assets/309671/3594511/c3ca9cd4-0c9f-11e4-8fd0-1ea7c3c8b2b3.png)

When you return to the tags section of the form after saving, you will now see a special token navigator you can use to find tokens that will work with this iterative approach (using "Nth" in place of the standard delta value in the token):

![screen shot 2014-07-16 at 12 22 00 am](https://cloud.githubusercontent.com/assets/512243/5281826/ad5e3eac-7ac6-11e4-8c7d-91076527c84d.png)

## Customizing

### Adding new schemas

You are not limited by the schemas included with this module; any Open Data schema may be defined in a custom module. Use the open_data_schema_ckan module as a model to get started.

### Using the xml output module

We've isolated xml output into its own module. A few reasons why:

+ It relies on a composer dependency
+ This module is distributed with dkan, a drupal installation profile, and we don't have a way of installing composer dependencies while building the distro with ```drush make```
+ We don't want to force all this trouble on users that just want ***json output*** 

Because of all this, if you still want to use xml output for your odsm endpoints (we don't judge), you need to:


+ Install composer dependencies:

```
$ cd modules/open_data_schema_map_xml_output
$ composer install
```

+ Enable module

```
$ drush -y en open_data_schema_map_xml_output
```

If you need instructions to install composer globally in your system please refer to https://getcomposer.org/doc/00-intro.md#globally.

### Date format
Date formats can be chanaged manually by changing the "Medium" date time format in "admin/config/regional/date-time" or in code by using one of the alter hooks:
![screen shot 2014-09-04 at 11 15 01 am](https://cloud.githubusercontent.com/assets/512243/4152408/a9cb06b2-344e-11e4-84c8-c2174b5fc566.png)

## Drush

### odsm-filecache
#### Use:
The Open Data Schema Map module now defines a drush command called `odsm-filecache`.  This command takes as  its argument the machine name for an ODSM endpoint.  For example:

```
drush odsm-filecache data_json_1_1;
```

The above command triggers the processing for the endpoint defined for the data_json_1_1 ODSM API and results in the following cached file being generated on completion:

```
public://odsm_cache_data_json_1_1
```

The command `odsm-filecache` is a direct callback to `open_data_schema_map_file_cache_endpoint` which wraps ` open_data_schema_map_render_api` with some logic for dumping output to a file. 

In order to enable the cached version of an API endpoint you need to run the command above replacing `data_json_1_1` with
the machine  name of the ODSM endpoint to be cached.

In order to update this cache you need to re-run the command that generated it.

We recommend you set up a cron job to run the command on a regular schedule, perhaps in sync with your data harvesting schedule.

## Contributing

We are accepting issues in the dkan issue thread only -> https://github.com/NuCivic/dkan/issues -> Please label your issue as **"component: open_data_schema_map"** after submitting so we can identify problems and feature requests faster.

If you can, please cross-reference commits in this repo to the corresponding issue in the dkan issue thread. You can do that easily adding this text:

```
NuCivic/dkan#issue_id
``` 

to any commit message or comment replacing **issue_id** with the corresponding issue id.

