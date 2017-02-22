(function ($) {

"use strict";

Drupal.behaviors.entityreference_filter_dynamic = {
  attach: function (context, settings) {
    if (settings.entityreference_filter) {
      $.each(settings.entityreference_filter, function(form_id, filter_setting) {
        var form = $('#' + form_id, context);
        if (form.length === 0) {
          return;
        }

        var view = filter_setting.view;
        var args = filter_setting.args;
        var dependent_filters = filter_setting.dynamic;
        var elements = {};
        var controlling_filters = {};

        if (dependent_filters) {
          $.each(dependent_filters, function(i, dep_controlling_filters) {
            $.each(dep_controlling_filters, function(j, controlling_filter) {
              controlling_filters[controlling_filter] = controlling_filter;
            });
          });
        }

        $.each(controlling_filters, function(i, controlling_filter) {
          var element = form.find('[name="' + controlling_filter + '"],[name="' + controlling_filter + '[]"]');
          if (element.length > 0) {
            elements[controlling_filter] = element;
            element.attr('autocomplete', 'off');
            var url = settings.basePath + settings.pathPrefix + 'entityreference_filter/update/' + view + '/' + controlling_filter;

            element.once('entityreference_filter').change(function() {
              var ajax = new Drupal.ajax(false, false, {url: url});
              var parentBeforeSerialize = ajax.beforeSerialize;
              ajax.beforeSerialize = function(element, options) {
                parentBeforeSerialize(element, options);
                options.type = 'GET';
                options.data = {};

                $.each(elements, function(fn, element) {
                  var value = element.fieldValue();
                  if (value.length > 0) {
                    options.data[fn] = value[0];
                  }
                });

                options.data.entityreference_filter_form_id = form_id;
                options.data.entityreference_filter_args = args;
              };
              ajax.eventResponse(ajax, {});
            });
            // Another way.
            //var ajax = new Drupal.ajax(false, element, {event: 'change', url: url});
            //var parentBeforeSerialize = ajax.beforeSerialize;
            //ajax.beforeSerialize = function(element, options) {
            //  parentBeforeSerialize(element, options);
            //  options.type = 'GET';
            //  options.data = {};
            //
            //  $.each(elements, function(fn, element) {
            //    var value = element.fieldValue();
            //    if (value.length > 0) {
            //      options.data[fn] = value[0];
            //    }
            //  });
            //
            //  options.data['entityreference_filter_form_id'] = form_id;
            //  options.data['entityreference_filter_args'] = args;
            //};
          }
        });
      });
    }
  }
};

/**
 * Command to insert new content into the DOM without wrapping in extra DIV element.
 */
Drupal.ajax.prototype.commands.entityreference_filter_insertnowrap = function (ajax, response, status) {
  // Get information from the response. If it is not there, default to
  // our presets.
  var wrapper = response.selector ? $(response.selector) : $(ajax.wrapper);
  var method = response.method || ajax.method;
  var effect = ajax.getEffect(response);

  // We don't know what response.data contains: it might be a string of text
  // without HTML, so don't rely on jQuery correctly interpreting
  // $(response.data) as new HTML rather than a CSS selector. Also, if
  // response.data contains top-level text nodes, they get lost with either
  // $(response.data) or $('<div></div>').replaceWith(response.data).
  var new_content_wrapped = $('<div></div>').html(response.data);
  var new_content = new_content_wrapped.contents();
  var settings = {};

  // If removing content from the wrapper, detach behaviors first.
  switch (method) {
    case 'html':
    case 'replaceWith':
    case 'replaceAll':
    case 'empty':
    case 'remove':
      settings = response.settings || ajax.settings || Drupal.settings;
      Drupal.detachBehaviors(wrapper, settings);
      break;
  }

  // Add the new content to the page.
  wrapper[method](new_content);

  // Immediately hide the new content if we're using any effects.
  if (effect.showEffect !== 'show') {
    new_content.hide();
  }

  // Determine which effect to use and what content will receive the
  // effect, then show the new content.
  if ($('.ajax-new-content', new_content).length > 0) {
    $('.ajax-new-content', new_content).hide();
    new_content.show();
    $('.ajax-new-content', new_content)[effect.showEffect](effect.showSpeed);
  }
  else if (effect.showEffect !== 'show') {
    new_content[effect.showEffect](effect.showSpeed);
  }

  // Attach all JavaScript behaviors to the new content, if it was successfully
  // added to the page, this if statement allows #ajax['wrapper'] to be
  // optional.
  if (new_content.parents('html').length > 0) {
    // Apply any settings from the returned JSON if available.
    settings = response.settings || ajax.settings || Drupal.settings;
    Drupal.attachBehaviors(wrapper, settings);
  }
};

})(jQuery);
