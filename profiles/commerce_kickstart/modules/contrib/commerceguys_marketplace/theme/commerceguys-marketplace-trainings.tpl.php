<h3><?php print t('Upcoming trainings'); ?></h3>
<?php foreach ($trainings as $training): ?>
  <div class="marketplace-training">
    <a class="clearfix" href="<?php echo COMMERCEGUYS_MARKETPLACE_URL;?>/training">
    <div class="marketplace-training-event-title"><?php echo $training['title'];?></div>
    <?php foreach ($training['events'] as $event): ?>
      <div class="marketplace-training-event-content">
        <span class="t-audience"><?php print t('for') . ' ' . $event['audience']; ?></span>
        <span class="t-title"><?php print $event['title']; ?></span>
      </div>
    <?php endforeach; ?>
    </a>
  </div>
<?php endforeach; ?>
