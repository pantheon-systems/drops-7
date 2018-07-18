(function ($) {

Drupal.devel_debug_log = {};
Drupal.devel_debug_log.active = 1;

Drupal.behaviors.devel_debug_log = {
  attach: function (context) {
    setTimeout("Drupal.devel_debug_log.refresh()", 5000);
    
    $('#devel-debug-log-messages-table').click(function () {
      Drupal.devel_debug_log.active = 0;
    });
  }
};

Drupal.devel_debug_log.refresh = function() {
  if (Drupal.devel_debug_log.active == 1) {
    $.ajax({
      url: '/devel-debug-log/callback',
      dataType: 'json',
      success: function(data) {
        $('#devel-debug-log-messages-table').html(data['content']);
      }
    });
  
    setTimeout("Drupal.devel_debug_log.refresh()", 5000);
  }
}

})(jQuery);
