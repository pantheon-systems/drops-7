<?php

/**
 * @file
 * Recline theme functions.
 */

/**
 * File size limit for remote zip files preview.
 *
 * This is an arbitrary number 600000 = 600kb
 * If you need to increase this consider that
 * could slow down preview pages loading.
 */
define('MAX_SIZE_PREVIEW_ZIP', 600000);

/**
 * Timeout limit for HEAD HTTP requests.
 */
define('RECLINE_REQUEST_TIMEOUT_HEAD', 2);

/**
 * Timeout limit for GET HTTP requests.
 */
define('RECLINE_REQUEST_TIMEOUT_GET', 10);


/**
 * Returns HTML for an recline field formatter.
 *
 * @param array $variables
 *   An associative array containing:
 *   - item: Associative array of recline field
 *
 * @ingroup themeable
 */
function theme_recline_default_formatter($vars) {
  $type = null;

  // Embedded view doesn't use html.tpl.php template
  // so we need to add bootstrap by hand.
  if (recline_embedded()) {
    $recline_path = libraries_get_path('recline');
    drupal_add_js($recline_path . '/vendor/bootstrap/3.2.0/js/bootstrap.js');
    drupal_add_css($recline_path . '/vendor/bootstrap/3.2.0/css/bootstrap.css');
  }

  if (isset($vars['item']['entity']->field_format['und'][0]['tid'])) {
    $format = taxonomy_term_load($vars['item']['entity']->field_format['und'][0]['tid']);
    $type = $format->name;
  }
  $default_file = array(
    'filemime' => $type,
    'filesize' => '',
    'filename' => '',
    'description' => '',
  );

  $file = isset($vars['item']['fid']) ? file_load($vars['item']['fid']) : $default_file;
  if (is_object($file)) {
    $file = (array) $file;
  }

  if (isset($vars['item']['url'])) {
    $url = $vars['item']['url'];
  }
  elseif (isset($vars['item']['uri'])) {
    $url = file_create_url($vars['item']['uri']);
  }
  elseif (isset($vars['item']['fid'])) {
    $url = file_create_url($file['uri']);
  }

  $file['filename'] = isset($file['filename']) ? $file['filename'] : $url;
  $description = isset($file['description']) ? $file['description'] : '';
  $output = recline_build_icon($url, $type, $file['filename'], $file['filemime'], $file['filesize'], $description);

  if(!$type) {
    $request = drupal_http_request($url, array('timeout' => RECLINE_REQUEST_TIMEOUT_HEAD, 'method' => 'HEAD'));
    // When request fail.
    if ($request->code != 200) {

      // When a request fail because anything but a 405 (method not allowed)
      // or a 400 (bad request) then display a preview unavailable message.
      // The 405 error could happen when request something with the HEAD method.
      // The HEAD method it's useful when you want to get the remote file
      // headers without transfer anything.
      if ($request->code != 405 && $request->code != 400) {
        $output['preview'] = recline_preview_unavailable();
        return drupal_render($output);
      }
      else {

        // If request fail b/c either a 405 or 400 error then we perform
        // a new request using GET but setting a timeout to 10 to avoid
        // this request get blocked because a big file download.
        $request = drupal_http_request($url, array('timeout' => RECLINE_REQUEST_TIMEOUT_GET));
        if ($request->code != 200) {
          $output['preview'] = recline_preview_unavailable();
          return drupal_render($output);
        }
      }
    }
  }
  switch ($type) {
    case 'json':
      $output['preview'] = recline_preview_json($url);
      break;
    case 'csv':
    case 'xls':
      $output['preview'] = recline_preview_multiview($vars);
      break;
    case 'geojson':
      $output['preview'] = recline_preview_geojson_leaflet($url);
      break;
    case 'zip':
      $filename = FALSE;

      if (substr($vars['item']['uri'], 0, 4) != 'http') {
        $filename = $vars['item']['uri'];
      }
      else {
        // Make sure we have the information about the remote file.
        if (!isset($request)) {
          // TODO we probably want to improve this by using the
          // GetRemoteFileInfo helper class from dkan_dataset module.
          $request = drupal_http_request($url, array('timeout' => RECLINE_REQUEST_TIMEOUT_HEAD, 'method' => 'HEAD'));
        }

        // Make sure that the request was successful and the target zip file
        // does not excced the maximum allowed for previews.
        if (isset($request->headers['content-length'])
          && $request->headers['content-length'] < MAX_SIZE_PREVIEW_ZIP) {
          $ch = curl_init($url);
          $filename = file_directory_temp() . "/tmp-" . rand(100000, 999999) . ".zip";
          $fp = fopen($filename, "w");
          $ch = curl_init($url);
          $filename = file_directory_temp() . '/tmp-' . rand(100000, 999999) . '.zip';
          $fp = fopen($filename, 'w');

          curl_setopt($ch, CURLOPT_FILE, $fp);
          curl_setopt($ch, CURLOPT_HEADER, 0);

          curl_exec($ch);
          curl_close($ch);
          fclose($fp);
        }
      }

      // If a suitable file is found then passe it threw the preview function,
      // else default to unavaiable.
      if ($filename) {
        $output['preview'] = recline_preview_zip($filename);
      }
      else {
        $output['preview'] = recline_preview_unavailable();
      }
      break;

    case 'html':
      $output['preview'] = recline_format_link_api($url);
      break;
    case 'png':
    case 'gif':
    case 'jpg':
      $output['preview'] = recline_format_image($url);
      break;
    case 'arcgis':
      $output['preview'] = recline_preview_arcgis_feature($url);
      break;
    case 'rdf':
    case 'xml':
      $output['preview'] = recline_prettify_xml($url);
      break;
    case 'pdf':
      $output['preview'] = recline_format_link_api($url);
      break;
    case 'wms':
      // We can accept just the bare endpoint or the XML preview.
      $parsed = parse_url($url);
      $wms_url = $parsed['scheme'] . '://' . $parsed['host'] . $parsed['path'];
      // We need the xml preview to get the correct layers.
      $service_url = $wms_url . '?request=GetCapabilities&service=WMS';
      // If the initial request was sent without the correct endpoint.
      if (!isset($request->data) || empty($request->data) || preg_match('/The request not allowed/', $request->data)) {
        $request = drupal_http_request($service_url, array('timeout' => RECLINE_REQUEST_TIMEOUT_GET));
      }
      if ($request->code != 200) {
        $output['preview'] = recline_preview_unavailable();
      }
      try {
        $xml = new SimpleXMLElement($request->data);
        $titles = array();
        $images = array();
        if (isset($xml->Capability->Layer->EX_GeographicBoundingBox)) {
          $north = "{$xml->Capability->Layer->EX_GeographicBoundingBox->northBoundLatitude}";
          $south = "{$xml->Capability->Layer->EX_GeographicBoundingBox->southBoundLatitude}";
          $east = "{$xml->Capability->Layer->EX_GeographicBoundingBox->eastBoundLongitude}";
          $west = "{$xml->Capability->Layer->EX_GeographicBoundingBox->westBoundLongitude}";
        }
        else {
          $north = variable_get("recline_wms_north_bound", "51.2");
          $south = variable_get("recline_wms_south_bound", "29.2");
          $east = variable_get("recline_wms_south_bound", "-73.2");
          $west = variable_get("recline_wms_south_bound", "-125.5");
        }
        foreach ($xml->Capability->Layer->Layer as $layer) {
          $titles[] = "{$layer->Title}";
          if (isset($layer->Style->LegendURL->OnlineResource)) {
            $images["{$layer->Title}"] = "{$layer->Style->LegendURL->OnlineResource->attributes('xlink', TRUE)->href}";
          }
        }
        $output['preview'] = recline_preview_wms($wms_url, $titles, $images, $north, $east, $south, $west);
      }
      catch (Exception $e) {
        $output['preview'] = recline_preview_unavailable();
      }
      break;
    default:
      $output[] = recline_preview_unavailable();
  }
  return drupal_render($output);
}

