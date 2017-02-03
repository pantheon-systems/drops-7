<?php
class OpenDataSchemaMap  extends PHPUnit_Framework_TestCase
{

    public static function setUpBeforeClass()
    {
      // Change /data.json path to /json during tests.
      $data_json = open_data_schema_map_api_load('data_json_1_1');
      $data_json->endpoint = 'json';
      drupal_write_record('open_data_schema_map', $data_json, 'id');
      drupal_static_reset('open_data_schema_map_api_load_all');
      menu_rebuild();
    }

  /**
   * Test all read api methods with access control.
   */
  public function testDkanDatasetAPIRead() {
    // Get all data.json succesful responses.
    $responses = $this->runQuerys('data_json_1_1');
    // Get all data.json sucessful responses.
    foreach ($responses as $r) {
      // There should be only one item.
      foreach ($r->dataset as $dataset) {
        // Test if title is set.
        $this->assertTrue(isset($dataset->title));
      }
    }

    // Get all site_read succesful responses.
    $responses = $this->runQuerys('ckan_site_read');
    // Test specifics to site_read for every succesful response.
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return');
    }

    // Get all revision_list succesful responses.
    $responses = $this->runQuerys('ckan_revision_list');
    // Test specifics to revision_list for every succesful response.
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the IDs');
    }

    // Get all package_list succesful responses.
    $responses = $this->runQuerys('ckan_package_list');

    // Test specifics to package_list for every succesful response.
    $uuids = array();
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the names');
      $data = drupal_json_decode($r->data);
      $uuids = $data['result'];
    }

    foreach ($uuids as $uuid) {
      // Get all package_revision_list succesful responses.
      $responses = $this->runQuerys('ckan_package_revision_list', $uuid);

      foreach ($responses as $r) {
        $this->runCommonTest($r, 'Return a dataset (package)');
        foreach ($r->result as $package) {
          $this->assertTrue($package->timestamp);
          $this->assertTrue($package->id);
        }
      }

      // Get all package_show succesful responses.
      $responses = $this->runQuerys('ckan_package_show', $uuid);
      foreach ($responses as $r) {
        $this->runCommonTest($r, 'Return the metadata of a dataset');
        $data = drupal_json_decode($r->data);
        $this->runPackageTests($data['result']);
      }
    }

    // Get all current_package_list_with_resources succesful responses.
    $responses = $this->runQuerys('ckan_current_package_list_with_resources');

    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the site\'s datasets');
      $data = drupal_json_decode($r->data);
      $result = isset($data['result']['result']) ? $data['result']['result'][0] : $data['result'][0];
      $this->runPackageTests($result);
    }

    // Get all group_list succesful responses.
    $responses = $this->runQuerys('ckan_group_list');
    foreach ($responses as $r) {
      $this->runCommonTest($r, 'Return a list of the names of the site\'s groups');
      $data = drupal_json_decode($r->data);
      $result = isset($data['result']['result']) ? $data['result']['result'][0] : $data['result'][0];
      $uuids = $result;
    }

    foreach ($uuids as $uuid) {
      // Get all group_package_show succesful responses.
      $responses = $this->runQuerys('ckan_group_package_show', $uuid);
      foreach ($responses as $r) {
        $this->runCommonTest($r, 'Return the datasets (packages) of a group');
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
   * @param array $packages
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
   * Runs querys for every hook_menu_item related to $slug.
   *
   * @param string $slug
   *   identifier for a specific api endpoint
   * @param string $uuid
   *   unique identifier for a specific group, resource or dataset query
   */
  protected function runQuerys($slug, $uuid = FALSE) {
    $uris = $this->getHookMenuItems($slug);
    foreach ($uris as $key => $uri) {
      $uris[$key] = array('uri' => $uri, 'options' => array());
      if ($uuid) {
        if (strpos($uri, '%') !== FALSE) {
          $uris[$key]['uri'] = str_replace('%', $uuid, $uri);
        }
        else {
          $uris[$key]['options'] = array('query' => array('id' => $uuid), 'absolute' => TRUE);
        }
      }
    }
    $succesful = array();

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
      $succesful[] = $result;
    }

    // Return succesful querys for further assertions.
    return $succesful;
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

    /*public function testDataJsonRollback() {
      $this->rollback('dkan_migrate_base_example_data_json11');
    }*/

}
