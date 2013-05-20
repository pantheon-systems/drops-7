<div class="marketplace-addon-licenses">
  <?php foreach ($groups as $group): ?>
    <div class="addon-category">
      <h2> <?php print $group['name']; ?> </h2>
      <?php foreach ($group['addons'] as $addon) : ?>
        <div class="marketplace-addon-license">
          <?php print $addon; ?>
        </div>
      <?php endforeach; ?>
    </div>
  <?php endforeach; ?>
</div>