/**
 * Previews WMS link.
 */
function recline_preview_wms($url, $titles, $images, $north, $east, $south, $west) {
  $items = array();
  foreach ($titles as $title) {
    if (isset($images[$title])) {
      $title = '<img src="' . $images[$title] . '" alt="' . t("!title legend item", array("!title" => $title)) . '"/> ' . $title;
    }
    $items[] = array(
      'data' => $title,
    );
  }
  $output = "var map = L.map('map', {
        zoom: 7
      });";
  drupal_add_js('https://cdn.jsdelivr.net/leaflet/1.0.2/leaflet.js', 'external');
  drupal_add_css('https://cdn.jsdelivr.net/leaflet/1.0.2/leaflet.css', 'external');
  $output .= "var url = '" . $url . "';";
  $output .= "L.tileLayer('http://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png').addTo(map);";
  $output .= "var layers = ['" . implode(",", $titles) . "'];";
  $output .= "var tiles = L.tileLayer.wms(url, {
          layers: layers,
          format: 'image/png',
          transparent: true,
      }).addTo(map);map.fitBounds([['" . $south . "','" . $west . "'],['" . $north . "','" . $east . "']]);";
  drupal_add_js($output, array('type' => 'inline', 'scope' => 'footer', 'weight' => 5));
  return array(
    '#type' => 'markup',
    '#markup' => '<div id="map" style="width: 100%; height: 500px"></div>' . theme('item_list', array('items' => $items, 'title' => t("Legend"), 'attributes' => array('class' => 'list-inline'))),
  );
}

