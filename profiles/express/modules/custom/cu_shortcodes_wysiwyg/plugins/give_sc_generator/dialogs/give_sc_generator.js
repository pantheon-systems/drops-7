/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'give_sc_generatorDialog', function( editor ) {
  
    
	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Give Button Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
 			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-settings',
				label: 'Give Button Settings',

				// The tab contents.
				elements: [
				  {
						// Select input for button color.
						type: 'select',
						id: 'give-color',
						label: 'give Color',
						items: [ [ 'Dark', 'dark' ], [ 'Light', 'light' ], [ 'Gold', 'gold' ] ],
    'default': 'dark',
					},
					{
					// Text input for button text.
						type: 'text',
						id: 'give-text',
						label: 'Give Button Text',
						// Validation checking whether the field is not empty.
						validate: CKEDITOR.dialog.validate.notEmpty( "Button text field cannot be empty" )
					},
					{
						// Text input for button url.
						type: 'text',
						id: 'give-url',
						label: 'URL',
						validate: CKEDITOR.dialog.validate.notEmpty( "URL field cannot be empty" )
					},

					{
						// Select input for button size.
						type: 'select',
						id: 'give-size',
						label: 'Give Button Size',
						items: [ [ 'Regular', 'regular' ], [ 'Large', 'large' ], [ 'Small', 'small' ] ],
					},
					{
						// Select input for button style.
						type: 'select',
						id: 'give-style',
						label: 'Give Button Style',
						items: [ [ 'Regular', 'regular' ], [ 'Full', 'full' ] ],
					},
					
					
					
				]
			},

		],

		// This method is invoked once a user clicks the OK button, confirming the dialog.
		onOk: function() {

			// The context of this function is the dialog object itself.
			// http://docs.ckeditor.com/#!/api/CKEDITOR.dialog
			var dialog = this;

			// Get Fields	
			
						
			var buttonURL = dialog.getValueOf( 'tab-settings', 'give-url' );
			var buttonText = dialog.getValueOf( 'tab-settings', 'give-text' );
			
			var color = dialog.getValueOf( 'tab-settings', 'give-color' );
			if ( color )
				color = ' color="' + color + '"';
				
		  var size = dialog.getValueOf( 'tab-settings', 'give-size' );
		  if ( size ) 
		    size = ' size="' + size + '"';
		    
		  var style = dialog.getValueOf( 'tab-settings', 'give-style' );
		  if ( style ) 
		    style = ' style="' + style + '"';
			 
			 
			editor.insertHtml( '[give url="' + buttonURL + '"' + color + size + style + ']' + buttonText + '[/give]');
		}
	};
});