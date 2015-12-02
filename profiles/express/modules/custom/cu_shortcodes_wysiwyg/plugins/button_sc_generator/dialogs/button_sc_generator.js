/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'button_sc_generatorDialog', function( editor ) {
  
  var iconList = Drupal.settings.font_awesome_icons;

	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Button Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-basic',
				label: 'Button Settings',

				// The tab contents.
				elements: [
					{
						// Text input field for the button text.
						type: 'text',
						id: 'button-text',
						label: 'Button Text',
						// Validation checking whether the field is not empty.
						validate: CKEDITOR.dialog.validate.notEmpty( "Button text field cannot be empty" )
					},
					{
						// Text input field for the button url.
						type: 'text',
						id: 'button-url',
						label: 'URL',
						validate: CKEDITOR.dialog.validate.notEmpty( "URL field cannot be empty" )
					}
				]
			},
			{
				// Definition of the Design Settings dialog tab (page).
				id: 'tab-design',
				label: 'Button Design',

				// The tab contents.
				elements: [
				  {
						// Select input for button color.
						type: 'select',
						id: 'button-color',
						label: 'Button Color',
						items: [ [ 'Blue', 'blue' ], [ 'Gold', 'gold' ], [ 'Black', 'black' ], [ 'Gray', 'gray' ], [ 'White', 'white' ] ],
    'default': 'blue',
					},
					{
						// Select input field for the button icon.
						type: 'select',
						id: 'button-icon',
						label: 'Button Icon [<a href="http://fortawesome.github.io/Font-Awesome/icons/" target="_blank">see icons</a>]',
						items: iconList,
					},
					{
						// Select input field for the button size.
						type: 'select',
						id: 'button-size',
						label: 'Button Size',
						items: [ [ 'Regular', 'regular' ], [ 'Large', 'large' ], [ 'Small', 'small' ] ],
					},
					{
						// Select input field for the button size.
						type: 'select',
						id: 'button-style',
						label: 'Button Style',
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
			var buttonURL = dialog.getValueOf( 'tab-basic', 'button-url' );
			var buttonText = dialog.getValueOf( 'tab-basic', 'button-text' );
			
			
			var color = dialog.getValueOf( 'tab-design', 'button-color' );
			if ( color )
				color = ' color="' + color + '"';
				
		  var icon = dialog.getValueOf( 'tab-design', 'button-icon' );
			if ( icon )
				icon = ' icon="fa-' + icon + '"';
				
		  var size = dialog.getValueOf( 'tab-design', 'button-size' );
		  if ( size ) 
		    size = ' size="' + size + '"';
		    
		  var style = dialog.getValueOf( 'tab-design', 'button-style' );
		  if ( style ) 
		    style = ' style="' + style + '"';
			 
			 
			editor.insertHtml( '[button url="' + buttonURL + '"' + color + size + style + icon + ']' + buttonText + '[/button]');
		}
	};
});