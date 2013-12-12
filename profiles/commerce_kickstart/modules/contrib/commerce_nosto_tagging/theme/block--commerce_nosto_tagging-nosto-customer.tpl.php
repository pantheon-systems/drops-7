<?php

/**
 * @file
 * Block template for Nosto customer tagging.
 *
 * Note that the username is put as "lastname"
 * and the "firstname" is left out.
 *
 * Available variables:
 * - $user: The user object.
 *
 * @see commerce_nosto_tagging_block_view()
 * @see commerce_nosto_tagging_theme()
 */
?>

<?php if (isset($user) && is_object($user)): ?>
  <div class="nosto_customer" style="display:none">
    <span class="email"><?php print check_plain($user->mail); ?></span>
    <span class="last_name"><?php print check_plain($user->name); ?></span>
  </div>
<?php endif; ?>
