(function ($) {
  $(document).ready(function(){
    // @TODO: add site's state to Drupal.settings to we can show/not show the migrate button via js 
    $('#navbar-bar').prepend('<div class="express-environment-indicator"><span class="express-environment-indicator-label"></span> </div>');
    
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