/**
 * Formats image.
 */
function recline_format_image($url) {
  return array(
    '#type' => 'markup',
    '#markup' => '<div  id="recline-image-preview"><img src="' . $url . '" alt="image preview"/></div>',
  );
}

/**
 * Builds output for a link of an api.
 */
function recline_format_link_api($url) {
  return array(
    'iframe' => array(
      '#type' => 'html_tag',
      '#tag' => 'iframe',
      '#attributes' => array(
        'src' => $url,
        'height' => 600,
        'width' => 900,
        'style' => '',
      ),
      '#value' => '',
    ),
  );
}

/**
 * Provide unavaiable status message.
 */
function recline_preview_unavailable() {
  return array(
    '#type' => 'markup',
    '#markup' => t('Data preview unavailable.'),
    '#prefix' => '<div class="messages status">',
    '#suffix' => '</div>',
  );
}

/**
 * Prettify json.
 */
function recline_preview_json($url) {
  $response = drupal_http_request($url, array('timeout' => RECLINE_REQUEST_TIMEOUT_GET));
  if ($response->code == '200') {
    $data = $response->data;
  }
  else {
    return recline_preview_unavailable();
  }
  $decoded = drupal_json_decode($data);

  // When a json is a geojson file then return a geojson preview
  // instead of a normal json preview.
  $types = array('FeatureCollection', 'GeometryCollection', 'Feature', 'Point', 'MultiPoint', 'LineString', 'MultiLineString', 'Polygon', 'MultiPolygon');
  foreach ($types as $type) {
    foreach ($decoded as $key => $value) {
      if ($type === $value) {
        return recline_preview_geojson_leaflet($url);
      }
    }
  }
  libraries_load('jsonview');
  drupal_add_js(array('recline' => array('data' => $data)), 'setting');
  drupal_add_js(drupal_get_path('module', 'recline') . '/js/jsondataview.js');
  $json = array(
    'toggle_btn' => array(
      '#type' => 'markup',
      '#markup' => '<a id="toggle-btn" class="embed-link">Toggle view</a>',
    ),
    'data_json' => array(
      '#type' => 'markup',
      '#markup' => '<div id="recline-data-json"></div>',
    ),
  );
  return $json;
}

/**
 * Prettify xml.
 */
function recline_prettify_xml($url) {
  $response = drupal_http_request($url, array('timeout' => RECLINE_REQUEST_TIMEOUT_GET));
  if ($response->code == '200') {
    $data = recline_replace_lt_gt($response->data);
  }
  else {
    return recline_preview_unavailable();
  }
  drupal_add_library('recline', 'highlight');
  drupal_add_js('jQuery(document).ready(function () {hljs.initHighlightingOnLoad();});', 'inline');
  $xml = array(
    'data_xml' => array(
      '#type' => 'markup',
      '#markup' => '<div id="recline-xml-preview"><pre><code class="hljs xml">' . $data . '</code></pre></div>',
    ),
  );
  return $xml;
}

/**
 * Replace angle brackets.
 */
function recline_replace_lt_gt($data) {
  $replaced = preg_replace('/</', '&lt;', $data);
  return preg_replace('/>/', '&gt;', $replaced);
}

/**
 * Preview for arcgis and rest data.
 */
