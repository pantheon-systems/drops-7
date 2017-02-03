<?php

/**
 * @file
 * CU Alerts template.
 */

?>
<div id="cu-alerts" data-remote-url="<?php print $remote_url ?>">
<?php if (!empty($data)) : ?>
  <?php foreach ($data as $alert) : ?>
    <div class="alert">
      <?php print $alert->title; ?>
      <?php if (!empty($alert->alert_url)) : ?>
        <?php print l('Read More &raquo;', $alert->alert_url, array('html' => TRUE)); ?>
      <?php endif; ?>
    </div>
  <?php endforeach; ?>
<?php endif; ?>
</div>
