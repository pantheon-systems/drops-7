<?php
  $label = trim(render($variables['expandable']['label']));
?>

<div id="<?php print cu_expandable_system_label($label); ?>-<?php print $variables['expandable']['bid']; ?>" class="expandable-tabcontent expandable-panel">
  <?php print render($variables['expandable']['text']); ?>
  <?php
    if (!empty($variables['expandable']['block'])) {
      foreach ($variables['expandable']['block']['#items'] as $block) {
        $block = cu_expandable_bean_render($block['target_id'], $variables['expandable']['block_title']);
        print render($block);
      }
    }
  ?>
</div>
