Requirements:
=============

1. CU-Boulder's LDAP only allows authentication from specific IPs, thus (if your site
is outside of the campus infrastructure), you must have the IP of the server
making the connections added to our systems. Until you do so, using this
feature as-is will result in "Sorry, unrecognized username or password"
during authentication attempts. You can contact the IT Security Office in
order to get your IP approved for this use through help@colorado.edu or by
contacting "5-HELP" at 303.735.4357.

Due to these restrictions, you will not be able to authenticate via LDAP
from a local development sandbox unless you are on-campus.

2. It is a requirement when doing LDAP Authentications to send logs of the
following to the IT Security Office:
 - "Session opened"
 - "Session closed"
 - "LDAP bind failure"
 - "Login attempt failed"
 - "Deleted user"
 - "New user"

You need to contact the ITSO with the contact information above when you
enable this feature. The ITSO's standard method for receiving this required
logging information is via syslog, so you will need to configure your
server(s) to syslog this information, filter it appropriately and send it to
ITSO's log archive servers or negotiate another method for supplying them
with these logs. Below is an example of how to do this with Linux rsyslog.

if $programname == 'drupal' and
($msg contains 'Session opened for' or
$msg contains 'Session closed for' or
$msg contains 'LDAP bind failure for' or
$msg contains 'Deleted user:' or
$msg contains 'Login attempt failed' or
$msg contains 'New user:')
then @IP-ADDRESS-SUPPLIED-TO-YOU-BY-ITSO

3. This module configures the LDAP Drupal module to use "LDAP Strict Mode" -
which requires all user accounts within Drupal to authenticate to LDAP.
While it is possible to alter this to use "LDAP Mixed Mode," this is usually
highly discouraged unless there is a specific need for both LDAP enabled
accounts and non-LDAP accounts.

4. In order to use this feature, your site must be SSL enabled - passwords
must be transfered via https - this feature comes with configurations for
the securepages module to ensure this. If you are using CU servers, ITSO
can provide SSL certificates free of charge.

5. Finally, this feature configures the Drupal LDAP module to allow anybody
with an "identikey" (anybody who can authenticate via LDAP) to become an
"authenticated" user on the site - this is by design and sites' permissions
should keep this in mind. Authenticated users should not be "privileged"
accounts and instead, new roles should be added to Drupal for granting
additional permissions.
