<table class="row article-intro">
  <tr>
    <td class="wrapper last">
      <table class="twelve columns">
        <tr>
          <td class="text-pad padding-top padding-bottom">


            <div class="content-padding">
              <div class="intro-image">
                <table>
                  <td class="padding-bottom">
                    <?php print render($content['field_newsletter_intro_image']); ?>
                  </td>
                </table>
              </div>
              <?php if (!empty($content['body'])): ?>
                <div class="intro-text">
                  <?php print render($content['body']); ?>
                </div>
              <?php endif; ?>
            </div>
          </td>
          <td class="expander"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
