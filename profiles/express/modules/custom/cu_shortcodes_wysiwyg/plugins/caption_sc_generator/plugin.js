(function($) {
 CKEDITOR.plugins.add('caption_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'caption_generator_command', new CKEDITOR.dialogCommand( 'caption_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'caption_sc_generator_button', {
    label: 'Image Caption Shortcode Generator', //this is the tooltip text for the button
    command: 'caption_generator_command',
    icon: this.path + 'images/caption_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'caption_sc_generatorDialog', this.path + 'dialogs/caption_sc_generator.js' );
  }
 });
 
})(jQuery);