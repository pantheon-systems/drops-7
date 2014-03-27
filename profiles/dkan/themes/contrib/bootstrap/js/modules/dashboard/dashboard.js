(function ($) {
  // Override core JS so it works with "button" tags.
  if (Drupal.behaviors.dashboard && Drupal.behaviors.dashboard.setupDrawer) {
    /**
     * Sets up the drag-and-drop behavior and the 'close' button.
     */
    Drupal.behaviors.dashboard.setupDrawer = function () {
      $('div.customize .canvas-content :input').click(Drupal.behaviors.dashboard.exitCustomizeMode);
      $('div.customize .canvas-content').append('<a class="button" href="' + Drupal.settings.dashboard.dashboard + '">' + Drupal.t('Done') + '</a>');
      // Initialize drag-and-drop.
      var regions = $('#dashboard div.region');
      regions.sortable({
        connectWith: regions,
        cursor: 'move',
        cursorAt: {top:0},
        dropOnEmpty: true,
        items: '> div.block, > div.disabled-block',
        placeholder: 'block-placeholder clearfix',
        tolerance: 'pointer',
        start: Drupal.behaviors.dashboard.start,
        over: Drupal.behaviors.dashboard.over,
        sort: Drupal.behaviors.dashboard.sort,
        update: Drupal.behaviors.dashboard.update
      });
    };
  }
})(jQuery);
