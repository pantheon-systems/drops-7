<?php

/**
 * Class view callback.
 */
function cu_course_view($course) {
  drupal_set_title(entity_label('cu_course', $course));
  return entity_view('cu_course', array(entity_id('cu_course', $course) => $course), 'full');
}

function cu_course_display($course) {

  $highlighted_class = NULL;

  //before a course is displayed, check to see if we need to udpate the classes
  if (module_exists('cu_class_import') && variable_get('cu_class_import_on_load', 1)) {
    cu_class_import($course->inst, $course->term_id, $course->subject, $course->course_id);
  }

  $breadcrumb = array();
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Courses', 'courses/search');
  drupal_set_breadcrumb($breadcrumb);
  // If there is a class number in the url, highlight that class
  if (isset($_GET['class_id']) && is_numeric($_GET['class_id'])) {
    $class_number = $_GET['class_id'];

    // Query course id table
    $query = db_select('field_data_field_class_crse_id', 'courseid');
    $query->fields('courseid', array('entity_id'));

    // Join to get class entities that match course id
    $query->join('cu_class', 'class', 'class.class_id = courseid.entity_id');
    $query->fields('class', array('class_id'));

    // Join to get specific class from url
    $query->join('field_data_field_class_class_nbr', 'class_nbr', 'class_nbr.entity_id = courseid.entity_id');
    $query->fields('class_nbr', array('field_class_class_nbr_value'));
    $query->condition('class_nbr.field_class_class_nbr_value', $class_number, '=');

    // Get only class entities
    $query->condition('class.type', 'class', '=');
    $query->condition('courseid.field_class_crse_id_value', $course->course_id, '=');
    $query->orderBy('courseid.field_class_crse_id_value', 'DESC');
    $highlight_class_ids = $query->execute()->fetchCol();

  }

  // Query course id table
  $query = db_select('field_data_field_class_crse_id', 'courseid');
  $query->fields('courseid', array('entity_id'));

  // Join to get class entities that match course id
  $query->join('cu_class', 'class', 'class.class_id = courseid.entity_id');
  $query->fields('class', array('class_id'));

  // Join to get cancel_dt for classes
  $query->leftJoin('field_data_field_class_cancel_dt', 'cancel_dt', 'class.class_id = cancel_dt.entity_id');
  $query->fields('cancel_dt', array('field_class_cancel_dt_value'));

  // Get only published class entities
  $query->condition('class.type', 'class', '=');
  $query->condition('class.status', 1, '=');
  $query->condition('courseid.field_class_crse_id_value', $course->course_id, '=');
  $query->condition(db_or()
    ->isNull('cancel_dt.field_class_cancel_dt_value')
    ->condition('cancel_dt.field_class_cancel_dt_value', date("Y-m-d H:i:s", time() - (7 * 24 * 60 * 60)), '>'));

  // Join for section number
  $query->join('field_data_field_class_class_section', 'section', 'section.entity_id = courseid.entity_id');
  $query->fields('section', array('field_class_class_section_value'));
  $query->orderBy('courseid.field_class_crse_id_value', 'ASC');
  $query->orderBy('section.field_class_class_section_value', 'ASC');
  $class_ids = $query->execute()->fetchCol();

  // Put loaded class entities into an array for viewing
  $classes = array();
  foreach ($class_ids as $class_id) {
    $classes[$class_id] = entity_load_single('cu_class', $class_id);
  }

  // Remove highlighted class from list
  if (!empty($highlight_class_ids)) {
    $current_class = current($highlight_class_ids);
    $highlighted_class = $classes[$current_class];
    unset($classes[$current_class]);
  }
  $class_multiple = !empty($classes) ? TRUE : FALSE;
  $output = array();

  if ($class_multiple) {
    $output['classes'] = entity_view('cu_class', $classes, 'short');
    $output['classes']['#weight'] = 100;
    $output['classes']['#prefix'] = '<div class="course-classes">';
    $output['classes']['#suffix'] = '</div>';
    // if multiple classes get description here
    $class = current($output['classes']['cu_class']);
    $output['description']['text'] = $class['field_class_ssr_descrlong'];
  }

  $output['#prefix'] = '<div class="course-class-list">';
  $output['#suffix'] = '</div>';

  // Check to see if the requested class is published.
  // @todo Remove this since the query filters out unpublished classes.
  if (isset($highlighted_class->status) && $highlighted_class->status < 1) {
    if ($class_multiple) {
      drupal_set_message("This section of this course is no longer available. Other sections listed bellow.", 'error');
    }
    else {
      drupal_set_message("This section of this course is no longer available. No other sections of this course are currently being offered.", 'error');
    }
  }

  // Check to see if the requested class cancelled.
  if (isset($highlighted_class->field_class_cancel_dt[LANGUAGE_NONE][0]['value'])) {
    if ($class_multiple) {
      drupal_set_message("This section of this course has been cancelled. Other sections listed bellow.", 'error');
    }
    else {
      drupal_set_message("This section of this course has been cancelled. No other sections of this course are currently being offered.", 'error');
    }
  }

  // If there is a highlighted class, display page differently
  if (!empty($highlight_class_ids)) {
    $classes = array();
    foreach ($highlight_class_ids as $class_id) {
      $classes[$class_id] = entity_load_single('cu_class', $class_id);
    }

    // Highlighted class
    $output['classes_highlighted'] = entity_view('cu_class', $classes, 'short');
    $output['classes_highlighted']['#weight'] = 99;
    $output['classes_highlighted']['#prefix'] = '<div class="course-classes-highlighted">';
    $output['classes_highlighted']['#suffix'] = '</div>';
    // If single class get description here
    $class = current($output['classes_highlighted']['cu_class']);
    $output['description']['text'] = $class['field_class_ssr_descrlong'];

    // Put the rest of the classes in an expandable section
    if ($class_multiple) {
      $output['classes']['#prefix'] = '<div class="course-classes-all"><a href="#other-classes" id="other-classes-trigger"><i class="fa fa-plus-square"></i> Other sections of this course</a><div id="other-classes">';
      $output['classes']['#suffix'] = '</div></div>';
    }
  }

  // Description markup
  $output['description']['#prefix'] = '<div class="course-description">';
  $output['description']['#suffix'] = '</div>';
  $output['description']['#sorted'] = TRUE;
  return $output;
}

