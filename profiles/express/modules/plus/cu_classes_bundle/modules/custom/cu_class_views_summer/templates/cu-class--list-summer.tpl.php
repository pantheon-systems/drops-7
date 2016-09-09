<div class="class-view-mode-list class-entity-wrapper">
  <div class="class-wrapper">
    <h2 class="class-title"><?php print $course_link; ?></h2>
    <div class="class-content">
      <div class="row clearfix">
        <div class="class-meta class-section col-lg-6 col-md-6 col-sm-6 col-xs-12">
          <div class="row">
            <div class="class-section col-lg-6 col-md-6 col-sm-6 col-xs-12">
              <h3>Class No.</h3>
              <?php print render($content['field_class_class_nbr']); ?>
            </div>
            <div class="class-section col-lg-6 col-md-6 col-sm-6 col-xs-12">

              <h3>Core</h3>
              <?php print render($content['field_class_crse_attr_value']); ?>
            </div>
          </div>

        </div>
        <div class="class-date-instructor col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <div class="class-section">
            <h3>Course Dates</h3>
            <?php print $start_date; ?> - <?php print $end_date; ?>
          </div>
          <div class="class-section">
            <h3>Instructor</h3>
            <?php print render($content['field_class_ssr_instr_long']); ?>
          </div>

        </div>
        <div class="class-availability class-section col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <h3>Availability</h3>
          <?php print $available; ?>
          <div class="updated">
            <?php print $updated; ?>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
