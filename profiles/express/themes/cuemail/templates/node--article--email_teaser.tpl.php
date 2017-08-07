<?php hide($content['field_tags']); ?>
<?php hide($content['article_tags']); ?>

<table class="row article-content article-teaser <?php if (!empty($elements['zebra'])) { print $elements['zebra']; } ?> <?php if (!empty($elements['#order_class'])) { print $elements['#order_class']; } ?>" role="presentation">
  <tr>
    <td class="wrapper last">
      <table class="twelve columns" role="presentation">
        <tr>
          <?php if (!empty($content['field_article_thumbnail'])): ?>
          <td class="three sub-columns text-pad padding-bottom">
            <?php print render($content['field_article_thumbnail']); ?>
          </td>
          <td class="nine sub-columns text-pad padding-bottom">
          <?php else: ?>
          <td class="twelve sub-columns text-pad padding-bottom">
          <?php endif; ?>
            <h3 class="teaser-title"><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h3>
            <?php if (!empty($content['field_article_categories'])): ?>
              <table role="presentation">
                <td class="tags">
                  <?php print render($content['field_article_categories']); ?>
                </td>
              </table>
            <?php endif; ?>
            <?php print render($content['body']); ?>

          </td>
          <td class="expander"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
<div class="border border-inset article-teaser-border <?php if (!empty($elements['#order_class'])) { print $elements['#order_class']; } ?>"></div>
