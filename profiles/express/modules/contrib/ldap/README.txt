


------------------------------------
When Uninstall Fails...or you need to make sure you have a fresh install
------------------------------------

1.  Remove ldap module directory
2   Execute the following sql.  Beware this will likely remove other ldap_* modules not in the ldap package.

DELETE FROM variables WHERE name like 'ldap_%';
DELETE FROM system WHERE name like 'ldap_%';
DROP TABLE ldap_authorization;
DROP TABLE ldap_servers;
DELETE FROM authmap WHERE module like 'ldap_%';  -- this will disassociate existing user from ldap without removing the users
