/**
 * @file
 * Attaches the behaviors for the Colorizer module.
 * Copied from Color module with #selector changed
 */

(function ($) {

Drupal.behaviors.colorizer = {
  attach: function (context, settings) {
    var i, j, colors, field_name;
    // This behavior attaches by ID, so is only valid once on a page.
    var form = $('.colorizer-form', context).once('color');
    if (form.length === 0) {
      return;
    }
    var inputs = [];
    var locks = [];
    var focused = null;

    // Add Farbtastic.
    $(form).prepend('<div id="placeholder"></div>').addClass('color-processed');
    var farb = $.farbtastic('#placeholder');

    // Decode reference colors to HSL.
    var reference = settings.color.reference;
    for (i in reference) {
      if (reference[i] != 'transparent') {
        reference[i] = farb.RGBToHSL(farb.unpack(reference[i]));
      }
    }

    // Build a preview.
    var height = [];
    var width = [];
    // Loop through all defined gradients.
    for (i in settings.gradients) {
      // Add element to display the gradient.
      $('#preview').once('color').append('<div id="gradient-' + i + '"></div>');
      var gradient = $('#preview #gradient-' + i);
      // Add height of current gradient to the list (divided by 10).
      height.push(parseInt(gradient.css('height'), 10) / 10);
      // Add width of current gradient to the list (divided by 10).
      width.push(parseInt(gradient.css('width'), 10) / 10);
      // Add rows (or columns for horizontal gradients).
      // Each gradient line should have a height (or width for horizontal
      // gradients) of 10px (because we divided the height/width by 10 above).
      for (j = 0; j < (settings.gradients[i]['direction'] == 'vertical' ? height[i] : width[i]); ++j) {
        gradient.append('<div class="gradient-line"></div>');
      }
    }

    // Fix preview background in IE6.
    if (navigator.appVersion.match(/MSIE [0-6]\./)) {
      var e = $('#preview #img')[0];
      var image = e.currentStyle.backgroundImage;
      e.style.backgroundImage = 'none';
      e.style.filter = "progid:DXImageTransform.Microsoft.AlphaImageLoader(enabled=true, sizingMethod=crop, src='" + image.substring(5, image.length - 2) + "')";
    }

    // Set up colorScheme selector.
    $('#edit-scheme', form).change(function () {
      var schemes = settings.color.schemes, colorScheme = this.options[this.selectedIndex].value;
      if (colorScheme !== '' && schemes[colorScheme]) {
        // Get colors of active scheme.
        colors = schemes[colorScheme];
        for (var field_name in colors) {
          var element = $('#edit-palette-' + field_name);
          if (element.length > 0) {
            callback(element, colors[field_name], false, true);
          }
        }
        preview();
      }
    });

    /**
     * Renders the preview.
     */
    function preview() {
      // first, find any existing sheet
      var css = settings.css;
      var palette = settings.color.reference;
      // perform the variable replacement in css template
      for (var field_name in palette) {
        value = $('#edit-palette-' + field_name).val();
        find = '@'+field_name+'\\b';
        css = css.replace(new RegExp(find, 'g'), value);
      }

      var style = settings.stylesheet;
      // see if sheet is already created
      if (!style) {
        style = document.createElement('style');
        style.setAttribute('type', 'text/css');

        var cssText = '';
        if (style.styleSheet) { // IE does it this way
          style.styleSheet.cssText = cssText;
        } else { // everyone else does it this way
          style.appendChild(document.createTextNode(cssText));
        }
        document.getElementsByTagName("head")[0].appendChild(style);
      }

      if (style.styleSheet) {
          // IE
          style.styleSheet.cssText = css;
      } else {
          // Other browsers
          style.innerHTML = css;
      }
      // save new sheet so we can reuse it later
      settings.stylesheet = style;
    }

    /**
     * Shifts a given color, using a reference pair (ref in HSL).
     *
     * This algorithm ensures relative ordering on the saturation and luminance
     * axes is preserved, and performs a simple hue shift.
     *
     * It is also symmetrical. If: shift_color(c, a, b) == d, then
     * shift_color(d, b, a) == c.
     */
    function shift_color(given, ref1, ref2) {
      console.log(given, ref1, ref2);
      // Convert to HSL.
      given = farb.RGBToHSL(farb.unpack(given));

      // Hue: apply delta.
      given[0] += ref2[0] - ref1[0];

      // Saturation: interpolate.
      if (ref1[1] == 0 || ref2[1] == 0) {
        given[1] = ref2[1];
      }
      else {
        var d = ref1[1] / ref2[1];
        if (d > 1) {
          given[1] /= d;
        }
        else {
          given[1] = 1 - (1 - given[1]) * d;
        }
      }

      // Luminance: interpolate.
      if (ref1[2] == 0 || ref2[2] == 0) {
        given[2] = ref2[2];
      }
      else {
        var d = ref1[2] / ref2[2];
        if (d > 1) {
          given[2] /= d;
        }
        else {
          given[2] = 1 - (1 - given[2]) * d;
        }
      }

      return farb.pack(farb.HSLToRGB(given));
    }

    /**
     * Callback for Farbtastic when a new color is chosen.
     */
    function callback(input, color, propagate, colorScheme) {
      var matched;
      // Set background/foreground colors.

      if (color == 'transparent') {
        $(input).css({backgroundColor: color});
      }
      else {
        $(input).css({
          backgroundColor: color,
          'color': farb.RGBToHSL(farb.unpack(color))[2] > 0.5 ? '#000' : '#fff'
        });
      }

      // Change input value.
      if ($(input).val() && $(input).val() != color) {
        if ($(input).val() != 'transparent') {
          var prev_color = farb.RGBToHSL(farb.unpack($(input).val()));
        }
        $(input).val(color);

        // Update locked values.
        if (propagate && ($(input).val() != 'transparent')) {
          var base_color = farb.RGBToHSL(farb.unpack(color));
          var ref_color;
          for (j = 0; j < inputs.length; j++) {
            console.log(j);
            if ((j != i) && locks[j] && !$(locks[j]).is('.unlocked')) {
              console.log('locked');
              ref_color = farb.RGBToHSL(farb.unpack($(inputs[j]).val()));
              matched = shift_color(color, prev_color, ref_color);
              callback(inputs[j], matched, false);
            }
          }

          // Update preview.
          preview();
        }

        // Reset colorScheme selector.
        if (!colorScheme) {
          resetScheme();
        }
      }
    }

    /**
     * Resets the color scheme selector.
     */
    function resetScheme() {
      $('#edit-scheme', form).each(function () {
        this.selectedIndex = this.options.length - 1;
      });
    }

    /**
     * Focuses Farbtastic on a particular field.
     */
    function focus() {
      var input = this;
      // Remove old bindings.
      focused && $(focused).unbind('keyup', farb.updateValue)
          .unbind('keyup', preview).unbind('keyup', resetScheme)
          .parent().removeClass('item-selected');

      // Add new bindings.
      focused = this;
      farb.linkTo(function (color) { callback(input, color, true, false); });
      farb.setColor(this.value);
      $(focused).keyup(farb.updateValue).keyup(preview).keyup(resetScheme)
        .parent().addClass('item-selected');
    }

    // Initialize color fields.
    $('#palette input.form-text', form)
    .each(function () {
      // Extract palette field name
      this.key = this.id.substring(13);

      // Link to color picker temporarily to initialize.
      farb.linkTo(function () {}).setColor('#000').linkTo(this);

      // Add lock.
      var i = inputs.length;
      var lock = $('<div class="lock"></div>').toggle(
        function () {
          $(this).addClass('unlocked');
        },
        function () {
          $(this).removeClass('unlocked');
        }
      );
      $(this).after(lock);
      locks.push(lock);

      $(this).parent().find('.lock').click();
      this.i = i;
      inputs.push(this);
    })
    .focus(focus);

    $('#palette label', form);

    // Focus first color.
    focus.call(inputs[0]);

    // Render preview.
    preview();
  }
};

})(jQuery);
