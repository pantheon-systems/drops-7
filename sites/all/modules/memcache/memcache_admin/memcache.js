// $Id: memcache.js,v 1.1.4.2 2010/08/25 16:29:23 Jeremy Exp $

// Global Killswitch
if (Drupal.jsEnabled) {
$(document).ready(function() {
    $("body").append($("#memcache-devel"));
  });
}
