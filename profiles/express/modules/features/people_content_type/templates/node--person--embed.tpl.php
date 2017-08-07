<a name="<?php print urlencode(strtolower($title)); ?>" id="<?php print urlencode(strtolower($title)); ?>"></a><div class="person-view-mode-embed node-view-mode-embed clearfix">
  <?php if(!empty($content['field_person_photo'][0])): ?>
    <?php print render($content['field_person_photo'][0]); ?>
  <?php endif; ?>
  <div class="person-view-mode-embed-content node-view-mode-embed-content">
    <h3<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
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
        $person_contact[] = '<i class="fa fa-envelope"></i> ' . render($content['field_person_email']);
      }
      if (!empty($content['field_person_phone'])) {
        $person_contact[] = '<i class="fa fa-phone"></i> ' . render($content['field_person_phone']);
      }
      $contact = join(' &nbsp;&nbsp;&nbsp; ' , $person_contact);
    ?>
    <?php if (!empty($person_contact)): ?>
      <div class="person-view-mode-embed-contact"><?php print $contact; ?></div>
    <?php endif; ?>
  </div>
</div>
