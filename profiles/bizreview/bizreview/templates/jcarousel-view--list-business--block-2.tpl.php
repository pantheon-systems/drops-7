<?php

/**
 * @file jcarousel-view.tpl.php
 * View template to display a list as a carousel.
 */
?>
<ul class="<?php print $jcarousel_classes; ?>">
  <?php $i=0; $rows_number=2; ?>
  <?php foreach ($rows as $id => $row): ?>
    <?php if($i%$rows_number==0) : ?>
      <li class="<?php print $row_classes[$id]; ?>">
	<?php endif; ?>
	<?php print $row; ?>
    <?php if($i%$rows_number==($rows_number-1)) : ?>
      </li>
	<?php endif; ?>
    <?php $i++; ?>
  <?php endforeach; ?>
  
    <?php if($i%$rows_number!=0) : ?>
      </li>
	<?php endif; ?>
</ul>
