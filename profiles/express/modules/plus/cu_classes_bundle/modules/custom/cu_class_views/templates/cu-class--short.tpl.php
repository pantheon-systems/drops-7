<div class="class-view-mode-short class-entity-wrapper">
  <div class="class-row-wrapper">
    <div class="class-content">
      <div class="row clearfix">
        <div class="col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <h3 class="class-title">
          Section <?php print sprintf("%03s", $content['field_class_class_section'][0]['#markup']); ?>
          <?php
            if(!empty($content['field_class_crs_topic_descr'][0]['#markup'])): ?>
             : <?php print $content['field_class_crs_topic_descr'][0]['#markup']; ?>

          <?php endif; ?>
          <?php if (!empty($content['field_class_units_acad_prog'])): ?>
            <p><?php print $content['field_class_units_acad_prog'][0]['#markup']; ?> Credit Hours</p>
          <?php endif; ?>
          </h3>
        </div>

        <div class="col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <h4 class="class-label">Campus:</h4>
          <?php print cu_class_views_institutions($content['field_class_institution'][0]['#markup']); ?>
        </div>
        <div class="col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <h4 class="class-label">Term:</h4>
          <?php
            $term = cu_class_view_term_code_translate($content['field_class_strm'][0]['#markup']);
            $term = ucfirst(join(' ', $term));
            print $term;
          ?>
          <?php
            if (!empty($content['field_class_session_code'][0]['#markup'])):
          ?>
            <br /> <?php print cu_class_views_session_translator($content['field_class_session_code'][0]['#markup']); ?>
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
      <div class="row clearfix">
        <div class="col-lg-3 col-md-3 col-sm-3 col-xs-12">
          <h4 class="class-label">Class Number:</h4>
          <?php print render($content['field_class_class_nbr']); ?>
        </div>
        <div class="col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <h4 class="class-label">Instructor:</h4>
          <?php print render($content['field_class_ssr_instr_long']); ?>
        </div>
        <div class="col-lg-2 col-md-2 col-sm-2 col-xs-12">
          <h4 class="class-label">Location:</h4>
          <?php print render($content['field_class_ssr_mtg_loc_long']); ?>
        </div>
        <div class="col-lg-4 col-md-4 col-sm-4 col-xs-12">
          <h4 class="class-label">Availability:</h4>
          <?php print $available; ?>
          <div class="updated">
            <?php print $updated; ?>
          </div>
        </div>
      </div>
    </div>
  </div>
</div>
