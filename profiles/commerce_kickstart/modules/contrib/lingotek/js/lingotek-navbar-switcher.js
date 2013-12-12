/**
 * @file
 * Provides a component that previews the page in various device dimensions.
 */

(function ($, Backbone, Drupal) {
/**
 * Attaches behaviors to the navbar tab and preview containers.
 */
Drupal.behaviors.lingotekNavbar = {
  attach: function (context) {
    var defaults = this.defaults;
    // once() returns a jQuery set. It will be empty if no unprocessed
    // elements are found. window and window.parent are equivalent unless the
    // Drupal page is itself wrapped in an iframe.
    var $body = $(window.parent.document.body).once('responsive-preview');

    if (true) {
      var tabModel = new Drupal.responsivePreview.models.TabStateModel();

      // The navbar tab view.
      var $tab = $(context).find('#lingotek-navbar-tab');
      if ($tab.length > 0) {
        var tabView = new Drupal.lingotekNavbar.views.TabView({
          el: $tab.get(),
          tabModel: tabModel,
        });
      }
    }
  },
};

Drupal.lingotekNavbar = Drupal.lingotekNavbar || {models: {}, views: {}};

/**
 * Backbone Model for the Responsive Preview navbar tab state.
 */
Drupal.lingotekNavbar.models.TabStateModel = Backbone.Model.extend({
  defaults: {
    // The state of navbar list of available device previews.
    isDeviceListOpen: false
  }
});

/**
 * Handles responsive preview navbar tab interactions.
 */
Drupal.lingotekNavbar.views.TabView = Backbone.View.extend({
  events: {
    'click': 'toggleDeviceList',
    'mouseleave': 'toggleDeviceList',
  },

  /**
   * Implements Backbone.View.prototype.initialize().
   */
  initialize: function () {
    this.tabModel = this.options.tabModel;
    this.tabModel.bind('change:isDeviceListOpen', this.render, this);
  },

  /**
   * Implements Backbone.View.prototype.render().
   */
  render: function () {

    var isDeviceListOpen = this.tabModel.get('isDeviceListOpen');
  
    this.$el
      // Render the visibility of the navbar tab.
      .toggle(true)
      // Toggle the display of the device list.
      .toggleClass('open', isDeviceListOpen);

    if (isDeviceListOpen) {
      this.correctDeviceListEdgeCollision();
    }
    return this;
  },

  /**
   * Toggles the list of devices available to preview from the navbar tab.
   *
   * @param Object event
   *   jQuery Event object.
   */
  toggleDeviceList: function (event) {

    // Force the options list closed on mouseleave.
    if (event.type === 'mouseleave') {
      this.tabModel.set('isDeviceListOpen', false);
    }
    else {
      this.tabModel.set('isDeviceListOpen', !this.tabModel.get('isDeviceListOpen'));
    }
  },

  /**
   * Model change handler; corrects possible device list window edge collision.
   */
  correctDeviceListEdgeCollision: function () {
    // The position of the dropdown depends on the language direction.
    var dir = document.getElementsByTagName('html')[0].getAttribute('dir');
    var edge = (dir === 'rtl') ? 'left' : 'right';
    this.$el
      .find('.item-list')
      .position({
        'my': edge +' top',
        'at': edge + ' bottom',
        'of': this.$el,
        'collision': 'flip fit'
      });
  }
});

}(jQuery, Backbone, Drupal));
