<?php
  $block_total = count($content['field_newsletter_text_block']['#items']);
?>
<?php if ($block_total > 1): ?>
  <?php
    $blocks = $content['field_newsletter_text_block'];
    $columns = 2;
    $chunks = array_chunk($blocks['#items'], $columns, true);
  ?>
  <?php foreach ($chunks as $key => $chunk): ?>
    <table class="row blocks" role="presentation">
      <tr>
        <?php foreach ($chunk as $key => $block): ?>
          <td class="wrapper <?php if ($key == 1) { print 'last'; } ?>">
            <table class="six columns" role="presentation">
              <tr>
                <td class="text-pad padding-bottom">
                  <?php print render($content['field_newsletter_text_block'][$key]); ?>
                </td>
                <td class="expander"></td>
              </tr>
            </table>
          </td>
        <?php endforeach; ?>
      </tr>
    </table>
  <?php endforeach; ?>
<?php else: ?>
  <table class="row blocks" role="presentation">
    <tr>
      <td class="wrapper last">
        <table class="twelve columns" role="presentation">
          <tr>
            <td class="text-pad padding-bottom">
              <?php print render($content['field_newsletter_text_block']); ?>
            </td>
            <td class="expander"></td>
          </tr>
        </table>
      </td>
    </tr>
  </table>
<?php endif; ?>
