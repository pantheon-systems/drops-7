Provides a method for adding iOS-style badges to menu items. Once enabled, 
go to Administration > Structure > Menus, and click "list links" next to 
the menu containing the target item. Click "edit" next to the item and 
select the badge type to display with the Display Menu Badge select box. 
Currently, there are five badge types available:

 * a test badge (just says "test")
 * a count of unread messages from the PrivateMsg module. 
 * a count of items in the Drupal Commerce shopping cart.
 * a count of pending incoming requests from the User Relationships module
 * a count of pending outgoing requests from the User Relationships module 
 
I will be adding more badge types in the future, and modules can supply 
their own badge types.

As of version 7.x-1.2, you can now create badge types with Views. See this 
screencast for instructions: http://www.youtube.com/watch?v=4AoZQNg5QOI

Developer API
To provide new badges to this module, declare your callback functions by 
implementing hook_menu_badges_options. For example:

function example_menu_badges_options() {
  return array(
    'example_get_unread_count_1' => array(
      'callback' => 'example_get_unread_count',
      'arguments' => array('role' => 'parent', 'relationship_type_id' => 1),
      'label' => t('An example menu badge'),
      'module' => 'example_module',
    ),
    'example_get_unread_count_2' => array(
      'callback' => 'example_get_unread_count',
      'arguments' => array('role' => 'child', 'relationship_type_id' => 1),
      'label' => t('An second example menu badge'),
      'module' => 'example_module',
    ),
  );
}

function example_get_unread_count($arguments) {
  $path = $arguments['path'];
  // Some logic goes here.
  if ($value > 0) {
    return $value;
  }
  return NULL;
}

Note: To hide the badge, it's important to return NULL from your callback 
function. Any other return value will be displayed. (including "false" 
values, so 0 will be displayed) Badges do not have to be numeric. Text 
will also work.