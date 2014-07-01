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
    <textarea class="embed-code" style="height: 25px;"><iframe width="700" height="400" src="<?php print $embed_url ?>" frameborder="0"></iframe></textarea>
  </div>
</div>
