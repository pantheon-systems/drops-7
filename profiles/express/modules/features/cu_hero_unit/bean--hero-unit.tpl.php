<?php if ($priority == 'text'): ?>
  <div id="hero-<?php print $bid; ?>" class="<?php print $hero_classes; ?>">
    <div class="hero-unit-content-wrapper">
      <div class="hero-unit-content">
        <div class="hero-unit-content-inner">
        <h2><?php print render($content['field_hero_unit_headline']); ?></h2>
        <?php if(!empty($content['field_hero_unit_text'])): ?>
          <p><?php print render($content['field_hero_unit_text']); ?></p>
        <?php endif; ?>
        <?php if(!empty($content['field_hero_unit_link'])): ?>
          <div class="hero-unit-links clearfix"><?php print render($content['field_hero_unit_link']); ?></div>
        <?php endif; ?>
      </div>
      </div>
    </div>
  </div>
  <?php if(!empty($content['field_hero_unit_image'])): ?>
  <style>
    #hero-<?php print $bid; ?> {
      background-image:url(<?php print $image_small; ?>);
    }
    @media all and (min-width: 480px) and (max-width: 959px) {
      #hero-<?php print $bid; ?> {
        background-image:url(<?php print $image_medium; ?>);
      }
    }
    @media all and (min-width: 960px) {
      #hero-<?php print $bid; ?> {
        background-image:url(<?php print $image_large; ?>);
      }
    }
  </style>
  <?php endif; ?>
<?php else: ?>
  <div class="<?php print $hero_classes; ?>">
    <img src="<?php print $image; ?>" alt=" " />
    <div class="hero-unit-image-content-wrapper clearfix">
      <div class="hero-unit-content-wrapper">
        <div class="hero-unit-content">
          <div class="hero-unit-content-inner">
          <h2><?php print render($content['field_hero_unit_headline']); ?></h2>
          <?php if(!empty($content['field_hero_unit_text'])): ?>
            <p><?php print render($content['field_hero_unit_text']); ?></p>
          <?php endif; ?>
          <?php if(!empty($content['field_hero_unit_link'])): ?>
            <div class="hero-unit-links clearfix"><?php print render($content['field_hero_unit_link']); ?></div>
          <?php endif; ?>
        </div>
        </div>
      </div>
    </div>
  </div>
<?php endif; ?>
