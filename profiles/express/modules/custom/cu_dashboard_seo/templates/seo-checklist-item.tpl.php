<?php
if (isset($indicator) && $indicator) {
  $icon = '<i class="fa fa-check"></i>';
}
else {
  $icon = '<i class="fa fa-times"></i>';
}
?>
<div class="seo-checklist-item">
  <h3><?php print $icon; ?><?php print $title; ?></h3>
  <div class="seo-checklist-description">
    <p>
      <?php
      if (isset($indicator) && $indicator) {
        print $description[1];
      }
      else {
        print $description[0];
      }
      ?>
    </p>
    <?php
    if (isset($edit) && $edit) {
      if (is_array($path)) {
        print l($edit, $path['link'], array(
          'attributes' => array(
            'class' => array(
              'btn',
              'btn-sm',
              'btn-info'
            ),
          ),
          'fragment' => $path['fragment'],
          'query' => $path['query'],
        ));
      }
      else {
        print l($edit, $path, array(
          'attributes' => array(
            'class' => array(
              'btn',
              'btn-sm',
              'btn-info'
            )
          )
        ));
      }

    }
    ?>
  </div>
</div>