function recline_preview_arcgis_feature($url) {
  drupal_add_js('https://cdn.jsdelivr.net/leaflet/1.0.2/leaflet.js', 'external');
  drupal_add_js('https:////cdn.jsdelivr.net/leaflet.esri/1.0.0/esri-leaflet.js', 'external');
  drupal_add_css('https://cdn.jsdelivr.net/leaflet/1.0.2/leaflet.css', 'external');
  drupal_add_css('body { margin:0; padding:0; } #rest-map { position: relative; height: 500px; width: 100%;}', 'inline');
  $lat = variable_get('recline_default_lat', "38");
  $lon = variable_get('recline_default_lon', "-99");
  $module_path = drupal_get_path('module', 'recline');
  drupal_add_js(array('recline' => array('url' => $url, 'lat' => $lat, 'lon' => $lon)), 'setting');
  drupal_add_js($module_path . '/restdataview.js');
  $output = array(
    'map' => array(
      '#type' => 'markup',
      '#markup' => '<div><div id="rest-map"></div></div>',
    ),
  );
  return $output;
}

/**
 * Returns output for a geojson file using leaflet library.
 */
function recline_preview_geojson_leaflet($url) {
  $response = drupal_http_request($url);
  if ($response->code == '200') {
    $geojson = $response->data;
  }
  else {
    return ;
  }
  libraries_load('leaflet_markercluster');
  $leaflet_imgs = libraries_get_path('recline') . '/vendor/leaflet/1.0.2/images/';
  drupal_add_js('L.Icon.Default.imagePath = "/' . $leaflet_imgs . '"', 'inline');
  drupal_add_library('leaflet_widget', 'leaflet');
  $output = "var dataExtent = " . $geojson;
  $output .= "; var map = L.map('map');
    L.tileLayer('//{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      attribution: 'Map data &copy; <a href=\"http://openstreetmap.org\">OpenStreetMap</a>'
    }).addTo(map);
    var markers = new L.markerClusterGroup();
    var geojson = L.geoJson(dataExtent);
    markers.addLayer(geojson);
    map.addLayer(markers);
    map.fitBounds(geojson.getBounds());";
  drupal_add_js($output, array('type' => 'inline', 'scope' => 'footer', 'weight' => 5));
  return array(
    'map' => array(
      '#type' => 'markup',
      '#markup' => '<div id="map" style="height:450px; width:100%"></div>',
    ),
  );
  return $output;
}
/**
 * Builds output render array for files if item is an archive.
 */
function recline_preview_zip($uri) {
  // List files if item is an archive.
  try {
    $archiver = archiver_get_archiver($uri);
  }
  catch (Exception $e) {
    return recline_preview_unavailable();
  }
  if ($archiver) {
    $module_path = drupal_get_path('module', 'recline');
    drupal_add_css($module_path . '/recline.css');
    $output['ziplist'] = array(
      '#theme' => 'item_list',
      '#prefix' => '<div id="recline-zip-list">',
      '#suffix' => '</div>',
      '#title' => t('%number files in this archive', array('%number' => count($archiver->listContents()))),
      '#items' => $archiver->listContents(),
    );
  }
  elseif ($format == 'zip') {
    $archivers_available = implode(', ', array_keys(archiver_get_info()));
    drupal_set_message(t('Unable to provide file listing for zip format on this system. Available archiver formats are: %s',
      array('%s' => $archivers_available)));
  }
  return $output;
}
/**
 * Builds icon for resource view.
 */
function recline_build_icon($url, $format, $filename, $filemime, $filesize, $description) {
  // Set options as per anchor format described at
  // http://microformats.org/wiki/file-format-examples
  $options = array(
    'attributes' => array(
      'type' => $filemime . '; length=' . $filesize,
      'data-format' => $format,
      'class' => array('format-label'),
      'title' => check_plain($filename),
    ),
  );
  $file = l($filename, $url, $options);
  return array(
    'download' => array(
      '#type' => 'markup',
      '#markup' => '<div class="download">' . $file . '</div>',
    ),
  );
}


/**
 * Builds output render array for csv files
 */
