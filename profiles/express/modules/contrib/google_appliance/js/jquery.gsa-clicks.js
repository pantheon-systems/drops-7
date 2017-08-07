/*! Google Search Appliance Click Tracking - v1.0.0 - 2014-07-08
* https://github.com/asmallwebfirm/gsa-clicks
* Copyright (c) 2014 Eric Peterson; Licensed GPL-2.0 */
(function($) {

  var options = {},
      gsaLoadFired = false;

  // Collection method.
  $.fn.gsaClicks = function(optionOverrides) {
    // Override default options with passed-in options.
    options = $.extend(options, $.fn.gsaClicks.options, optionOverrides);

    $.each(options.clickTypes, function (clickType, selector) {
      var $selected;

      if (selector) {
        // Save off the selected element(s).
        $selected = $(selector);

        // Mark the click type for each selected element.
        $.fn.gsaClicks.write.call($selected, 'gsa-clicktype', clickType);

        // Mark rank for click types with multiple elements.
        if ($selected.length > 1) {
          $selected.each(function(index) {
            $.fn.gsaClicks.write.call($(this), 'gsa-rank', index + 1);
          });
        }

        // Users can provide custom clickData by setting clickData to a function
        // that returns a string to be passed to the GSA based on the element.
        if (typeof options.clickData === 'function') {
          // We write the data returned by the callback.
          $.fn.gsaClicks.write.call($selected, 'gsa-clickdata', options.clickData($selected, clickType));
        }
      }
    });

    // Fire off a "load" event to the GSA.
    if (!gsaLoadFired) {
      $.fn.gsaClicks.click.call(this, 'load');
      gsaLoadFired = true;
    }

    return this.mousedown(function() {
      var $link = $(this),
          clickType = $.fn.gsaClicks.read.call($link, 'gsa-clicktype'),
          rank = $.fn.gsaClicks.read.call($link, 'gsa-rank'),
          clickData = $.fn.gsaClicks.read.call($link, 'gsa-clickdata');

      $.fn.gsaClicks.click.call($link,
        typeof clickType === 'undefined' ? "OTHER" : clickType,
        $link.attr('href'),
        typeof rank  === 'undefined' ? null : rank,
        typeof clickData === 'undefined' ? null : clickData
      );
    });
  };

  // Default options.
  $.fn.gsaClicks.options = {
    // Host against which click calls should be made (exclude /click path part).
    host: '',
    // Click path part; only override if necessary.
    pathPart: '/click',
    // Collection for which search results are being presented.
    collection: 'default_collection',
    // Query string that resulted in the results presented.
    query: '',
    // The page start of the results presented.
    start: 0,
    // An object whose keys are click types and whose values are selectors used
    // to attach click type data.
    clickTypes: {}
  };

  /**
   * Report click data to the /click service on the GSA.
   *
   * @param clickType
   *   Click type.
   * @param targetUrl
   *   (optional) Target URL the user clicked on.
   * @param rank
   *   (optional) Rank.
   * @param clickData
   *   (optional) Click data.
   *
   * @see http://www.google.com/support/enterprise/static/gsa/docs/admin/70/gsa_doc_set/xml_reference/advanced_search_reporting.html#1080237
   */
  $.fn.gsaClicks.click = function(clickType, targetUrl, rank, clickData) {
    var img,
        src;

    // Construct the image src parameter.
    src = options.host + options.pathPart +
      "?site=" + encodeURIComponent(options.collection) +
      "&q=" + encodeURIComponent(options.query) +
      "&s=" + encodeURIComponent(options.start) +
      "&ct=" + encodeURIComponent(clickType);

    // Only apply optional arguments if provided with valid values.
    if (typeof targetUrl !== 'undefined' && targetUrl !== null) {
      src = src.concat('&url=', encodeURIComponent(targetUrl.replace(/#.*/, "")));
    }

    if (typeof rank !== 'undefined' && rank !== null) {
      src = src.concat('&r=', encodeURIComponent(rank));
    }

    if (typeof clickData !== 'undefined' && clickData !== null) {
      src = src.concat('&cd=', encodeURIComponent(clickData));
    }

    // Create an image element with an src value as described above.
    img = document.createElement('img');
    img.src = src;

    return true;
  };

  /**
   * Writes specified data to a given attribute on the given element.
   */
  $.fn.gsaClicks.write = function(attribute, data) {
    $(this).data(attribute, data);
  };

  /**
   * Reads a specified attribute from the given element.
   */
  $.fn.gsaClicks.read = function(attribute) {
    return $(this).data(attribute);
  };

}(jQuery));
