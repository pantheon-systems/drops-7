<!-- Facebook Pixel Code -->
<script>
  !function (f, b, e, v, n, t, s) {
    if (f.fbq)return;
    n = f.fbq = function () {
      n.callMethod ?
        n.callMethod.apply(n, arguments) : n.queue.push(arguments)
    };
    if (!f._fbq)f._fbq = n;
    n.push = n;
    n.loaded = !0;
    n.version = '2.0';
    n.queue = [];
    t = b.createElement(e);
    t.async = !0;
    t.src = v;
    s = b.getElementsByTagName(e)[0];
    s.parentNode.insertBefore(t, s)
  }(window,
    document, 'script', '//connect.facebook.net/en_US/fbevents.js');

  fbq('init', '<?php print check_plain($variables['account_id']); ?>');
  fbq('track', 'PageView');
  <?php if (!empty($variables['action'])): ?>
  fbq('track', '<?php print check_plain($variables['action']); ?>');
  <?php endif; ?>
</script>
<noscript><img height="1" width="1" style="display:none"
               src="https://www.facebook.com/tr?id=<?php print check_plain($variables['account_id']); ?>&ev=PageView&noscript=1"/>
</noscript>
<!-- End Facebook Pixel Code -->
