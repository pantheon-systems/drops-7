(function($) {
 CKEDITOR.plugins.add('video_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'video_generator_command', new CKEDITOR.dialogCommand( 'video_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'video_sc_generator_button', {
    label: 'Video Shortcode Generator', //this is the tooltip text for the button
    command: 'video_generator_command',
    icon: this.path + 'images/video_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'video_sc_generatorDialog', this.path + 'dialogs/video_sc_generator.js' );
  }
 });
 
})(jQuery);