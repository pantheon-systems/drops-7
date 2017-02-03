<?php

/**
 * @file
 * Template for visualization embed button.
 *
 * Variables:
 * - $embed_url: the url of the rendered content to be embedded.
 */
?>
<div class="visualization-embed">
  <a class="embed-link" href="#embed-wrapper"><?php print t('Embed'); ?></a>
  <div id="embed-wrapper" class="embed-code-wrapper">
    <form>
	    <div class="form-group">
		    <label for="embed-width">Width</label>
		    <input id="embed-width" type="text" class="form-control"/>
		    <label for="embed-height">Height</label>
		    <input id="embed-height" type="text" class="form-control"/>
		    <label for="embed-code">Embed code</label>
		    <textarea id="embed-code" class="embed-code" style="height: 25px;" onclick="select()"><iframe width="960" height="600" src="<?php print $embed_url ?>" frameborder="0"></iframe></textarea>
	  	</div>
  	</form>
  </div>
</div>
