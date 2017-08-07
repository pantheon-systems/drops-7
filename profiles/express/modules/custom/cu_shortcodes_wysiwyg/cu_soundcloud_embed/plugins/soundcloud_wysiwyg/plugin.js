(function($) {
 CKEDITOR.plugins.add('soundcloud_embed', {
  init: function( editor )
  {


   editor.addCommand( 'soundcloud_embed_command', new CKEDITOR.dialogCommand( 'soundcloud_embedDialog' ) );

   editor.ui.addButton( 'soundcloud_embed_button', {
    label: 'Soundcloud Shortcode Generator', //this is the tooltip text for the button
    command: 'soundcloud_embed_command',
    icon: this.path + 'images/soundcloud_embed_button.gif'
   });
   CKEDITOR.dialog.add( 'soundcloud_embedDialog', this.path + 'dialogs/soundcloud_embed.js' );
  }
 });

})(jQuery);
