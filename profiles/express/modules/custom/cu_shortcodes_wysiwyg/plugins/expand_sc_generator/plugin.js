(function($) {
 CKEDITOR.plugins.add('expand_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'expand_generator_command', new CKEDITOR.dialogCommand( 'expand_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'expand_sc_generator_button', {
    label: 'Expand Content Shortcode Generator', //this is the tooltip text for the button
    command: 'expand_generator_command',
    icon: this.path + 'images/expand_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'expand_sc_generatorDialog', this.path + 'dialogs/expand_sc_generator.js' );
  }
 });
 
})(jQuery);