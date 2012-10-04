<div id="app-wrapper" class="clearfix">
  <div id="sidebar">
    <h2><?php print $author_title; ?></h2>
    <div class = 'app-author'><?php print $author ?></div>
    
    <h2><?php print $version_title; ?></h2>
    <div class = 'app-version'><?php print $version ?></div>
    
    <div class="divider"></div>
    
    <!--Ratings-->
    <h2><?php print $rating_title; ?></h2>
    <h3><?php print $rating_caption; ?></h3>
    <?php print drupal_render($rating); ?>
  </div>
  <div class="app-main">
    <div id="app-top">
      <?php if($logo): ?>
        <div class = 'app-logo-small'>
          <?php print $logo ?>
        </div>
      <?php endif; ?>
      <h1><?php print $name ?></h1>
    </div>
    
    <div class = 'app-description'>
      <h2><?php print $description_title; ?></h2>
      <?php print $description ?>
    </div>
    <?php if($screenshot): ?>
      <div class = 'app-screenshot'><?php print $screenshot ?><div class="screenshot-shadow"></div></div>
    <?php endif; ?>
  </div>
</div>
 




