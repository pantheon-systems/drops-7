<?php
  if (!$column_classes) {
    $column_classes = array(
      'main' => 'col-lg-12 col-md-12 col-sm-12 col-xs-12',
      'sidebar' => 'col-lg-12 col-md-12 col-sm-12 col-xs-12',
    );
  }
?>

<div class="newsletter-wrapper row clearfix">

  <div class="newsletter-main <?php print($column_classes['main']) ?>">
    <?php
      if (!empty($content['field_newsletter_intro_image'])): ?>
      <div class="newsletter-hero">
        <?php
        $content['field_newsletter_intro_image'][0]['#image_style'] = 'email_large';
        print render($content['field_newsletter_intro_image']);
        ?>
      </div>

    <?php endif; ?>
    <?php if (!empty($content['body'][0]['#markup'])): ?>
      <div class="newsletter-intro">
        <?php print render($content['body']); ?>
      </div>
    <?php endif; ?>
    <?php print render($content['field_newsletter_section']); ?>
  </div>
  <?php if (!empty($column_classes['sidebar'])): ?>
    <div class="newsletter-sidebar <?php print $column_classes['sidebar']; ?>">
      <div class="newsletter-ad-promo-wrapper">
        <div class="row clearfix">
          <?php if (!empty($content['field_newsletter_ad_promo'][0])): ?>
            <?php
              print render($content['field_newsletter_ad_promo'][0]);
            ?>
          <?php endif; ?>
          <?php if (!empty($content['field_newsletter_ad_promo'][1])): ?>
            <?php
              print render($content['field_newsletter_ad_promo'][1]);
            ?>
          <?php endif; ?>
        </div>

        <?php if (!empty($content['field_newsletter_text_block'])): ?>
          <div class="newsletter-text-blocks clearfix row">
            <?php
              $blocks = array_intersect_key($content['field_newsletter_text_block'], element_children($content['field_newsletter_text_block']));

              $block_wrapper_class = count($blocks) > 1 ? 'col-lg-6 col-md-6 col-sm-6 col-xs-12' : 'col-lg-12 col-md-12 col-sm-12 col-xs-12';

              foreach ($blocks as $block) {
                print theme('cu_newsletter_block', array('content' => $block, 'block_wrapper_class' => $block_wrapper_class));
              }
             ?>
          </div>
        <?php endif; ?>

      </div>

      <?php  ?>
    </div>
  <?php endif; ?>
</div>
