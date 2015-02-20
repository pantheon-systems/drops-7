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
    <textarea class="embed-code" style="height: 25px;" onclick="select()"><?php print $embed_url ?></textarea>
  </div>
</div>
