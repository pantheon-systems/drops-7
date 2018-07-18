(function($) {
 CKEDITOR.plugins.add('button_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'button_generator_command', new CKEDITOR.dialogCommand( 'button_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'button_sc_generator_button', {
    label: 'Button Shortcode Generator', //this is the tooltip text for the button
    command: 'button_generator_command',
    icon: this.path + 'images/button_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'button_sc_generatorDialog', this.path + 'dialogs/button_sc_generator.js' );
  }
 });
 
})(jQuery);