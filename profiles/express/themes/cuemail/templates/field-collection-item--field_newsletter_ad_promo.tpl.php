<?php
  $img = render($content['field_newsletter_ad_image']);

  if (!empty($content['field_newsletter_ad_link'])) {
    $url = $content['field_newsletter_ad_link'][0]['#element']['url'];
    $ad = l($img, $url, array('html' => TRUE));
  }
  else {
    $ad = $img;
  }
?>
<?php print $ad; ?>
