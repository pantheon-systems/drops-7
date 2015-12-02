/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'icon_sc_generatorDialog', function( editor ) {
  
  var iconList = Drupal.settings.font_awesome_icons;

	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Icon Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
 			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-settings',
				label: 'Icon Settings',

				// The tab contents.
				elements: [
				  {
						// select input field for the icon shape.
						type: 'select',
						id: 'icon-shape',
						label: 'Icon [<a href="http://fortawesome.github.io/Font-Awesome/icons/" target="_blank">see icons</a>]',
						items: iconList,
					},
					
					{
						// select input field for the icon size.
						type: 'select',
						id: 'icon-size',
						label: 'Icon Size',
						items: [ [ 'Regular', 'regular' ], [ 'Large', 'fa-lg' ], [ '2x', 'fa-2x' ], [ '3x', 'fa-3x' ], [ '4x', 'fa-4x' ], [ '5x', 'fa-5x' ] ],
					},
					{
						// select input field for the icon pull.
						type: 'select',
						id: 'icon-pull',
						label: 'Icon Pull',
						items: [ [ 'None', 'none' ], [ 'Left', 'left' ], [ 'Right', 'right' ] ],
					},
					{
						// select input field for the icon color.
						type: 'select',
						id: 'icon-color',
						label: 'Icon Color',
						items: [ [ 'Inherit', 'inherit' ], [ 'Black', 'black' ], [ 'White', 'white' ], [ 'Light Gray', 'light-gray' ], [ 'Medium Gray', 'gray' ], [ 'Dark Gray', 'dark-gray' ], [ 'Gold', 'gold' ] ],
					},
					{
						// select input field for the icon wrapper.
						type: 'select',
						id: 'icon-wrapper',
						label: 'Icon Wrapper',
						items: [ [ 'None', 'none' ], [ 'Circle', 'circle' ], [ 'Square', 'square' ], [ 'Rounded', 'rounded' ] ],
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
				
		  var icon = dialog.getValueOf( 'tab-settings', 'icon-shape' );
			if ( icon )
				icon = ' shape="fa-' + icon + '"';
				
		  var size = dialog.getValueOf( 'tab-settings', 'icon-size' );
		  if ( size ) 
		    size = ' size="' + size + '"';
		    
		  var pull = dialog.getValueOf( 'tab-settings', 'icon-pull' );
		  if ( pull ) 
		    pull = ' pull="' + pull + '"';
		    
		  var color = dialog.getValueOf( 'tab-settings', 'icon-color' );
		  if ( color ) 
		    color = ' color="' + color + '"';
		    
		  var wrapper = dialog.getValueOf( 'tab-settings', 'icon-wrapper' );
		  if ( wrapper ) 
		    wrapper = ' wrapper="' + wrapper + '"';
			 
			editor.insertHtml( '[icon ' + icon + size + pull + color + wrapper + ' /]');
		}
	};
});