function cu_course_title($course) {
  $title = array();
  $title[] = $course->subject;
  $title[] = $course->catalog_nbr;
  $title[] = $course->title;
  return join(' ', $title);
}


/**
 * Display classes from course at courses/SUBJECT/catalog_number
 */
function cu_course_classes_display($subject, $catalog_number) {
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Courses', 'courses/search');
  $breadcrumb[] = l(cu_course_list_title_from_subject($subject), 'courses/subject/' . $subject);
  drupal_set_breadcrumb($breadcrumb);
  $output = array();

  $query = db_select('cu_course', 'courses');
  $query->fields('courses', array('course_id', 'subject', 'catalog_nbr'));
  $query->condition('courses.subject', $subject, '=');
  $query->condition('courses.catalog_nbr', $catalog_number, '=');
  $course = $query->execute()->fetchCol();
  $course_id = $course[0];

  // Query course id table
  $query = db_select('field_data_field_class_crse_id', 'courseid');
  $query->fields('courseid', array('entity_id'));

  // Join to get class entities that match course id
  $query->join('cu_class', 'class', 'class.class_id = courseid.entity_id');
  $query->fields('class', array('class_id'));

  // Get only class entities
  $query->condition('class.type', 'class', '=');
  $query->condition('courseid.field_class_crse_id_value', $course_id, '=');
  $query->orderBy('courseid.field_class_crse_id_value', 'DESC');
  $class_ids = $query->execute()->fetchCol();




  // Put loaded class entities into an array for viewing
  $classes = array();
  foreach ($class_ids as $class_id) {
    $classes[$class_id] = entity_load_single('cu_class', $class_id);
  }
  // Remove highlighted class from list
  if (!empty($highlight_class_ids)) {
    $current_class = current($highlight_class_ids);
    unset($classes[$current_class]);
  }
  $class_multiple = !empty($classes) ? TRUE : FALSE;
  $output = array();

  if ($class_multiple) {
    $output['classes'] = entity_view('cu_class', $classes, 'short');
    $output['classes']['#weight'] = 100;
    $output['classes']['#prefix'] = '<div class="course-classes">';
    $output['classes']['#suffix'] = '</div>';
    // if multiple classes get description here
    $class = current($output['classes']['cu_class']);
    $output['description']['credits']['#markup'] = number_format($class['field_class_units_acad_prog'][0]['#markup'], 1, '.', '') . ' Credit Hours';
    $output['description']['credits']['#prefix'] = '<h3 class="course-credits">';
    $output['description']['credits']['#suffix'] = '</h3>';
    $output['description']['text'] = $class['field_class_ssr_descrlong'];
  }


  $output['#prefix'] = '<div class="course-class-list">';
  $output['#suffix'] = '</div>';


  return $output;
}

function cu_course_title_from_catalog_number($subject, $catalog_number) {
  $query = db_select('cu_course', 'courses');
  $query->fields('courses', array('title', 'subject', 'catalog_nbr'));
  $query->condition('courses.subject', $subject, '=');
  $query->condition('courses.catalog_nbr', $catalog_number, '=');
  $course = $query->execute()->fetchAssoc();
  $title = array();
  $title[] = $course['subject'];
  $title[] = $course['catalog_nbr'];
  $title[] = $course['title'];
  return join(' ', $title);
}

function cu_course_by_subject_display($subject) {
  $breadcrumb[] = l('Home', '<front>');
  $breadcrumb[] = l('Courses', 'courses/search');
  drupal_set_breadcrumb($breadcrumb);
  $output = array();

  $query = db_select('cu_course', 'courses');
  $query->fields('courses', array('title','subject', 'catalog_nbr'));
  $query->condition('courses.subject', $subject, '=');
  $query->orderBy('catalog_nbr', 'ASC');
  $result = $query->execute();
  $courses = array();
  while($course = $result->fetchAssoc()) {
    $catalog_number = $course['catalog_nbr'];
    $courses[$catalog_number] = $course;
    $title = array();
    $title[] = $course['subject'];
    $title[] = $course['catalog_nbr'];
    $title[] = $course['title'];
    $course_title =  join(' ', $title);
    $output['courses'][$catalog_number]['#markup'] = l($course_title, 'courses/subject/' . $course['subject']. '/' . $catalog_number);
    $output['courses'][$catalog_number]['#prefix'] = '<div class="course-class-list-item">';
    $output['courses'][$catalog_number]['#suffix'] = '</div>';
  }

  return $output;
}

function cu_course_list_title_from_subject($subject) {
  return cu_class_subject_translate(strtoupper($subject)) . ' Courses';
}
