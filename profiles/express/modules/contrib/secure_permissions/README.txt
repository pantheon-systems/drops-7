/**
 * @file
 * README file for Secure Permissions.
 */

Secure Permissions
Disables the user interface for creating and assigning roles and permissions.

CONTENTS
--------
1.    Use case
2.    Installation
3.    Exporting settings to code
4.    Configuring the module
5.    API Hooks
5.1  hook_secure_permissions_roles()
5.2  hook_secure_permissions($role)
6.    To Do

Secure Permissions is an advanced security module for Drupal 7. Please
read this document before continuing.

This module was inspired by some claims regarding superior security of the
Plone platform. See, in particular, 'Problem A2: Broken Access Control' at
http://plone.org/products/plone/security/overview/security-overview-of-plone

The module was inspired by @djay75 via Twitter.

----
1. Use case

This module is designed for cases where you want control of Roles and
Permissions only in a development environment. When fully enabled, this module
will make it so that the live site cannot have its permissions modified, except
through code.

It may be sufficient for most users to simply enable this module on the live
site, and to disable it when it is no longer needed.

----
2. Installation

Before installing this module you should configure the site Roles and
Permissions as you see fit. After installing and configuring this module,
changes to these settings can only be made through code.

On installation this module will have two immediate effects:

  1. Permissions will no longer be editable through the user interface.
  2. Roles will no longer be editable through the user interface.

On many sites, this is sufficient. However, for advanced security, you
have several additonal options. See sections 3 and 4 for details.

The module will also add a permission to your site, 'Export permission
definitions'. This permission should be given to trusted roles before you
continue.

Users with this permissions may configure this module and may export site
Roles and Permissions to code.

----
3. Exporting settings to code

Give your account the 'Export permission definitions' permission defined by the
module or log in as the Site Maintenance user.

Find the link under People and Permissions >> Secure Permissions

Click the Export Permissions tab. It should take you to a form with two text
areas, filled with PHP code.

The Secure permissions module stores the permissions in a module (file) that is
inaccessible through the user interface.

You now need to create and enable that module in 4 easy steps.

   1. Create directory. cd to /sites/all/modules and issue the command:
       mkdir secure_permissions_data
   2. Create 2 empty files. cd to /sites/all/modules/secure_permissions_data and
       issue the command: touch secure_permissions_data.info secure_permissions_data.module
   3. Copy data. Copy the text from the fields below into the respective files you just
       created using the tools of your choice.
   4. Enable the module. Navigate to admin/build/modules/list and enable your new module.

To change permissions with the module enabled, you must now edit your
secure_permissions_data.module file.

Now you are ready to configure the Secure Permissions module to run.

After editing the file navigate to /admin/user/secure_permissions/view, select
'Load permissions from code'and click 'Save configuration' to update the permissions.

You may rename the module; remember to rename all the functions.

Note that if you have set an administrative role, the permissions for that role will not
be exported.

----
4. Configuring the module

For advanced features of this module, you must export your Roles and Permissions
to code. Since this cannot be done before the module is installed, the module
will not enforce its rules until you tell it to do so.

To activate the module, navigate to:

  Administer > Configuration and Modules > People and Permissions > Secure
  Permissions

Here, you can tell the Secure Permissions module how to behave. You have eight
options that can be set. These are split into two groups, those that control the
User Interface of Drupal, and those that affect how permissions are loaded from
code via the API.

  USER INTERFACE SETTINGS

  'Disable permissions and roles forms'
  Check to make the Roles and Permissions forms unchangeable. Users may
  be able to view them, but cannot edit or submit them. Default is OFF.
  You should enable this setting after granting your account the ability to
  access 'Export permission definitions'.

  'Show permissions page'
  Check to allow the Permissions page to be viewed by administrators.
  Disabling this option will prevent users from seeing permission definitions
  at all. Default is ON.

  'Show roles page'
  Check to allow the Roles page to be viewed by administrators.
  Disabling this option will prevent users from seeing role definitions
  at all. Default is ON.

  'Display permissions updates'
  Check to display messages when Secure Permissions reset site permissions.
  Default is ON.

  API SETTINGS

  'Load permissions from code'
  Check to activate the module's advanced features.
  When set, the module will rebuild Roles and Permissions every time that
  a module is enabled or disabled. Checking this option means that all
  site Roles and Permissions will be managed in code. Default is OFF.

  NOTE: none of the following settings will be in effect if 'Load permissions
  from code' is not enabled. Using these features is not required, however.

  'Reload default permissions on rebuild'
  Check to have the module load Drupal's default permissions for the anonymous
  and authenticated user roles when permissions are rebuilt. Default is OFF.

  'Use administrative role'
  Check to include an administrative role from the site.
  The 'administrator' role ships with Drupal, and has all site permissions. If
  you uncheck this option, this role will be deleted. Default is ON.

  'Administrative role name'
  Set to the name of the administrative role you wish to use.
  If 'Use administrative role' is disabled, this value is not used.
  Default is 'administrator'. Normally, you should not change this value.
  NOTE: If using translations, this string should not be translated through
  this setting.

----
5. API hooks

The module functions by using two API hooks, described below. To use these
functions you must place them in a custom module file and name them properly.

The export function will generate these hooks for you. The API is described
here for developers.

----
5.1 hook_secure_permissions_roles()

Defines the roles used by a site. These are keyed by the uniqueness of the role
name, since we cannot guarantee the role id used by various sites.

WARNING: If you do not implement this hook, the module will reset your site
roles to the roles that ship with Drupal's default install.

Note that the module implements this hook to ensure that the 'anonymous user'
and 'authenticated user' roles always exist.

If the 'Use administrative role' is set to TRUE, the module will also maintain
an administrative role that has all site permissions.

The hook returns a simple positional array of unique role names.

  function example_secure_permissions_roles() {
    return array(
      'editor',
      'producer',
    );
  }

----
5.2 hook_secure_permissions($role)

Defines the permissions assigned to each role. Typically, you will implement
all permissions for your site in this hook.

This hook takes a $role string as an argument. You should respond with the
appropriate permissions grants for that role. You should only return grants
that are TRUE.

  function example_secure_permissions($role) {
    $permissions = array(
      'anonymous user' => array(
        'access content',
        'use text format 1',
      ),
      'authenticated user' => array(
        'access comments',
        'access content',
        'post comments',
        'post comments without approval',
        'use text format 1',
      ),
      'editor' => array(
        'bypass node access',
        'administer nodes',
      ),
      'producer' => array(
        'create page content',
        'edit any page content',
      ),
    );
    if (isset($permissions[$role])) {
      return $permissions[$role];
    }
  }


NOTE: The use of isset() is recommended here, since the hook will fire
once per role, and it is possible that your module will not reply in all cases.

NOTE: If configured to do so, the module will return the default permissions
defined by Drupal's installer. Disable the 'Reload default permissions on
rebuild' setting to disable this behavior.
