<a name="<?php print urlencode(strtolower($title)); ?>" id="<?php print urlencode(strtolower($title)); ?>"></a>
<strong><a href="<?php print $node_url; ?>"><?php print $title; ?></a></strong>
<div class="person-job-titles">
  <?php print $content['job_titles']; ?>
</div>
<div class="person-departments">
  <?php print $content['departments']; ?>
</div>
