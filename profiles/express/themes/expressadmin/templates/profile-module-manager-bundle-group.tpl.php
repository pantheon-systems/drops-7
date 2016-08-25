<?php
/* Variables:
 * $group : group_bundle or group_admin_bundle
 * $label : bundle label
 * $bundles : bundles in group
 */

 // Set Bundle label
 $label  = ($group == 'group_admin_bundle') ? 'Admin Bundles' : 'Bundles';
?>

<div class="admin-panel1 clearfix">
  <h2><?php print $label; ?></h2>
  <div class="body1">
    <div class="admin-list1 block-column-container row clearfix">
    <?php
      foreach ($bundles as $bundle) {
        print render($bundle);
      }
    ?>
  </div>
  </div>
</div>
