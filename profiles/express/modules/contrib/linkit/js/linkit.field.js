/**
 * @file
 * Linkit field ui functions
 */
(function($, behavior) {
  'use strict';

  Drupal.behaviors[behavior] = {
    attach: function(context, settings) {
      // If there is no fields, just stop here.
      if (undefined === settings.linkit || null === settings.linkit.fields) {
        return false;
      }

      $.each(settings.linkit.fields, function(i, instance) {
        $('#' + instance.source, context).once(behavior, function() {
          var element = this;

          $('.linkit-field-' + instance.source).click(function(event) {
            event.preventDefault();

            // Only care about selection if the element is a textarea.
            if ('textarea' === element.nodeName.toLowerCase()) {
              instance.selection = Drupal.linkit.getDialogHelper('field').getSelection(element);
            }

            Drupal.settings.linkit.currentInstance = instance;
            Drupal.linkit.createModal();
          });
        });
      });
    }
  };

  /**
   * Linkit field dialog helper.
   */
  Drupal.linkit.registerDialogHelper('field', {
    afterInit: function() {},

    /**
     * Insert the link into the field.
     *
     * @param {Object} data
     *   The link object.
     */
    insertLink: function(data) {
      var instance = Drupal.settings.linkit.currentInstance,
          // Call the insert plugin.
          link = Drupal.linkit.getInsertPlugin(instance.insertPlugin).insert(data, instance);

      if (instance.hasOwnProperty('selection')) {
        // Replace the selection and insert the link there.
        this.replaceSelection(instance.source, instance.selection, link);
      }
      else if (instance.hasOwnProperty('titleField')) {
        // The "linkContent" property will always be present when AJAX used.
        // Otherwise, if you use simple insert without autocomplete, then this
        // property will be undefined and title field should not be filled in.
        //
        // @see Drupal.behaviors.linkitSearch.attach
        if (instance.hasOwnProperty('linkContent')) {
          this.replaceFieldValue(instance.titleField, instance.linkContent);
        }

        // The "path" property will always be present after dialog was
        // opened and contain raw URL.
        //
        // @see Drupal.behaviors.linkitDashboard.attach
        this.replaceFieldValue(instance.source, data.path);
      }
      else {
        // Replace the field value.
        this.replaceFieldValue(instance.source, link);
      }
    },

    /**
     * Get field selection.
     */
    getSelection: function(element) {
      var object = {
        start: element.value.length,
        end: element.value.length,
        length: 0,
        text: ''
      };

      // Mozilla and DOM 3.0.
      if ('selectionStart' in element) {
        var length = element.selectionEnd - element.selectionStart;

        object = {
          start: element.selectionStart,
          end: element.selectionEnd,
          length: length,
          text: element.value.substr(element.selectionStart, length)
        };
      }
      // IE.
      else if (document.selection) {
        element.focus();

        var range = document.selection.createRange(),
            textRange = element.createTextRange(),
            textRangeDuplicate = textRange.duplicate();

        textRangeDuplicate.moveToBookmark(range.getBookmark());
        textRange.setEndPoint('EndToStart', textRangeDuplicate);

        if (!(range || textRange)) {
          return object;
        }

        // For some reason IE doesn't always count the \n and \r in the length.
        var text_part = range.text.replace(/[\r\n]/g, '.'),
            text_whole = element.value.replace(/[\r\n]/g, '.'),
            the_start = text_whole.indexOf(text_part, textRange.text.length);

        object = {
          start: the_start,
          end: the_start + text_part.length,
          length: text_part.length,
          text: range.text
        };
      }

      return object;
    },

    /**
     * Replace the field selection.
     */
    replaceSelection: function(id, selection, text) {
      var field = this.getField(id);
      field.value = field.value.substr(0, selection.start) + text + field.value.substr(selection.end, field.value.length);
    },

    /**
     * Replace the field value.
     */
    replaceFieldValue: function(id, text) {
      this.getField(id).value = text;
    },

    getField: function(id) {
      return document.getElementById(id);
    }
  });
})(jQuery, 'linkitField');
