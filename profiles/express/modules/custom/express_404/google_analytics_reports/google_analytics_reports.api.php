<?php

/**
 * @file
 * Hooks provided by the Google Analytics Reports module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Allow modules to alter Google Analytics field data before saving in database.
 *
 * @param $field
 *   An associative array containing:
 *   - id: Google Analytics field id without "ga:" at the beginning.
 *   - kind: collection type.
 *   - attributes: an associative array containing:
 *     - type: the type of field.
 *     - dataType: the type of data this field represents.
 *     - group: The dimensions/metrics group the column belongs to.
 *     - status: the status of the column.
 *     - uiName: the name/label of the field used in user interfaces (UI).
 *     - description: The full description of the field.
 *     - allowedInSegments: Indicates whether the column can be used in
 *       the segment query parameter.
 *     - calculation: this shows how the metric is calculated. Only available
 *       for calculated metrics.
 *     - minTemplateIndex: this is the minimum index for the field. Only
 *       available for templatized fields.
 *     - maxTemplateIndex: this is the maximum index for the field. Only
 *       available for templatized fields
 *     - premiumMinTemplateIndex: this is the minimum index for the field
 *       for premium properties. Only available for templatized fields.
 *     - premiumMaxTemplateIndex: this is the maximum index for the field
 *       for premium properties. Only available for templatized fields.
 *     - allowedInSegments: Indicates whether the field can be used in
 *       the segment query parameter.
 */
function hook_google_analytics_reports_field_import_alter(&$field) {
  // Change data type for Date field.
  if ($field['id'] == 'date') {
    $field['attributes']['dataType'] = 'date';
  }
}

/**
 * @} End of "addtogroup hooks".
 */
