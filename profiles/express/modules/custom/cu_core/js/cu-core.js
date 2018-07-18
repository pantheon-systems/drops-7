(function ($) {
  $(document).ready(function(){
    $('#navbar-bar').prepend('<div class="express-environment-indicator"><span class="express-environment-indicator-label"></span> <a href="' + Drupal.settings.basePath + 'admin/settings/site-status/verify">Migration Status</a></div>');
    
    // Prod New Launched
    $('.environment-production.infrastructure-new.status-launched .express-environment-indicator .express-environment-indicator-label').text('Live Site  - New Server Environment');
    // Prod New Staged
    $('.environment-production.infrastructure-new.status-staged .express-environment-indicator-label').text('Staged Site  - New Server Environment');
    // Prod Old Launched
    $('.environment-production.infrastructure-old.status-launched .express-environment-indicator-label').text('Live Site  - Old Server Environment');
    // Prod Old Launched
    $('.environment-production.infrastructure-old.status-staged .express-environment-indicator-label').text('Temporary Staging Site  - Old Server Environment');

    // Test Launched
    $('.environment-test.status-launched .express-environment-indicator-label').text('Testing Site');
    $('.environment-test.status-staged .express-environment-indicator-label').text('Temporary Training Site');
  });
}(jQuery));
