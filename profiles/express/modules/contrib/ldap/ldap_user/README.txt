

LDAP User Module

The core functionality of this module is provisioning and storage of an ldap identified Drupal user based on ldap attributes.  In Drupal 6 this functionality was in ldap_synch, ldap_provision, ldap_profile, etc. This has been moved to one module centered on the Drupal User - LDAP User Entry data.

-----------------
hooks relating ldap_user entities and drupal user entities
-----------------

-- hook_user_create, hook_user_update, hook_user_delete should look for ldap_user entity with matching uid and deal with ldap_user entity appropriately.
-- hook_ldap_user_create, hook_ldap_user_update, hook_ldap_user_delete should do appropriate things to user entity


----------------------
ldap user module use cases:
----------------------

Provide interface for manually working with LDAP identified user data.
- create ldap indentified user (via ldap_user or user form).  Perhaps on submission, drupal.user is created and.
- edit ldap identified user (go directly to ldap_user entity and edit and/or add link to edit ldap_user to user forms)
- associate existing user with ldap (add prepopulate link from user page to create ldap_user page.)


Populate/Synch/Create/Update/Remove LDAP identified Drupal users via batch, hook responses, webservices


----------------------
Diagrams
----------------------

Sequence Diagram of $ldapUserConf->provisionDrupalAccount() method.
http://www.gliffy.com/go/publish/3664260/
