<?php
  $link_color = array();
  $link_color['dark'] = '#cfb87c';
  $link_color['white'] = '#007DBB';  
  $color = $link_color[$content['field_twitter_style'][0]['#markup']];
  $number = $content['field_twitter_block_items'][0]['#markup'] ? 'data-tweet-limit="' . $content['field_twitter_block_items'][0]['#markup'] . '"' : '';
?>
<a <?php print $number; ?> data-link-color="<?php print $color; ?>" data-theme="<?php print $content['field_twitter_style'][0]['#markup']; ?>" data-screen-name="<?php print $content['field_twitter_block_user'][0]['#markup']; ?>" class="twitter-timeline" href="https://twitter.com/<?php print $content['field_twitter_block_user'][0]['#markup']; ?>" data-widget-id="360845078434041856">Tweets by @<?php print $content['field_twitter_block_user'][0]['#markup']; ?></a>
<script>!function (d,s,id) {var js,fjs=d.getElementsByTagName(s)[0],p=/^http:/.test(d.location)?'http':'https';if (!d.getElementById(id)) {js=d.createElement(s);js.id=id;js.src=p+"://platform.twitter.com/widgets.js";fjs.parentNode.insertBefore(js,fjs);}}(document,"script","twitter-wjs");</script>
