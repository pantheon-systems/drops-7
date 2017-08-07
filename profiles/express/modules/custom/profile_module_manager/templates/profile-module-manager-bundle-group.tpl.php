<?php
/* Variables:
 * $group : group_bundle or group_admin_bundle
 * $label : bundle label
 * $bundles : bundles in group
 */

 // Set Bundle label
 $label  = ($group == 'group_admin_bundle') ? 'Admin Bundles' : 'Bundles';
?>

<div class="admin-panel">
  <h2><?php print $label; ?></h2>
  <div class="body">
    <ul class="admin-list">
    <?php
      foreach ($bundles as $bundle) {
        print '<li class="leaf">' . render($bundle) . '</li>';
      }
    ?>
    </ul>
  </div>
</div>
