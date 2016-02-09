<div id="print-header" class="clearfix">
  <div class="print-site-name">
    <?php print $site_name; ?>
  </div>
  <div class="print-site-logo">
    <?php print $print_logo; ?>
  </div>
</div>

<?php if (variable_get('custom_white_logo') && variable_get('custom_black_logo')): ?>
  <?php
    // Load logo files
    $custom_logo = array(
      'white' => file_load(variable_get('custom_white_logo')),
      'black' => file_load(variable_get('custom_black_logo')),
    );
    // Create img markup
    $custom_logo['white']->markup = '<img class="custom-logo custom-logo-white" src="' . file_create_url($custom_logo['white']->uri) . '" alt="' . check_plain($vars['site_name']) . '" />';
    $custom_logo['black']->markup = '<img class="custom-logo custom-logo-black" src="' . file_create_url($custom_logo['black']->uri) . '" alt="' . check_plain($vars['site_name']) . '" />';
  ?>

  <div<?php print $attributes; ?>>
    <div<?php print $content_attributes; ?>>
      <div class="branding-data clearfix">
        <?php

          // Link images to <front> and add site name
          print l($custom_logo['white']->markup, '<front>', array('attributes' => array('rel' => 'home', 'title' => check_plain($vars['site_name'])), 'html' => TRUE));

          print l($custom_logo['black']->markup, '<front>', array('attributes' => array('rel' => 'home', 'title' => check_plain($vars['site_name'])), 'html' => TRUE));
        ?>
        <div class="element-invisible">
          <?php if ($is_front): ?>
            <h1 class="site-title<?php print $class; ?>"><?php print $linked_site_name; ?></h1>
          <?php else: ?>
            <h2 class="site-title<?php print $class; ?>"><?php print $linked_site_name; ?></h2>
          <?php endif; ?>
          <?php if ($site_slogan): ?>
          <div class="site-slogan<?php print $class; ?>"><?php print $site_slogan; ?></div>
          <?php endif; ?>
        </div>
      </div>
    </div>
  </div>
<?php else: ?>


<div<?php print $attributes; ?>>
  <div<?php print $content_attributes; ?>>
    <div class="branding-data clearfix">
      <?php if (isset($linked_logo_img)): ?>
      <div class="logo-img">
        <?php print $linked_logo_img; ?>
      </div>
      <?php endif; ?>
      <?php if ($site_name || $site_slogan): ?>
      <?php $class = $site_name_hidden && $site_slogan_hidden ? ' element-invisible' : ''; ?>
      <div class="site-name-slogan<?php print $class; ?>">
        <?php $class = $site_name_hidden && !$site_slogan_hidden ? ' element-invisible' : ''; ?>
        <?php if ($is_front): ?>
        <h1 class="site-title<?php print $class; ?>"><?php print $linked_site_name; ?></h1>
        <?php else: ?>
        <h2 class="site-title<?php print $class; ?>"><?php print $linked_site_name; ?></h2>
        <?php endif; ?>
        <?php $class = ($site_slogan_hidden && !$site_name_hidden ? ' element-invisible' : ''); ?>
        <?php if ($site_slogan): ?>
        <div class="site-slogan<?php print $class; ?>"><?php print $site_slogan; ?></div>
        <?php endif; ?>
      </div>
      <?php endif; ?>
    </div>
    <?php print $content; ?>
  </div>
</div>
<?php endif; ?>
