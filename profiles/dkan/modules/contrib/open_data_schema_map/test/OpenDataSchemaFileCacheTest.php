<?php

/**
 * Test for file cache functionality.
 */
class OpenDataSchemaFileCacheTest extends PHPUnit_Framework_TestCase {

  /**
   * Remove cache files if found.
   */
  protected function tearDown() {
    $api = open_data_schema_map_api_load('data_json_1_1');
    open_data_schema_map_file_cache_delete($api);
  }

  /**
   * Test open_data_schema_map_file_cache_admin_link_label().
   */
  public function testFileCacheAdminLinkLabel() {
    // Defaults to none.
    $api = open_data_schema_map_api_load('data_json_1_1');
    $expected = "none";
    $actual = open_data_schema_map_file_cache_admin_link_label($api);
    $this->assertEquals($actual, $expected);

    // Otherwise defaults to age of cache.
    open_data_schema_map_file_cache_create($api);
    $expected = "0 hrs";
    $actual = open_data_schema_map_file_cache_admin_link_label($api);
    $this->assertEquals($actual, $expected);
  }

  /**
   * Test open_data_schema_map_file_cache_admin_link().
   */
  public function testFileCacheAdminLink() {
    $api = open_data_schema_map_api_load('data_json_1_1');
    $label = open_data_schema_map_file_cache_admin_link_label($api);
    $expected = l($label, OPEN_DATA_SCHEMA_MAP_ADMIN_PATH . '/cache/' . $api->machine_name);
    $actual = open_data_schema_map_file_cache_admin_link($api);
    $this->assertEquals($actual, $expected);
  }

  /**
   * Test open_data_schema_map_file_cache_create().
   */
  public function testFileCacheCreate() {
    $api = open_data_schema_map_api_load('data_json_1_1');
    $this->assertFalse(open_data_schema_map_file_cache_exits($api));
    open_data_schema_map_file_cache_create($api);
    $this->assertTrue(open_data_schema_map_file_cache_exits($api));
  }

  /**
   * Test open_data_schema_map_file_cache_delete().
   */
  public function testFileCacheDelete() {
    $api = open_data_schema_map_api_load('data_json_1_1');
    open_data_schema_map_file_cache_create($api);
    $this->assertTrue(open_data_schema_map_file_cache_exits($api));

    open_data_schema_map_file_cache_delete($api);
    $this->assertFalse(open_data_schema_map_file_cache_exits($api));
  }

}
