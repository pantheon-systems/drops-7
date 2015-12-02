<?php
/**
 * @file
 *   settings file for testing search
 *
 * The Google Appliance module does not cache search results in the database, rather search results
 * are presented via HTTP GET requests that are dependent upon your "Connection Information"
 * settings on the module administration screen. So, we cannot automate the validation
 * of the logic that displays module output (error messages or search results), without
 * knowing in advance (at test time) what the connection information is.
 *
 * Since there is no public GSA available for general testing, you must provide a test settings
 * file that will allow the testing framework to query your Google Search Appliance device.
 *
 * Create a file named 'test-settings.inc' and place it alongside this example file in the
 * testing directory with actual information that will produce search results. Your file must use
 * the var name $file_spec in order for this to work. If the file is not found during tests, the
 * associated tests will throw an exception.
 */

$file_spec = array(
  'hostname' => '',   // hostname of your GSA device
  'frontend' => '',   // frontend client to run the tests against
  'collection' => '', // collection to run the tests against

  'query' => ''       // query to use in the search tests
                      // be sure to use a query that will produce at least 30 or so results
                      // so that the pager appears and is tested.
);
