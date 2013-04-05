<article<?php print $attributes; ?>>
  <div<?php print $content_attributes; ?>>
    <?php
      // We hide the comments and links.
      hide($content['comments']);
      hide($content['links']);
      print render($content);
    ?>
  </div>
</article>
