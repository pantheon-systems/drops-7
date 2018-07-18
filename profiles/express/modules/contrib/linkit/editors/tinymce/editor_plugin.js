/**
 * @file
 * Plugin for inserting links with Linkit.
 */

(function ($) {

  tinymce.create('tinymce.plugins.linkit', {
    init : function(editor, url) {

      // Register commands
      editor.addCommand('mceLinkit', function() {
        if (typeof Drupal.settings.linkit === 'undefined') {
          alert(Drupal.t('Could not find the Linkit profile.'));
          return ;
        }

        // Set the editor object.
        Drupal.settings.linkit.currentInstance.editor = editor;

        // Find the current input format of the field we're looking at. Note that we get it in the form
        // "format<formatname" instead of just "<formatname>" so we use .substring() to remove the "format".
        if (Drupal.wysiwyg && Drupal.wysiwyg.instances[editor.id].format) {
          var format = Drupal.wysiwyg.instances[editor.id].format.substring(6);
        } else {
          alert(Drupal.t('Could not find the Linkit profile.'));
          return;
        }

        // Set profile based on the current text format of this field.
        Drupal.settings.linkit.currentInstance.profile = Drupal.settings.linkit.formats[format].profile;

        // Set the name of the source field..
        Drupal.settings.linkit.currentInstance.source = editor.id;

        // Set the source type.
        Drupal.settings.linkit.currentInstance.helper = 'tinymce';

        // Stores the current editor selection for later restoration. This can
        // be useful since some browsers looses it's selection if a control
        // element is selected/focused inside the dialogs.
        editor.windowManager.bookmark = editor.selection.getBookmark(1);

        // Create the modal.
        Drupal.linkit.createModal();
      });

      // Register buttons
      editor.addButton('linkit', {
        title : Drupal.t('Link to content'),
        cmd : 'mceLinkit',
        image : url + '/images/linkit.png'
      });

      // We need the real contextmenu in order to make this work.
      if (editor && editor.plugins.contextmenu) {
        // Contextmenu gets called - this is what we do.
        editor.plugins.contextmenu.onContextMenu.add(function(th, m, e, col) {
          // Only if selected node is an link do this.
          if (e.nodeName == 'A' || !col) {
            // Remove all options from standard contextmenu.
            m.removeAll();
            th._menu.add({
              title : Drupal.t('Link to content'),
              cmd : 'mceLinkit',
              icon : 'linkit'
            });
            //m.addSeparator();
          }
        });
      }
    },

    getInfo : function() {
      return {
        longname : 'Linkit',
        author : 'Emil Stjerneman',
        authorurl : 'http://www.stjerneman.com',
        infourl : 'http://drupal.org/project/linkit',
        version : tinymce.majorVersion + "." + tinymce.minorVersion
      };
    }
  });

  // Register plugin
  tinymce.PluginManager.add('linkit', tinymce.plugins.linkit);

})(jQuery);