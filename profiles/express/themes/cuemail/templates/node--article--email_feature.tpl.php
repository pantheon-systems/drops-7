<?php hide($content['field_tags']); ?>
<?php hide($content['article_tags']); ?>

<table class="row article-content article-feature <?php if (!empty($elements['zebra'])) { print $elements['zebra']; } ?> <?php if (!empty($elements['#order_class'])) { print $elements['#order_class']; } ?>" role="presentation">
  <tr>
    <td class="wrapper last">
      <table class="twelve columns" role="presentation">
        <tr>
          <td class="text-pad padding-bottom">
            <?php if(!empty($content['field_article_thumbnail'])): ?>

              <table role="presentation">
                <td class="padding-bottom">
                  <?php print render($content['field_article_thumbnail']); ?>
                </td>
              </table>
            <?php endif; ?>
            <div class="content-padding">
              <h3 class="feature-title"><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
              <?php if (!empty($content['field_article_categories'])): ?>
                <table role="presentation">
                  <td class="tags">
                    <?php print render($content['field_article_categories']); ?>
                  </td>
                </table>
              <?php endif; ?>
              <?php print render($content['body']); ?>

            </div>
          </td>
          <td class="expander"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
<div class="border border-inset article-feature-border <?php if (!empty($elements['#order_class'])) { print $elements['#order_class']; } ?>"></div>
