<div class="issue-wrapper clearfix row">
  <?php if (!empty($content['field_issue_image']) || !empty($content['body']) || !empty($content['body']) || !empty($content['body'])): ?>
    <div class="issue-notes col-lg-4 col-md-4 col-sm-4 col-xs-12">
        <div class="row">
          <?php if (!empty($content['body']) || !empty($content['body']) || !empty($content['body'])) {
            $mobile_class[1] = 'col-xs-12';
            $mobile_class[2] = 'col-xs-12';
          }
          else {
            $mobile_class[1] = 'col-xs-12';
            $mobile_class[2] = 'col-xs-12';
          }
          ?>
          <div class="issue-cover-image col-lg-12 col-md-12 col-sm-12 <?php print $mobile_class[1]; ?>">
            <?php print render($content['field_issue_image']); ?>
          </div>
          <div class="issue-notes-content col-lg-12 col-md-12 col-sm-12 <?php print $mobile_class[2]; ?>">
            <?php print render($content['body']); ?>
            <?php print render($content_sidebar_left); ?>
            <?php print render($content_sidebar_right); ?>
          </div>
        </div>
    </div>
    <div class="issue-contents col-lg-8 col-md-8 col-sm-8 col-xs-12">
  <?php else: ?>
    <div class="issue-contents col-lg-12 col-md-12 col-sm-12 col-xs-12">
  <?php endif; ?>
    <?php print render($content['field_issue_section']); ?>
  </div>
</div>
