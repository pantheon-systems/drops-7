<div class="person-view-mode-teaser clearfix">
  <?php if(!empty($content['field_person_photo'])): ?>
    <?php print render($content['field_person_photo']); ?>
  <?php endif; ?>
  <div class="person-view-mode-teaser-content">
    <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
    <div class="person-job-titles"><?php print $content['job_titles']; ?></div>
    <div class="person-departments"><?php print $content['departments']; ?></div>
    <?php if(!empty($content['body'])): ?>
      <div class="people-bio">
        <?php print render($content['body']); ?>
      </div>
    <?php endif; ?>
    <?php
      $person_contact = array();
      if (!empty($content['field_person_email'])) {
        $person_contact[] = '<span class="person-contact-info-item person-contact-info-email"><i class="fa fa-envelope"></i> ' . render($content['field_person_email']) . '</span>';
      }
      if (!empty($content['field_person_phone'])) {
        $person_contact[] = '<span class="person-contact-info-item person-contact-info-phone"><i class="fa fa-phone"></i> ' . render($content['field_person_phone']) . '</span>';
      }
      $contact = join(' <span class="person-contact-separator">&nbsp;&nbsp;&nbsp;</span> ' , $person_contact);
    ?>
    <?php if (!empty($person_contact)): ?>
      <div class="person-view-mode-teaser-contact"><?php print $contact; ?></div>
    <?php endif; ?>

  </div>
</div>
