<?php
/**
 * @file
 * Open Data Schema Map PHPUnit Tests.
 */

/**
 * Class OpenDataSchemaMapBaseTest
 */
class OpenDataSchemaMapBaseTest  extends PHPUnit_Framework_TestCase
{
  /**
   * {@inheritdoc}
   */
  public static function setUpBeforeClass() {
    // Change /data.json path to /json during tests.
    $data_json = open_data_schema_map_api_load('data_json_1_1');
    $data_json->endpoint = 'json';
    drupal_write_record('open_data_schema_map', $data_json, 'id');
    drupal_static_reset('open_data_schema_map_api_load_all');
    menu_rebuild();

    // Save original variables.
    $original_included_agency_nids = variable_get('odsm_settings_groups', array());
    variable_set('original_included_agency_nids', $original_included_agency_nids);

    // Save original filter enabled on data.json.
    variable_set('original_data_json_1_1_filter_enabled', $data_json->filter_enabled);
  }

  /**
   * {@inheritdoc}
   */
  public static function tearDownAfterClass() {
    // Restore /data.json path, filter_eenabled.
    $data_json = open_data_schema_map_api_load('data_json_1_1');
    $data_json->endpoint = 'data.json';
    $data_json->filter_enabled = variable_get('original_data_json_1_1_filter_enabled', FALSE);
    drupal_write_record('open_data_schema_map', $data_json, 'id');
    drupal_static_reset('open_data_schema_map_api_load_all');
    menu_rebuild();

    // Restore overridden variables.
    $original_included_agency_nids = variable_get('original_included_agency_nids');
    variable_set('odsm_settings_groups', $original_included_agency_nids);
    variable_del('original_included_agency_nids');
    variable_del('original_included_agency_nids');
  }

  /**
   * Test all read api methods with access control.
   */
  public function testDkanDatasetAPIRead() {
    // Get all data.json successful responses.
    $responses = $this->runQueries('data_json_1_1');
    // Get all data.json sucessful responses.
    foreach ($responses as $r) {
      // There should be only one item.
      foreach ($r->dataset as $dataset) {
        // Test if title is set.
        $this->assertTrue(isset($dataset->title));
      }
    }

    // Get all site_read successful responses.
    $responses = $this->runQueries('ckan_site_read');
    // Test specifics to site_read for every successful response.
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return');
    }

