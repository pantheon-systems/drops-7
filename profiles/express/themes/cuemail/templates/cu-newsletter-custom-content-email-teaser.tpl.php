<?php
  if (!empty($content['image'])) {
    $content['image'][0]['#image_style'] = 'email_teaser_thumbnail';
  }
?>
<table class="row article-content" role="presentation">
  <tr>
    <td class="wrapper last">
      <table class="twelve columns" role="presentation">
        <tr>
          <?php if (!empty($content['image'])): ?>
          <td class="three sub-columns text-pad padding-bottom">
            <?php if(!empty($content['link'])): ?>
              <a href="<?php print $content['link']; ?>"><?php print render($content['image']); ?></a>
            <?php else: ?>
              <?php print render($content['image']); ?>
            <?php endif; ?>
          </td>
          <td class="nine sub-columns text-pad padding-bottom">
          <?php else: ?>
          <td class="twelve sub-columns text-pad padding-bottom">
          <?php endif; ?>
            <h3 class="teaser-title"><?php print render($content['title']); ?></h3>
            <?php print render($content['body']); ?>

          </td>
          <td class="expander"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
<div class="border border-inset"></div>
