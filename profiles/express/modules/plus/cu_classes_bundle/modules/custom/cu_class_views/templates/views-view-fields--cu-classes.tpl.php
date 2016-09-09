<?php
// Create class title
$class_id = $fields['class_id']->raw;
$course_title = cu_class_views_get_course_title($class_id);
$class_title = array();

$course_title_array[] = '<span>' . $fields['field_class_subject']->content . '</span>';
$course_title_array[] = $fields['field_class_catalog_nbr']->content;
$course_title_array[] = ' : ';
$course_title_array[] = html_entity_decode($course_title);
$class_title[] = '<h2 class="class-title">' . join(' ', $course_title_array)  . '</h2>';
$section_number = sprintf("%03s", $fields['field_class_class_section']->content);
$topic = !empty($fields['field_class_crs_topic_descr']->content) ? ' : ' . $fields['field_class_crs_topic_descr']->content : '';
$class_title[] =  '<div class="class-section"><h3 class="section-label">Section</h3> ' . $section_number  . $topic . '</div>';


// Link class title to course page
$class_title_link = l(join(' ', $class_title), 'course/' . $fields['field_class_crse_id']->content, array('html' => TRUE, 'query' => array('class_id' => $fields['field_class_class_nbr']->content), 'attributes' => array('class' => array('class-title-group'))));

// Term/strm
$term = cu_class_view_term_code_translate($fields['field_class_strm']->content);
$term = ucfirst(join(' ', $term));

// Dates
$dates = array();
$dates[] = $fields['field_class_start_dt']->content;
$dates[] = $fields['field_class_end_dt']->content;

$updated = 'Updated: ' . $fields['field_class_last_updated']->content;

// Available seats
$cap = intval($fields['field_class_enrl_cap']->content);
$total = intval($fields['field_class_enrl_tot']->content);
$available = $cap - $total;
if ($available == 1) {
  $available_seats = $available . ' seat available';
} else {
  $available_seats = $available . ' seats available';
}

// Format/Instruction Mode
$format = $fields['field_class_instruction_mode_des']->content;
$format_parts = explode('/', $format);
foreach ($format_parts as $key => $part) {
  $format_parts[$key] = '<span>' . $part . '</span>';
}
$format = join(' / ', $format_parts);

// Meeting time
$meeting_time_raw = $fields['field_class_ssr_mtg_sched_long']->content;
$meeting_time_exempt = array (
  'tba',
  'TBA',
  'tbd',
  'TBD',
);
if (!in_array($meeting_time_raw, $meeting_time_exempt)) {
  $meeting_time_parts = explode(' ', $meeting_time_raw);
  // Meeting schedule display
  foreach ($meeting_time_parts as $key => $part) {
    // Add spaces to days
    if (ctype_alpha($part)) {
      $regex = '/(?<!^)((?<![[:upper:]])[[:upper:]]|[[:upper:]](?![[:upper:]]))/';
      $meeting_time_parts[$key] = preg_replace( $regex, ' $1', $part ) . '<br />';
    }
    // Convert to 12hour time
    if (strpos($part, ':') !== FALSE) {
      $meeting_time_parts[$key] = date('g:i a', strtotime($part));
    }
  }
  $meeting_time = join(' ',$meeting_time_parts);
}
else {
  $meeting_time = $meeting_time_raw;
}

?>
<div class="class-view-mode-list class-entity-wrapper">
  <div class="class-row-wrapper">
    <div class="class-content">
      <div class="row clearfix">
        <div class="col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <?php print $class_title_link; ?>
        </div>

        <div class="col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <h4 class="class-label">Campus:</h4>
          <?php print cu_class_views_institutions($fields['field_class_institution']->content); ?>
        </div>
        <div class="col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <h4 class="class-label">Term:</h4>
          <?php print $term; ?>
          <?php if (isset($fields['field_class_session_code']->content)): ?>
            <br />
            <?php print cu_class_views_session_translator($fields['field_class_session_code']->content); ?>
          <?php endif; ?>
        </div>
        <div class="col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <h4 class="class-label">Format:</h4>
          <?php print $format; ?>
        </div>
        <div class="col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <h4 class="class-label">Schedule:</h4>
          <?php print $meeting_time; ?>
        </div>

      </div>
    </div>
  </div>
</div>
