<?php if(!drupal_is_front_page()): ?>
<div class="page-title-wrapper">
  <div class="page-title-section container-12 clearfix">
    <div id="page_title" class="grid-12">
      <h1 class="title" id="page-title"><?php print drupal_get_title(); ?></h1>
    <?php if ($breadcrumb): ?>
      <div id="breadcrumb"><?php print $breadcrumb; ?></div>
    <?php endif; ?>
    </div>
  </div>
</div>
<?php endif; ?> 

<?php if ($wrapper): ?><div<?php print $attributes; ?>><?php endif; ?>  
  <div<?php print $content_attributes; ?>>    
       
    <?php if ($messages): ?>
      <div id="messages" class="grid-<?php print $columns; ?>"><?php print $messages; ?></div>
    <?php endif; ?>
    <?php print $content; ?>
  </div>
<?php if ($wrapper): ?></div><?php endif; ?>