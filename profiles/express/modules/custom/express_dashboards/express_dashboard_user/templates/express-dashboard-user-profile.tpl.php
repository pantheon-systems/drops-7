<div class="express-dashboard-user-profile-wrapper">
  <p>
    User since: <?php print date('F j, Y', $user['created']); ?>.
  </p>
  <p>
    <small>Roles: <?php print join(', ', $user['roles']); ?></small>
  </p>
</div>
