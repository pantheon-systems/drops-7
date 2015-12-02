<?php

/**
 *
 * PERMISSIONS introduced by this module:
 *
 * administer system menus
 *   Allows a user to show/list _all_ menus on the menu overview admin page.
 *   Users without this permission are not shown the content_menu_special_menus
 *   (variable details see below).
 *
 *
 *
 * SYSTEM VARIABLES of this module that can be changed:
 * (There's currently no config interface to set them, but you can do that
 *  manually or programmatically)
 *
 * content_menu_add_existing_content_url
 *   URL of the "select existing content" page to use when creating a new menu
 *   item that is linking to "existing content".
 *
 * content_menu_add_existing_content_view
 *   VIEWNAME[:DISPLAYNAME] of the view (if any) that should be used
 *   when creating a new menu item that is linking to "existing content".
 *   An "nid" field of this field based view[/display] is changed to be used as
 *   "select" link to select existing content for the new menu item to link to.
 *
 * content_menu_special_menus
 *   Array of menu machine names that are considered special/system menus, and
 *   are hence not altered by content_menu, and not shown to users without the
 *   new 'administer system menus' permission.
 *   Default: array('management', 'user-menu', 'navigation', 'devel'));
 *
 * content_menu_alter_all_menus
 *   Boolean flag to state whether all menus should be altered to have to
 *   improved interface, including those listed in the variable
 *   content_menu_special_menus.
 *   Default: FALSE (to let system menus be untouched, 'just in case').
 *
 *
 */


/**
 * Extend the target types for a new menu item.
 *
 * @param $target_types Array
 *   Array of menu item target types that you can extend.
 *   The key is a machine name of a target type (alphanumerical char, _)
 *   The value is an array containing the following entries:
 *   - label => translated label to display in the interface
 *   - handler => full name of a handler function to use, with the signature
 *                ...(&$form, &$form_state, &$item [, $arg_1, ..., $arg_n])
 *   - arguments => array of arg_name=>arg_value (optional),
 *                  each argument is passed to handler callback function.
 *   - clean_menu_save_message => flag whether the existing message
 *                                "Your configuration has been saved"
 *                                should be removed (TRUE) or not (FALSE).
 *
 * @param $context Array
 *   Array with context information. Currently the following key is used:
 *     'menu_name' => machine name of the menu the target types are assembled.
 *
 */
function hook_menu_item_target_types_alter(&$target_types, &$context) {
  $target_types['url'] = array(
    'label' => t('URL'),
    'handler' => 'content_menu_menu_form_handler_url',
    'clean_menu_save_message' => TRUE
  );
}


/**
 * Alter a menu item's form element in the menu item administration.
 *
 * @param $el Array The form element of the menu item to alter.
 */
function _content_menu_menu_item_element_alter(&$el) {
  module_load_include('inc', 'content_menu', 'content_menu.menu_admin');
  _content_menu_menu_item_element_alter($el);
}
