<?php

/**
 * @file
 * RAVE Alerts block template.
 */
?>
<div id="rave-alerts" data-remote-url="<?php print $remote_url ?>">
  <?php if (variable_get('rave_alerts_enable', 1) && variable_get('rave_alerts_display', 1)): ?>
    <div class="alert">
      <?php print $data['channel']['item']['description']; ?>
      <?php if (!empty($data['channel']['item']['link'])) : ?>
        <?php print l('Read More &raquo;', $data['channel']['item']['link'], array('html' => TRUE)); ?>
      <?php elseif ($rave_alerts_site = variable_get('rave_alerts_deafult_read_more_url', NULL)): ?>
        <?php print l('Read More &raquo;', $rave_alerts_site, array('html' => TRUE)); ?>
      <?php endif; ?>
    </div>
  <?php endif; ?>
</div>
