<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <link type="text/css" rel="stylesheet" href="https://api.jirafe.com/dashboard/css/magento_ui.css" media="all" />
  <script type="text/javascript" src="https://api.jirafe.com/dashboard/js/magento_ui.js"></script>
</head>
<body>
<div id="jirafe"></div>

<script type="text/javascript">
(function($) {
  $(function() {
    $('#jirafe').jirafe(<?php echo drupal_json_encode($data) ?>);
    setTimeout(function() { if ($('mod-jirafe') == undefined){ $('messages').insert ("<ul class=\"messages\"><li class=\"error-msg\">We're unable to connect with the Jirafe service for the moment. Please wait a few minutes and refresh this page later.</li></ul>"); } }, 4000);
  });
  setInterval(function() {
    $('iframe', window.parent.document).css('height', document.body.scrollHeight);
  }, 100);
})(jQuery);
</script>
</body>
</html>
