INTRODUCTION
------------

This module allows any user with the "invite new user" permission to send role 
invites to an email address. This module assumes that you are using external 
authentication (such as LDAP or Shibboleth) and that users receiving the invites 
can already log into the website.

Upon receiving the invitation email, the user is directed to the user login 
page. Upon successful authentication, the elevated role is automatically 
granted.

REQUIREMENTS
------------

This module requires the following modules:

* Token (https://drupal.org/project/token)

RECOMMENDED MODULES
-------------------

* LDAP (https://www.drupal.org/project/ldap)
  Protocol to provide external authentication
  
* Shibboleth authentication (https://www.drupal.org/project/shib_auth)
  Another protocol to provide external authentication

INSTALLATION
------------

 * Install as you would normally install a contributed Drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.

CONFIGURATION
-------------

Administration page located at:
 Administration >> Configuration >> People >> Invite
 
NEEDS WORK

TROUBLESHOOTING
---------------

NEEDS WORK

FAQ
---

NEEDS WORK

MAINTAINERS
-----------

Current maintainers: 
 * Kevin Reynen (https://www.drupal.org/u/kreynen)
 * Alex Finnarn (https://www.drupal.org/u/afinnarn)
 
This project has been sponsored by:
 * University of Colorado at Boulder 
   (https://www.drupal.org/u/university-of-colorado-boulder)