function recline_preview_multiview($variables) {
  $output = array();
  $datastoreStatus = $table = FALSE;
  $module_path = drupal_get_path('module', 'recline');
  $item = $variables['item'];

  //Check problem loading file
  if (!isset($item['uri']) && $item['fid']) {
    $file = file_load($item['fid']);
    $file = (array)$file;
    $item = array_replace($item, $file);
  }
  $node = $item['entity'];
  $embed_markup = theme('recline_embed_button', array('node' => $node));
  // See if datastore is loaded, if so, prepare recline to view from it.
  if (module_exists('dkan_datastore_api') && module_exists('feeds_flatstore_processor') && function_exists('dkan_datastore_api_get_feeds_source')) {
    $source_id = dkan_datastore_api_get_feeds_source($node->nid);
    if ($table = feeds_flatstore_processor_table_name($source_id, $node->nid)) {
      if (db_table_exists($table)) {
        $datastoreStatus = $table;
      }
    }
  }

  // Load all the required libraries.
  libraries_load('lodash');
  libraries_load('backbone');
  $recline = libraries_load('recline');
  libraries_load('csv');
  libraries_load('jsxlsx');
  libraries_load('xls');
  libraries_load('mustache');
  libraries_load('moment');
  libraries_load('slickgrid');
  libraries_load('leaflet');
  libraries_load('flot');
  libraries_load('deep_diff');
  libraries_load('recline_deeplink');
  libraries_load('leaflet_markercluster');
  drupal_add_library('system', 'ui.sortable');
  drupal_add_js($module_path . '/backend.ckan_get.js');
  drupal_add_js($module_path . '/recline.js');
  drupal_add_css($module_path . '/recline.css');
  $is_remote = !empty($node->field_link_remote_file);
  $is_uploaded = !empty($node->field_upload);
  $show_all = $is_uploaded && empty($item['map']) && empty($item['grid']) && empty($item['graph']);

  if ($show_all || $is_remote || !empty($item['map']) || !empty($item['grid']) || !empty($item['graph']) || !empty($item['embed'])) {
    $uuid = isset($node->uuid) ? $node->uuid : FALSE;
    $file_path = isset($item['uri']) ? file_create_url($item['uri']) : '';
    $delimiter = isset($item['delimiter']) ? $item['delimiter'] : ',';
    $real_path = drupal_realpath($item['uri']);
    $grid = !$is_remote && !$show_all ? (int) $item['grid'] : 1;
    $graph = !$is_remote && !$show_all ? (int) $item['graph'] : 1;
    $map = !$is_remote && !$show_all ? (int) $item['map'] : 1;
    $embed = !empty($item['embed']) ? (int) $item['embed'] : 0;

    $settings['recline'] = array(
      'file' => $file_path,
      'fileType' => $item['filemime'],
      'fileSize' => (file_exists($real_path)) ? filesize($real_path) : 0,
      'delimiter' => $delimiter,
      'grid' => (int) $grid,
      'graph' => (int) $graph,
      'map' => (int) $map,
      'embed' => (int) $embed,
      'uuid' => $uuid,
      'datastoreStatus' => $datastoreStatus,
      'maxSizePreview' => variable_get('file_size_preview_limit', FILE_SIZE_PREVIEW_LIMIT),
    );
    drupal_add_js($settings, 'setting');
  }

  // Necessary to fix https://github.com/mapbox/mapbox.js/issues/339
  // Update the leaflet library and test if this bug goes away.
  // That will involve a lot of QA but we have to do it a some point.
  if ($recline) {
    $leaflet_imgs = $recline['library path'] . '/vendor/leaflet/1.0.2/images/';
    drupal_add_js('L.Icon.Default.imagePath = "/' . $leaflet_imgs . '"', 'inline');
  }

  if (!empty($settings['recline']['embed'])) {
    $output['embed'] = array(
      '#type' => 'markup',
      '#markup' => $embed_markup,
    );
  }

  $output['data_explorer'] = array(
    '#type' => 'markup',
    '#markup' => '<span class="data-explorer"></span>',
  );
  return $output;
}

/**
 * Implements hook_preprocess_HOOK().
 *
 * Process variables for recline-embed-button.tpl.php.
 */
function template_preprocess_recline_embed_button(&$variables) {
  $node = $variables['node'];
  $variables['embed_url'] = url('node/' . $node->nid . '/recline-embed', array('absolute' => TRUE));
}

/**
 * Returns HTML for an recline field widget.
 *
 * @param array $variables
 *   An associative array containing:
 *   - item: Associative array of recline field
 *
 * @ingroup themeable
 */
function theme_recline_widget($variables) {
  $element = $variables['element'];

  $output = '';
  $output .= '<div class="recline-widget form-managed-file clearfix">';
  $output .= drupal_render_children($element);
  $output .= '</div>';

  return $output;
}
