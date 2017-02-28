<?php
  if (!empty($content['image'])) {
    $content['image'][0]['#image_style'] = 'email_feature_thumbnail';
  }
?>
<table class="row article-content" role="presentation">
  <tr>
    <td class="wrapper last">
      <table class="twelve columns">
        <tr>
          <td class="text-pad padding-bottom">
            <?php if(!empty($content['image'])): ?>
              <table role="presentation">
                <td class="padding-bottom">
              <?php if(!empty($content['link'])): ?>
                <a href="<?php print $content['link']; ?>"><?php print render($content['image']); ?></a>
              <?php else: ?>
                <?php print render($content['image']); ?>
              <?php endif; ?>


                </td>
              </table>
            <?php endif; ?>
            <div class="content-padding">
              <h3 class="feature-title"><?php print render($content['title']); ?></h3>
              <?php print render($content['body']); ?>
            </div>
          </td>
          <td class="expander"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
<div class="border border-inset"></div>
