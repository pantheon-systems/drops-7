<?php


/**
 * Alter the query before executing the query.
 *
 * Keeping this code around until the classes work is stable.
 * Delete this file if code is still commented out.
 *
 * @param $view
 *   The view object about to be processed.
 * @param $query
 *   An object describing the query.
 * @see hook_views_query_substitutions()
 */
/*
function cu_class_views_views_query_alter(&$view, &$query) {

  if ($view->name == 'cu_classes') {
    foreach ($query->where[2]['conditions'] as $key => $value) {
      // If no subject value selected, continue.
      if ($value['field'] == 'field_data_field_class_subject.field_class_subject_value') {
        $subjects = db_select('cu_courses_subjects', 'subjects');
        $subjects->fields('subjects', array('code', 'value', 'readable_value', 'parent_org', 'status'));
        $subjects->orderBy('subjects.value', 'ASC');
        $subjects->condition('subjects.status', 1, '=');
        $subjects->condition('subjects.readable_value', $value['value'], '=');
        $results = $subjects->execute()->fetchAllAssoc('code');
        $query->where[2]['conditions'][$key]['value'] = implode('|', array_keys($results));
      }
    }
  }
}
*/

