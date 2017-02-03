/*
Copyright (c) 2003-2013, CKSource - Frederico Knabben. All rights reserved.
For licensing, see LICENSE.html or http://ckeditor.com/license
*/

/**
 * @file Plugin for inserting images from Drupal media module
 *
 * @TODO Remove all the legecy media wrapper once it's sure nobody uses that
 * anymore.
 */
( function() {
  var mediaPluginDefinition = {
    icons: 'media',
    requires: ['button'],
    // Check if this instance has widget support. All the default distributions
    // of the editor have the widget plugin disabled by default.
    hasWidgetSupport: typeof(CKEDITOR.plugins.registered.widget) != 'undefined',
    mediaLegacyWrappers: false,

    // Wrap Drupal plugin in a proxy plugin.
    init: function(editor){
      editor.addCommand( 'media',
      {
        exec: function (editor) {
          var data = {
            format: 'html',
            node: null,
            content: ''
          };
          var selection = editor.getSelection();

          if (selection) {
            data.node = selection.getSelectedElement();
            if (data.node) {
              data.node = data.node.$;
            }
            if (selection.getType() == CKEDITOR.SELECTION_TEXT) {
              if (CKEDITOR.env.ie && CKEDITOR.env.version < 10) {
                data.content = selection.getNative().createRange().text;
              }
              else {
                data.content = selection.getNative().toString();
              }
            }
            else if (data.node) {
              // content is supposed to contain the "outerHTML".
              data.content = data.node.parentNode.innerHTML;
            }
          }
          Drupal.settings.ckeditor.plugins['media'].invoke(data, Drupal.settings.ckeditor.plugins['media'], editor.name);
        }
      });

      editor.ui.addButton( 'Media',
      {
        label: 'Add media',
        command: 'media',
        icon: this.path + 'images/icon.gif'
      });

      var ckeditorversion = parseFloat(CKEDITOR.version);

      // Because the media comment wrapper don't work well for CKEditor we
      // replace them by using a custom mediawrapper element.
      // Instead having
      // <!--MEDIA-WRAPPER-START-1--><img /><!--MEDIA-WRAPPER-END-1-->
      // We wrap the placeholder with
      // <mediawrapper data="1"><img /></mediawrapper>
      // That way we can deal better with selections - see selectionChange.
      CKEDITOR.dtd['mediawrapper'] = CKEDITOR.dtd;
      CKEDITOR.dtd.$blockLimit['mediawrapper'] = 1;
      CKEDITOR.dtd.$inline['mediawrapper'] = 1;
      CKEDITOR.dtd.$nonEditable['mediawrapper'] = 1;
      if (ckeditorversion >= 4.1) {
        // Register allowed tag for advanced filtering.
        editor.filter.allow( 'mediawrapper[!data]', 'mediawrapper', true);
        // Don't remove the data-file_info attribute added by media!
        editor.filter.allow( '*[!data-file_info]', 'mediawrapper', true);
        // Ensure image tags accept all kinds of attributes.
        editor.filter.allow( 'img[*]{*}(*)', 'mediawrapper', true);
        // Objects should be selected as a whole in the editor.
        CKEDITOR.dtd.$object['mediawrapper'] = 1;
      }
      function prepareDataForWysiwygMode(data) {
        data = Drupal.media.filter.replaceTokenWithPlaceholder(data);
        // Legacy media wrapper.
        mediaPluginDefinition.mediaLegacyWrappers = (data.indexOf("<!--MEDIA-WRAPPER-START-") !== -1);
        data = data.replace(/<!--MEDIA-WRAPPER-START-(\d+)-->(.*?)<!--MEDIA-WRAPPER-END-\d+-->/gi, '<mediawrapper data="$1">$2</mediawrapper>');
        return data;
      }
      function prepareDataForSourceMode(data) {
        var replacement = '$2';
        // Legacy wrapper
        if (mediaPluginDefinition.mediaLegacyWrappers) {
          replacement = '<!--MEDIA-WRAPPER-START-$1-->$2<!--MEDIA-WRAPPER-END-$1-->';
        }
        data = data.replace(/<mediawrapper data="(.*)">(.*?)<\/mediawrapper>/gi, replacement);
        data = Drupal.media.filter.replacePlaceholderWithToken(data);
        return data;
      }

      // Ensure the tokens are replaced by placeholders while editing.
      // Check for widget support.
      if (mediaPluginDefinition.hasWidgetSupport) {
        editor.widgets.add( 'mediabox',
        {
          button: 'Create a mediabox',
          template: '<mediawrapper></mediawrapper>',
          editables: {},
          allowedContent: '*',
          upcast: function( element ) {
            if (element.name != 'mediawrapper') {
              // Ensure media tokens are converted to media placeholdes.
              element.setHtml(prepareDataForWysiwygMode(element.getHtml()));
            }
            return element.name == 'mediawrapper';
          },

          downcast: function( widgetElement ) {
            var token = prepareDataForSourceMode(widgetElement.getOuterHtml());
            return new CKEDITOR.htmlParser.text(token);
            return element.name == 'mediawrapper';
          }
        });
      }
      else if (ckeditorversion >= 4) {
        // CKEditor >=4.0
        editor.on('setData', function( event ) {
          event.data.dataValue = prepareDataForWysiwygMode(event.data.dataValue);
        });
      }
      else {
        // CKEditor >=3.6 behaviour.
        editor.on( 'beforeSetMode', function( event, data ) {
          event.removeListener();
          var wysiwyg = editor._.modes[ 'wysiwyg' ];
          var source = editor._.modes[ 'source' ];
          wysiwyg.loadData = CKEDITOR.tools.override( wysiwyg.loadData, function( org )
          {
            return function( data ) {
              return ( org.call( this, prepareDataForWysiwygMode(data)) );
            };
          } );
          source.loadData = CKEDITOR.tools.override( source.loadData, function( org )
          {
            return function( data ) {
              return ( org.call( this, prepareDataForSourceMode(data) ) );
            };
          } );
        });
      }

      // Provide alternative to the widget functionality introduced in 4.3.
      if (!mediaPluginDefinition.hasWidgetSupport) {
        // Ensure tokens instead the html element is saved.
        editor.on('getData', function( event ) {
          event.data.dataValue = prepareDataForSourceMode(event.data.dataValue);
        });

        // Ensure our enclosing wrappers are always included in the selection.
        editor.on('selectionChange', function( event ) {
          var ranges = editor.getSelection().getRanges().createIterator();
          var newRanges = [];
          var currRange;
          while(currRange = ranges.getNextRange()) {
            var commonAncestor = currRange.getCommonAncestor(false);
            if (commonAncestor && typeof(commonAncestor.getName) != 'undefined' && commonAncestor.getName() == 'mediawrapper') {
              var range = new CKEDITOR.dom.range( editor.document );
              if (currRange.collapsed === true) {
                // Don't allow selection within the wrapper element.
                if (currRange.startOffset == 0) {
                  // While v3 plays nice with setting start and end to avoid
                  // editing within the media wrapper element, v4 ignores that.
                  // Thus we try to move the cursor further away.
                  if (parseInt(CKEDITOR.version) > 3) {
                    range.setStart(commonAncestor.getPrevious());
                    range.setEnd(commonAncestor.getPrevious());
                  }
                  else {
                    range.setStartBefore(commonAncestor);
                  }
                }
                else {
                  // While v3 plays nice with setting start and end to avoid
                  // editing within the media wrapper element, v4 ignores that.
                  // Thus we try to move the cursor further away.
                  if (parseInt(CKEDITOR.version) > 3) {
                    range.setStart(commonAncestor.getNext(), 1);
                    range.setEnd(commonAncestor.getNext(), 1);
                  }
                  else {
                    range.setStartAfter(commonAncestor);
                  }
                }
              }
              else {
                // Always select the whole wrapper element.
                range.setStartBefore(commonAncestor);
                range.setEndAfter(commonAncestor);
              }
              newRanges.push(range);
            }
          }
          if (newRanges.length) {
            editor.getSelection().selectRanges(newRanges);
          }
        });
      }
    }
  };
  // Add dependency to widget plugin if possible.
  if (parseFloat(CKEDITOR.version) >= 4.3 && mediaPluginDefinition.hasWidgetSupport) {
    mediaPluginDefinition.requires.push('widget');
  }
  CKEDITOR.plugins.add( 'media', mediaPluginDefinition);
} )();
