
/**
 *  @file
 *  Attach Media ckeditor behaviors.
 */

(function ($) {
  Drupal.media = Drupal.media || {};

  Drupal.settings.ckeditor.plugins['media'] = {
    /**
     * Execute the button.
     */
    invoke: function (data, settings, instanceId) {
      if (data.format == 'html') {
        if (jQuery(data.node).is('[data-media-element]')) {
          // Change the view mode for already-inserted media.
          var mediaFile = Drupal.media.filter.extract_file_info(jQuery(data.node));
          Drupal.media.popups.mediaStyleSelector(mediaFile, function (mediaFiles) {
            Drupal.settings.ckeditor.plugins['media'].insertMediaFile(mediaFile, mediaFiles, CKEDITOR.instances[instanceId]);
          }, settings['global']);
        }
        else {
          Drupal.media.popups.mediaBrowser(function (mediaFiles) {
            Drupal.settings.ckeditor.plugins['media'].mediaBrowserOnSelect(mediaFiles, instanceId);
          }, settings['global']);
        }
      }
    },

    /**
     * Respond to the mediaBrowser's onSelect event.
     */
    mediaBrowserOnSelect: function (mediaFiles, instanceId) {
      var mediaFile = mediaFiles[0];
      var options = {};
      Drupal.media.popups.mediaStyleSelector(mediaFile, function (formattedMedia) {
        Drupal.settings.ckeditor.plugins['media'].insertMediaFile(mediaFile, formattedMedia, CKEDITOR.instances[instanceId]);
      }, options);

      return;
    },

    insertMediaFile: function (mediaFile, formattedMedia, ckeditorInstance) {
      // Customization of Drupal.media.filter.registerNewElement().
      var element = Drupal.media.filter.create_element(formattedMedia.html, {
        fid: mediaFile.fid,
        view_mode: formattedMedia.type,
        attributes: formattedMedia.options
      });

      // Use own wrapper element to be able to properly deal with selections.
      // Check prepareDataForWysiwygMode() in plugin.js for details.
      var wysiwygHTML = Drupal.media.filter.getWysiwygHTML(element);

      // Insert element. Use CKEDITOR.dom.element.createFromHtml to ensure our
      // custom wrapper element is preserved.
      if (wysiwygHTML.indexOf("<!--MEDIA-WRAPPER-START-") !== -1) {
        ckeditorInstance.plugins.media.mediaLegacyWrappers = true;
        wysiwygHTML = wysiwygHTML.replace(/<!--MEDIA-WRAPPER-START-(\d+)-->(.*?)<!--MEDIA-WRAPPER-END-\d+-->/gi, '');
      } else {
        wysiwygHTML = '<mediawrapper data="">' + wysiwygHTML + '</mediawrapper>';
      }

      var editorElement = CKEDITOR.dom.element.createFromHtml(wysiwygHTML);
      ckeditorInstance.insertElement(editorElement);

      // Initialize widget on our html if possible.
      if (parseFloat(CKEDITOR.version) >= 4.3 && typeof(CKEDITOR.plugins.registered.widget) != 'undefined') {
        ckeditorInstance.widgets.initOn( editorElement, 'mediabox' );
      }
    },

    /**
     * Forces custom attributes into the class field of the specified image.
     *
     * Due to a bug in some versions of Firefox
     * (http://forums.mozillazine.org/viewtopic.php?f=9&t=1991855), the
     * custom attributes used to share information about the image are
     * being stripped as the image markup is set into the rich text
     * editor.  Here we encode these attributes into the class field so
     * the data survives.
     *
     * @param imgElement
     *   The image
     * @fid
     *   The file id.
     * @param view_mode
     *   The view mode.
     * @param additional
     *   Additional attributes to add to the image.
     */
    forceAttributesIntoClass: function (imgElement, fid, view_mode, additional) {
      var wysiwyg = imgElement.attr('wysiwyg');
      if (wysiwyg) {
        imgElement.addClass('attr__wysiwyg__' + wysiwyg);
      }
      var format = imgElement.attr('format');
      if (format) {
        imgElement.addClass('attr__format__' + format);
      }
      var typeOf = imgElement.attr('typeof');
      if (typeOf) {
        imgElement.addClass('attr__typeof__' + typeOf);
      }
      if (fid) {
        imgElement.addClass('img__fid__' + fid);
      }
      if (view_mode) {
        imgElement.addClass('img__view_mode__' + view_mode);
      }
      if (additional) {
        for (var name in additional) {
          if (additional.hasOwnProperty(name)) {
            switch (name) {
              case 'field_file_image_alt_text[und][0][value]':
                imgElement.attr('alt', additional[name]);
                break;
              case 'field_file_image_title_text[und][0][value]':
                imgElement.attr('title', additional[name]);
                break;
              default:
                imgElement.addClass('attr__' + name + '__' + additional[name]);
                break;
            }
          }
        }
      }
    },

    /**
     * Retrieves encoded attributes from the specified class string.
     *
     * @param classString
     *   A string containing the value of the class attribute.
     * @return
     *   An array containing the attribute names as keys, and an object
     *   with the name, value, and attribute type (either 'attr' or
     *   'img', depending on whether it is an image attribute or should
     *   be it the attributes section)
     */
    getAttributesFromClass: function (classString) {
      var actualClasses = [];
      var otherAttributes = [];
      var classes = classString.split(' ');
      var regexp = new RegExp('^(attr|img)__([^\S]*)__([^\S]*)$');
      for (var index = 0; index < classes.length; index++) {
        var matches = classes[index].match(regexp);
        if (matches && matches.length === 4) {
          otherAttributes[matches[2]] = {
            name: matches[2],
            value: matches[3],
            type: matches[1]
          };
        }
        else {
          actualClasses.push(classes[index]);
        }
      }
      if (actualClasses.length > 0) {
        otherAttributes['class'] = {
          name: 'class',
          value: actualClasses.join(' '),
          type: 'attr'
        };
      }
      return otherAttributes;
    },

    sortAttributes: function (a, b) {
      var nameA = a.name.toLowerCase();
      var nameB = b.name.toLowerCase();
      if (nameA < nameB) {
        return -1;
      }
      if (nameA > nameB) {
        return 1;
      }
      return 0;
    }
  };

})(jQuery);
