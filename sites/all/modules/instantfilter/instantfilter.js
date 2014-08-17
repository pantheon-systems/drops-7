(function ($) {

/**
 * Attaches the instantfilter behavior.
 */
Drupal.behaviors.instantFilter = {
  attach: function (context, settings) {
    // Bind instantfilter behaviors specified in the settings.
    for (var base in settings.instantfilter) {
      $('#' + base + ':input', context).once('instantfilter', function () {
        var $this = $(this);
        $this.data('instantfilter', new Drupal.instantFilter(this, settings.instantfilter[base]));
        $this.trigger('drupalInstantFilterCreated');
      });
    }

    // Bind instantfilter behaviors to all elements showing the class.
    $('.instantfilter-filter:input', context).once('instantfilter', function () {
      var $this = $(this);
      $this.data('instantfilter', new Drupal.instantFilter(this));
      $this.trigger('drupalInstantFilterCreated');
    });

    // If context has an parent with the instantfilter class, then context is
    // added dynamicly (e.g. using AJAX). If so, the index needs to be rebuild.
    $(context).closest('.instantfilter-container').each(function () {
      $(this).trigger('drupalInstantFilterIndexInvalidated');
    });
  }
};

/**
 * The instantFilter object.
 * 
 * @constructor
 * @param element
 *   DOM input element to attach the instantfilter to.
 * @param settings
 *   Settings for the instantfilter.
 */
Drupal.instantFilter = function (element, settings) {
  var self = this;
  this.instanceID = Drupal.instantFilter.instanceCounter++;

  this.settings = $.extend({
    container: null,
    groups: {
      '.instantfilter-group': { zebra: false }
    },
    items: {
      '.instantfilter-item': { ignore: null }
    },
    empty: Drupal.t('There were no results.')
  }, settings);
  this.index = false;

  this.element = $(element);
  if (this.settings.container) {
    this.container = $(this.settings.container);
  }
  else {
    this.container = $(document.body);
  }

  var events = (this.element.is('[type=text]')) ? 'keyup' : 'change';
  events += ' drupalInstantFilterTriggerSearch';
  this.element.bind(events, function () {
    self.applyFilter($.trim($(this).val().toLowerCase()));
    self.element.trigger('drupalInstantFilterAfterSearch');
  });

  this.container
    .addClass('instantfilter-container')
    .bind('drupalInstantFilterIndexInvalidated', function () {
      self.index = false;
    });

  // Apply filter once if the element is not empty.
  if (this.element.val()) {
    this.element.trigger('drupalInstantFilterTriggerSearch');
  }
};

Drupal.instantFilter.instanceCounter = 0;
Drupal.instantFilter.filterRules = Drupal.instantFilter.filterRules || {};

/**
 * Get text value of an item.
 */
Drupal.instantFilter.prototype.getValue = function (element, settings) {
  var text = '';
  if (!settings.ignore || !$(element).is(settings.ignore)) {
    for (var i = 0; i < element.childNodes.length; i++) {
      if (element.childNodes[i].nodeType == 1) { // ELEMENT_NODE
        text += this.getValue(element.childNodes[i], settings);
      }
      else if (element.childNodes[i].nodeType == 3) { // TEXT_NODE
        text += element.childNodes[i].nodeValue;
      }
    }
  }
  return text.toLowerCase();
};

/**
 * Rebuild index of the filter.
 */
Drupal.instantFilter.prototype.rebuildIndex = function () {
  var self = this;
  var allitems = '';

  this.items = [];
  this.groups = [];

  var i = 0;
  for (var selector in this.settings.items) {
    allitems += ',' + selector;

    this.container.find(selector).each(function () {
      var item = $.extend({}, self.settings.items[selector], {
        element: $(this),
        value: self.getValue(this, self.settings.items[selector]),
        groups: []
      });

      self.items[i] = item;
      item.element.data('instantfilter:' + self.instanceID + ':item', i);
      i++;
    });
  }

  allitems = allitems.substring(1);

  var i = 0;
  for (var selector in this.settings.groups) {
    this.container.find(selector).each(function () {
      var group = $.extend({}, self.settings.groups[selector], {
        element: $(this),
        total: 0,
        results: 0
      });

      // Link group to items.
      group.element.find(group.items || allitems).each(function () {
        group.total++;

        var item = $(this).data('instantfilter:' + self.instanceID + ':item');
        if (item !== undefined && self.items[item]) {
          self.items[item].groups.push(group);
        }
      });

      self.groups[i] = group;
      group.element.data('instantfilter:' + self.instanceID + ':group', i);
      i++;
    });
  }
};

/**
 * Filters all items for the given string.
 * 
 * All items containing the string will stay visible, while other items are
 * hidden. All groups that don't have any matching items will also be hidden.
 * 
 * @param search
 *   The string to filter items on.
 */
Drupal.instantFilter.prototype.applyFilter = function (search) {
  var self = this;

  if (!this.index) {
    this.rebuildIndex();
    this.index = true;
  }

  this.search = search;
  // Reset the total and group result counters.
  this.results = 0;
  for (var i in this.groups) {
    this.groups[i].results = 0;
  }

  for (var i in this.items) {
    var item = this.items[i];
    var match = item.value.indexOf(this.search) >= 0;

    if (match) {
      // Invoke additional filter rules and execute all of them.
      $.each(Drupal.instantFilter.filterRules, function () {
        match = this.rule(self, item, i);
        if (!match) {
          // Break out of the loop if the match is false.
          return false;
        }
      });
    }

    if (match) {
      // Increment the total and item's groups result counters.
      this.results++;
      for (var i in item.groups) {
        item.groups[i].results++;
      }
    }

    item.element[match ? 'show' : 'hide']();
  }

  for (var i in this.groups) {
    var group = this.groups[i];

    group.element[group.results ? 'show' : 'hide']();

    if (group.zebra) {
      group.element.children(':visible')
        .removeClass('odd even')
        .filter(':odd').addClass('even').end()
        .filter(':even').addClass('odd');
    }
  }

  // If any results are found, remove the 'no results' message.
  // Otherwise display the 'no results message.
  if (this.results) {
    this.element.closest('.form-item').find('.instantfilter-no-results').remove();
  }
  else {
    if (!this.element.closest('.form-item').find('.instantfilter-no-results').length) {
      this.element.closest('.form-item').append($('<p class="instantfilter-no-results"/>').text(this.settings.empty));
    };
  };
};

})(jQuery);
