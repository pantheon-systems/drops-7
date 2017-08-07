<?php if (!empty($content['field_newsletter_section_title'])): ?>
<table class="row article-section-title" role="presentation">
  <tr>
    <td class="wrapper last">
      <table class="twelve columns" role="presentation">
        <tr>
          <td class="text-pad padding-bottom">
            <?php print render($content['field_newsletter_section_title']); ?>
          </td>

          <td class="expander"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
<?php endif; ?>
<?php print render($content['newsletter_content']); ?>
<table class="row article-section-link" role="presentation">
  <tr>
    <td class="wrapper last">
      <table class="twelve columns" role="presentation">
        <tr>
          <td class="text-pad padding-bottom">
            <?php print render($content['field_newsletter_more_link']); ?>
          </td>

          <td class="expander"></td>
        </tr>
      </table>
    </td>
  </tr>
</table>
