/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'box_sc_generatorDialog', function( editor ) {
  
    
	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Box Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
 			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-settings',
				label: 'Box Settings',

				// The tab contents.
				elements: [
				  
					{
						// Text input for box title.
						type: 'text',
						id: 'box-title',
						label: 'Title (optional)',
					},
					{
					// Text input for box text.
						type: 'textarea',
						id: 'box-text',
						label: 'Box Text',
						// Validation checking whether the field is not empty.
						validate: CKEDITOR.dialog.validate.notEmpty( "box Content Text field cannot be empty" )
					},
					{
						// Select input for box style.
						type: 'select',
						id: 'box-color',
						label: 'Box Color',
						items: [ [ 'Black', 'black' ], [ 'Dark Gray', 'darkgray' ], [ 'Light Gray', 'lightgray' ], [ 'White', 'white' ] ],
    'default': 'lightgray',
					},
					{
						// Select input for box color.
						type: 'select',
						id: 'box-style',
						label: 'Box Style',
						items: [ [ 'Filled', 'filled' ], [ 'Border', 'border' ] ],
    'default': 'filled',
					},
					{
						// Select input for box style.
						type: 'select',
						id: 'box-float',
						label: 'Float',
						items: [ [ 'None', 'none' ], [ 'Right', 'right' ], [ 'Left', 'left' ] ],
						'default': 'none'
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
			var boxTitle = dialog.getValueOf( 'tab-settings', 'box-title' );
			if ( boxTitle ) 
		    boxTitle = ' title="' + boxTitle + '"';
			
			var boxContent = dialog.getValueOf( 'tab-settings', 'box-text' );
			var boxColor = ' color="' + dialog.getValueOf( 'tab-settings', 'box-color' ) + '"';
			var boxStyle = ' style="' + dialog.getValueOf( 'tab-settings', 'box-style' ) + '"';
			var boxFloat = ' float="' + dialog.getValueOf( 'tab-settings', 'box-float' ) + '"';
			 
			 
			editor.insertHtml( '[box' + boxTitle + boxColor + boxStyle + boxFloat + ']' + boxContent + '[/box]');
		}
	};
});