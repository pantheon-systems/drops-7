<?php hide($content['links']); ?>
<div class="person-photo"><?php print render($content['field_person_photo']); ?></div>

<?php
  // Add check for $person_title due to warning notice.
  if (!isset($person_title)) {
    $person_title = array();
  }
  if (!empty($content['field_person_title'])) {
    $person_title[] = render($content['field_person_title']);
  }
  if (!empty($content['field_person_department']['#items'])) {
    $person_title[] = render($content['field_person_department']);
  }
  $title = join(' &bull; ' , $person_title);
  //print $title;
?>
<div class="person-job-titles"><?php print $content['job_titles']; ?></div>
<div class="person-departments"><?php print $content['departments']; ?></div>
<?php if(!empty($content['field_person_email']) || !empty($content['field_person_phone'])): ?>
<div class="people-contact people-section">
  <?php if(!empty($content['field_person_email'])): ?>
    <div class="person-email person-contact-info-item"><i class="fa fa-envelope"></i> <?php print render($content['field_person_email']); ?></div>
  <?php endif; ?>
  <?php if(!empty($content['field_person_phone'])): ?>
    <div class="person-phone person-contact-info-item"><i class="fa fa-phone"></i> <?php print render($content['field_person_phone']); ?></div>
  <?php endif; ?>
</div>
<?php endif; ?>

<?php if(!empty($content['field_person_website'])): ?>
<div class="people-links people-section">
  <?php print render($content['field_person_website']); ?>
</div>
<?php endif; ?>

<?php if(!empty($content['field_person_address']) || !empty($content['field_person_office_hours'])): ?>
<div class="people-office people-section">
  <?php if(!empty($content['field_person_address'])): ?>
    <?php print render($content['field_person_address']); ?>
  <?php endif; ?>
  <?php if(!empty($content['field_person_office_hours'])): ?>
    <?php print render($content['field_person_office_hours']); ?>
  <?php endif; ?>
</div>
<?php endif; ?>


<?php if(!empty($content['body'])): ?>
  <div class="people-bio">
    <?php print render($content['body']); ?>
  </div>
<?php endif; ?>
