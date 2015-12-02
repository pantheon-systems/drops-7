<?php

/**
 * @file recaptcha-widget-noscript.tpl.php
 * Default theme implementation to present the reCAPTCHA noscript code.
 *
 * Available variables:
 * - $sitekey: Google web service site key.
 * - $language: Current site language code.
 * - $url: Google web service API url.
 *
 * @see template_preprocess()
 * @see template_preprocess_recaptcha_widget_noscript()
 */
?>
<noscript>
  <div style="width: 302px; height: 352px;">
    <div style="width: 302px; height: 352px; position: relative;">
      <div style="width: 302px; height: 352px; position: absolute;">
        <iframe src="<?php print $url; ?>" frameborder="0" scrolling="no" style="width: 302px; height:352px; border-style: none;"></iframe>
      </div>
      <div style="width: 250px; height: 80px; position: absolute; border-style: none; bottom: 21px; left: 25px; margin: 0px; padding: 0px; right: 25px;">
        <textarea id="g-recaptcha-response" name="g-recaptcha-response" class="g-recaptcha-response" style="width: 250px; height: 80px; border: 1px solid #c1c1c1; margin: 0px; padding: 0px; resize: none;" value=""></textarea>
      </div>
    </div>
  </div>
</noscript>
