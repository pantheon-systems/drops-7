# Taxonomy fixtures

This drupal module provides an "unfeaturized" way of exporting/importing taxonomy terms to/from a json file.

## Fixtures?

Yes. As in **sample data** or **initial data**.

## "Unfeaturized"?

It just doesn't use the [features](https://www.drupal.org/project/features) module at all. Using the features module for **content** is evil.

## Tags are content?

Well, kind of. Tags are to be referenced by content. Which means you need them available to be referenced and you may run into the scenario where you: 

+ Need to add a couple of tags to a vocabulary
+ Need A way to deploy these to multiple environments without the need to manually create them on a production instance.
+ Need to quickly deploy a set of tags for use in CI Tests or QA Sites.
+ Want to easily create a few tags when a installing a Drupal Profile.

## Ok, I'm in. How do I setup to use it?

In order to begin importing and exporting content you need to provide a simple mapping. You do that implementing **hook_taxonomy_fixtures_vocabulary()** for your module. Let's look at how DKAN begin to use it to bootstrap "Topics":

```php
/**
 * Implements hook_taxonomy_fixtures_vocabulary().
 */
function dkan_default_topics_taxonomy_fixtures_vocabulary() {
  $data_path = drupal_get_path('module', 'dkan_default_topics') . '/data';
  $vocabularies = array();

  $vocabularies['dkan_topics'] = array(
    'export_path' => $data_path,
    'map' =>     array(
      'name' => 'name',
      'field_icon_type' => 'type',
      'field_topic_icon' => 'icon',
      'field_topic_icon_color' => 'icon_color',
      'description' => 'description',
      'weight' => 'weight',
      'uuid' => 'uuid',
    ),
  ); 

  return $vocabularies;
}
```

This hook returns:

+ A map of drupal fields to JSON fields
+ Where to export the JSON field (and eventually where to import it from)
+ Everything tied together in an associative array which keys are the exported vocabularies machine names


### Drupal to JSON Mapping

```php
'map' => array(
  'name' => 'name',
  'field_icon_type' => 'type',
  'field_topic_icon' => 'icon',
  'field_topic_icon_color' => 'icon_color',
  'description' => 'description',
  'weight' => 'weight',
  'uuid' => 'uuid',
),
```

The array keys are the drupal fields names. Values are the JSON Fields names.

### Export/import location

Where do you want the JSON Export to live. In this case a **data** directory inside the **dkan_default_topics** module directory (wherever that is)

```php
'export_path' => drupal_get_path('module', 'dkan_default_topics') . '/data',
```

## How do i know if this is setup correctly?

Running the following will output all the subscribed vocabularies:

```
drush tf list
```

If your vocabulary is there you are good to go.


## Ok, i'm ready. How do i export and import data using this?

There are a drush commands to accomplish all this.

### Export Data

In order to export all the subscribed vocabularies you run:

```bash
$ drush tf export
```

If you want to export just the `dkan_topics` vocabulary:

```bash
$ drush tf export dkan_topics
```

### Import Data

To import all the exported vocabularies

```bash
$ drush tf import
```

To import just the `dkan_topics` vocabulary

```bash
$ drush tf import dkan_topics
```
