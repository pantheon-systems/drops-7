(function($) {
 CKEDITOR.plugins.add('icon_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'icon_generator_command', new CKEDITOR.dialogCommand( 'icon_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'icon_sc_generator_button', {
    label: 'Icon Shortcode Generator', //this is the tooltip text for the button
    command: 'icon_generator_command',
    icon: this.path + 'images/icon_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'icon_sc_generatorDialog', this.path + 'dialogs/icon_sc_generator.js' );
  }
 });
 
})(jQuery);