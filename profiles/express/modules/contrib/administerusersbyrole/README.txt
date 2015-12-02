SUMMARY

This module allows site builders to set up fine-grained permissions for
allowing "sub-admin" users to edit and delete other users — more specific
than Drupal Core's all-or-nothing 'administer users' permission. It also
provides and enforces a 'create users' permission.

CORE PERMISSIONS

Administer users
  DO NOT set this for sub-admins.  This permission bypasses all of the
  permissions in "Administer Users by Role".

View user profiles
  Your sub-admins should probably have this permission.  (Most things work
  without it, but for example with a View showing users, the user name
  will only become a link if this permission is set.)

NEW PERMISSIONS

Access the users overview page
  See the list of users at admin/people.  Only users the can be edited will
  be shown.  (This restriction is necessary to ensure that batch operations
  are safe; in the unusual case of cancel permission but not edit permission,
  then you could create a View for the sub-admin to have a list of users.)

Create new users
  Create users, at admin/people/create.

Edit users with no custom roles
  Allows editing of any authenticated user that has no custom roles set.

Edit users with no custom roles
  Allows editing of any authenticated user with the specified role.
  To edit a user with multiple roles, the sub-admin must have permission to
  edit ALL of those roles.  ("Edit users with no custom roles" is NOT needed.)

The permission for cancel work exactly the same as those for edit.