    // Get all revision_list successful responses.
    $responses = $this->runQueries('ckan_revision_list');
    // Test specifics to revision_list for every successful response.
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the IDs');
    }

    // Get all package_list successful responses.
    $responses = $this->runQueries('ckan_package_list');

    // Test specifics to package_list for every successful response.
    $uuids = array();
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the names');
      $data = drupal_json_decode($r->data);
      $uuids = $data['result'];
    }

    foreach ($uuids as $uuid) {
      // Get all package_revision_list successful responses.
      $responses = $this->runQueries('ckan_package_revision_list', $uuid);

      foreach ($responses as $r) {
        $this->runCommonTest($r, 'Return a dataset (package)');
        foreach ($r->result as $package) {
          $this->assertTrue($package->timestamp);
          $this->assertTrue($package->id);
        }
      }

      // Get all package_show successful responses.
      $responses = $this->runQueries('ckan_package_show', $uuid);
      foreach ($responses as $r) {
        $this->runCommonTest($r, 'Return the metadata of a dataset');
        $data = drupal_json_decode($r->data);
        $this->runPackageTests($data['result']);
      }
    }

    // Get all current_package_list_with_resources successful responses.
    $responses = $this->runQueries('ckan_current_package_list_with_resources');

    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the site\'s datasets');
      $data = drupal_json_decode($r->data);
      $result = isset($data['result']['result']) ? $data['result']['result'][0] : $data['result'][0];
      $this->runPackageTests($result);
    }

    // Get all group_list successful responses.
    $responses = $this->runQueries('ckan_group_list');
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the names of the site\'s groups');
      $data = drupal_json_decode($r->data);
      $result = isset($data['result']['result']) ? $data['result']['result'][0] : $data['result'][0];
      $uuids = $result;
    }

    foreach ($uuids as $uuid) {
      // Get all group_package_show successful responses.
      $responses = $this->runQueries('ckan_group_package_show', $uuid);
      foreach ($responses as $r) {
        $this->runCommonTest($r, 'Return the datasets (packages) of a group');
      }
    }
  }

  /**
   * Test filtering of API.
   */
  public function testDkanDatasetAPIFilter() {
    $api_machine_name = 'data_json_1_1';
    // Test when enable_filter is disabled.
    self::modifyApiFieldValues('data_json_1_1', array('filter_enabled' => 0));

    $query = new EntityFieldQuery();
    $num_datasets = $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'dataset')
      ->propertyCondition('status', NODE_PUBLISHED)
      ->count()->execute();

    // Load data.json responses.
    $responses = $this->runQueries($api_machine_name);
    $data = json_decode($responses[0]->data);

    // Ensure all datasets are being shown.
    $message = t(
      'No Filter Enabled: Found @num_results_datasets, expected @expected_datasets datasets',
      array(
        '@num_results_datasets' => count($data->dataset),
        '@expected_datasets' => $num_datasets,
      )
    );
    $this->assertEquals(count($data->dataset), $num_datasets, $message);

    // Test filter enabled, no groups are selected, publishers without name ok.
    self::modifyApiFieldValues($api_machine_name, array('filter_enabled' => 1));
    variable_set('odsm_settings_groups', array());
    variable_set('odsm_settings_no_publishers', 1);

    $query = new EntityFieldQuery();
    $num_datasets = $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'dataset')
      ->propertyCondition('status', NODE_PUBLISHED)
      ->count()->execute();

    // Load data.json responses.
    $responses = $this->runQueries($api_machine_name);
    $data = json_decode($responses[0]->data);

    // Ensure all datasets are being shown.
    $message = t(
      'Filter Enabled/No Groups: Found @num_results_datasets, expected @expected_datasets datasets',
      array(
        '@num_results_datasets' => count($data->dataset),
        '@expected_datasets' => $num_datasets,
      )
    );
    $this->assertEquals(count($data->dataset), $num_datasets, $message);

    // Test one group for filtering, publishers without name ok.
    variable_set('odsm_settings_no_publishers', 1);

    $query = db_query("SELECT node.title AS node_title, node.nid AS nid
        FROM {node} node
        WHERE ((node.status = '1') AND (node.type IN  ('group')) )
        ORDER BY node_title ASC"
    );
    $groups = $query->fetchAll();


    // Set the 1st group as the one to be included.
    $group_to_filter = reset($groups);
    variable_set('odsm_settings_groups', array($group_to_filter->nid => $group_to_filter->nid));

    // Load data.json responses.
    $responses = $this->runQueries($api_machine_name);
    $data = json_decode($responses[0]->data);

    foreach ($data->dataset as $dataset) {
      // Check each dataset's og_group_ref (publisher/group).
      $query = new EntityFieldQuery();
      $loaded_dataset_nids = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'dataset')
        ->propertyCondition('status', NODE_PUBLISHED)
        ->propertyCondition('uuid', $dataset->identifier)
        ->execute();
      $loaded_group_nodes = node_load_multiple(array_keys($loaded_dataset_nids['node']));

      foreach ($loaded_group_nodes as $loaded_group_node) {
        if (isset($loaded_group_node->og_group_ref[LANGUAGE_NONE])) {
          foreach ($loaded_group_node->og_group_ref[LANGUAGE_NONE] as $target_ids) {
            // Ensure the dataset is part of the group.
            $message = t(
              'Group of dataset @dataset_title did not match filter group NID @nid',
              array(
                '@dataset_title' => $group_to_filter->title,
                '@nid' => $target_ids['target_id'],
              )
            );
            $this->assertEquals($group_to_filter->nid, $target_ids['target_id'], $message);
          }
        }
        else {
          // No publisher set, no check necessary on this test.
        }
      }
    }

    // Test one group for filtering, publishers without name not ok.
    variable_set('odsm_settings_no_publishers', 0);

    $query = db_query("SELECT node.title AS node_title, node.nid AS nid
        FROM {node} node
        WHERE ((node.status = '1') AND (node.type IN  ('group')) )
        ORDER BY node_title ASC"
    );
    $groups = $query->fetchAll();

    // Set the 1st group as the one to be included.
    $group_to_filter = reset($groups);
    variable_set('odsm_settings_groups', array($group_to_filter->nid => $group_to_filter->nid));

    // Load data.json responses.
    $responses = $this->runQueries($api_machine_name);
    $data = json_decode($responses[0]->data);

    foreach ($data->dataset as $dataset) {
      // Check each dataset's og_group_ref (publisher/group).
      $query = new EntityFieldQuery();
      $loaded_dataset_nids = $query->entityCondition('entity_type', 'node')
        ->entityCondition('bundle', 'dataset')
        ->propertyCondition('status', NODE_PUBLISHED)
        ->propertyCondition('uuid', $dataset->identifier)
        ->execute();
      $loaded_group_nodes = node_load_multiple(array_keys($loaded_dataset_nids['node']));

      foreach ($loaded_group_nodes as $loaded_group_node) {
        if (isset($loaded_group_node->og_group_ref[LANGUAGE_NONE])) {
          foreach ($loaded_group_node->og_group_ref[LANGUAGE_NONE] as $target_ids) {
            // Ensure the dataset is part of the group.
            $message = t(
              'Group of dataset @dataset_title did not match filter group NID @nid',
              array(
                '@dataset_title' => $group_to_filter->title,
                '@nid' => $target_ids['target_id'],
              )
            );
            $this->assertEquals($group_to_filter->nid, $target_ids['target_id'], $message);
          }
        }
        else {
          // No publisher set, that violates the test.
          $message = t(
            'Blank publisher found on dataset @dataset_title',
            array(
              '@dataset_title' => $group_to_filter->title,
            )
          );
          $this->assertTrue(FALSE, $message);
        }
      }
    }
  }

  /**
   * Run common test to an array of package.
   *
   * @param object $result
   *   A dkan_dataset_api result object.
   * @param string $text
   *   A string to match against the returned help string.
   */
  protected function runCommonTest($result, $text) {
    $data = drupal_json_decode($result->data);
    if (isset($data['result'])  && count($data['result'])) {
      $this->assertTrue(count($data['result']) > 0);
      $this->assertTrue($data['success']);
    }
    $this->assertTrue(strpos($data['help'], $text) !== FALSE);
  }

  /**
   * Run common test to an array of package.
   *
   * @param mixed $packages
   *   An array of json datasets.
   */
  protected function runPackageTests($packages) {
    if (is_array($packages)) {
      // Loop every dataset.
      foreach ($packages as $package) {
        $this->runPackageTest($package);
      }
    }
    else {
      $this->runPackageTest($packages);
    }
  }

  /**
   * Run common test to a package item.
   *
   * @param object $package
   *   A package object.
   */
  protected function runPackageTest($package) {
    $this->assertTrue(!empty($package['metadata_created']));
    $this->assertTrue(!empty($package['metadata_modified']));
    $this->assertTrue(!empty($package['id']));
    $this->assertTrue(!empty($package['resources']));

    // Loop every resource.
    foreach ($package->resources as $resource) {
      $this->assertTrue(!empty($resource['name']));
      $this->assertTrue(!empty($resource['id']));
      $this->assertTrue(isset($resource['revision_id']));
      $this->assertTrue(!empty($resource['created']));
      // Using property exists until find correct token for this field.
      $this->assertTrue(isset($resource['state']));
    }
  }

  /**
   * Runs queries for every hook_menu_item related to $slug.
   *
   * @param string $slug
   *   identifier for a specific api endpoint
   * @param string $uuid
   *   unique identifier for a specific group, resource or dataset query
   *
   * @return array
   *   Array of successful queries.
   */
  protected function runQueries($slug, $uuid = '') {
    $uris = $this->getHookMenuItems($slug);

    foreach ($uris as $key => $uri) {
      $uris[$key] = array('uri' => $uri, 'options' => array());
      if (!empty($uuid)) {
        if (strpos($uri, '%') !== FALSE) {
          $uris[$key]['uri'] = str_replace('%', $uuid, $uri);
        }
        else {
          $uris[$key]['options'] = array('query' => array('id' => $uuid), 'absolute' => TRUE);
        }
      }
    }
    $successful = array();

    foreach ($uris as $uri) {
      $options = $uri['options'];
      $options['absolute'] = TRUE;
      if ($base_url_port = getenv('BASE_URL_PORT')) {
        global $base_url;
        $options['base_url'] = $base_url . ':' . $base_url_port;
      }
      $url = url($uri['uri'], $options);
      $result = drupal_http_request($url);
      $this->assertTrue($result->code == 200 ? TRUE : FALSE);
      $successful[] = $result;
    }

    // Return successful querys for further assertions.
    return $successful;
  }

  /**
   * Helper that gets defined hook_menu items related to a specific callback.
   *
   * @param string $callback
   *   a string representing a drupal callback.
   *
   * @return array
   *   an array of related callbacks.
   */
  protected function getHookMenuItems($callback) {
    $records = open_data_schema_map_api_load_all();
    $endpoints = array();
    foreach ($records as $record) {
      $endpoints[$record->machine_name] = $record->endpoint;
    }
    return array($endpoints[$callback]);
  }

  /**
   * Sets fields/values in an API schema.
   *
   * @param string $api_name
   *   API machine name
   * @param array $field_values
   *   Array of field_name => value to set
   */
  protected static function modifyApiFieldValues($api_name, $field_values) {
    $data_json = open_data_schema_map_api_load($api_name);
    foreach ($field_values as $field => $value) {
      $data_json->$field = $value;
    }
    drupal_write_record('open_data_schema_map', $data_json, 'id');
    drupal_static_reset('open_data_schema_map_api_load_all');
    menu_rebuild();
  }
}
