<?php

/**
 * @file
 * Template for recline embed button.
 *
 * Variables:
 * - $embed_url: the url of the rendered content to be embedded.
 */
?>
<div class="recline-embed">
  <a class="embed-link" href="#"><?php print t('Embed'); ?></a>
  <div class="embed-code-wrapper">
    <textarea class="embed-code" style="height: 75px;width:100%;" onclick="select()"><?php print $embed_url ?></textarea>
    <br>
    <small>Data Preview URL</small>
    <textarea class="preview-code" style="height: 75px;width:100%;max-width:100%" onclick="select()"><?php print $embed_url ?></textarea>
    <br><br>
  </div>
</div>