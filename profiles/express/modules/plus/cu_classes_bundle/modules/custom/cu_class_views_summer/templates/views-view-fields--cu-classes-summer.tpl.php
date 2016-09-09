<?php

// Create class title
$class_title = array();
$class_title[] = '<h2 class="class-title">' .  html_entity_decode($fields['field_class_course_title_long']->content) . '</h2>';
$class_title[] = '<h3 class="class-number">' .  $fields['field_class_subject']->content . ' ' . $fields['field_class_catalog_nbr']->content . '</h3>';
$section_number = sprintf("%03s", $fields['field_class_class_section']->content);
$class_title[] =  '<h3 class="class-section">Section ' . $section_number . '</h3>';

if (isset($fields['field_class_instruction_mode']->content)) {
  $mode = $fields['field_class_instruction_mode']->content;
  //$class_title[] = '<span>' . cu_class_format_translate($mode) . '</span>';
}

// Link class title to course page
$class_title_link = l(join(' ', $class_title), 'course/' . $fields['field_class_crse_id']->content, array('html' => TRUE, 'query' => array('class_id' => $fields['field_class_class_nbr']->content), 'attributes' => array('class' => array('class-title-group'))));

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
// Core requirements
if ($fields['field_class_crse_attr']->content == 'BCRQ') {
  $reqs = $fields['field_class_crse_attr_value']->content;
  $course_requirements = cu_class_core_requirements($reqs);
}
$session = cu_class_session_translator($fields['field_class_session_code']->content);

$format = cu_class_format_translate($fields['field_class_instruction_mode']->content);

// Meeting time
$meeting_time_raw = $fields['field_class_ssr_mtg_sched_long']->content;
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
?>
<div class="class-view-mode-list class-entity-wrapper">
  <div class="class-row-wrapper">
    <div class="class-content">
      <div class="row clearfix">
        <div class="class-meta col-lg-4 col-md-4 col-sm-4 col-xs-12">
          <?php print $class_title_link; ?>
        </div>
        <div class="class-date-instructor col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <div class="class-section">
            <h3 >Session</h3>
            <?php print $session; ?>

            <div class="class-meeting-time">
              <?php
                if (strlen($fields['field_class_ssr_mtg_sched_long']->content) > 5) {
                  print $meeting_time;
                }
              ?>
            </div>

          </div>

        </div>
        <div class="class-availability col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <div class="class-section">
            <h3 >Credits</h3>
            <?php
              $credits_value = $fields['field_class_units_acad_prog']->content;
              print number_format($credits_value, 1, '.', '');
            ?>
          </div>
        </div>

        <div class="class-format col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <div class="class-section">
            <h3 >Format</h3>
            <?php print $format; ?>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
