Link Badges is an API module that allows developers to add iOS-style badges to 
links rendered by theme('link') or the l() function. These are useful for 
things like unread counts. 

Once the module is enabled, badge values can be specified by adding either a 
value or a callback function to the link options array. For example:

Value:
l('Some text', 'some/path', array('link_badge' => array('value' => 10)));

Callback function:
l('Some text', 'some/path', array('link_badge' => array(
                                                    'callback' => 'example_callback_function', 
                                                    'arguments' => array('some_arg' => 'some value', 'another_arg' => 'another_val'),
                                                  )
                                  )
);

The callback function would look like:

function example_callback_function($arguments) {
  $value = get_some_value($arguments['some_arg']);
  if ($value > 0) {
    return $value;
  }
  return NULL;
}

Note: To hide the badge, it's important to pass NULL as the value or return NULL from 
your callback function. Any other return value will be displayed. (including "false" 
values, such as 0) Badges do not have to be numeric. Text will also work.