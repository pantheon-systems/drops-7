<div class="expandable <?php print $expandable_classes; ?>" id="expandable-<?php print $variables['bean']->bid; ?>">
  <?php
    if ($content['display'] == 'select'):
  ?>
    <div class="expandable-select-prompt">
      <a href="#" aria-expanded="false" class="expandable-prompt" data-tabset="expandable-<?php print $variables['bean']->bid; ?>"><?php print $content['prompt']; ?></a>
    </div>
  <?php endif; ?>
  <div class="expandable-tab-group">
    <ul class="expandable-tablist" role="tablist">
      <?php foreach ($content['labels'] as $key => $label):?>
        <?php
          $label = trim(render($label));
        ?>
        <li class="expandable-tablist-item " role="presentation">
          <a href="#<?php print cu_expandable_system_label($label); ?>-<?php print $variables['bean']->bid; ?>" id="label_<?php print cu_expandable_system_label($label); ?>-<?php print $variables['bean']->bid; ?>" class="expandable-tablist-link" role="tab" data-expandable-panel="<?php print cu_expandable_system_label($label); ?>-<?php print $variables['bean']->bid; ?>" data-tabset="expandable-<?php print $variables['bean']->bid; ?>" tabindex="0"><?php print render($label); ?></a>
        </li>
      <?php endforeach; ?>

    </ul>
  </div>
  <div class="expandable-panel-group">

  <?php foreach ($content['sections'] as $section):?>
    <?php $section['bid'] = $variables['bean']->bid; ?>
    <?php print theme('expandable_section_tabs', array('expandable' => $section)); ?>
  <?php endforeach; ?>
  </div>
</div>
