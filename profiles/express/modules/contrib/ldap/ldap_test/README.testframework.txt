

Summary of simpletest framework for LDAP_* modules


Configuration Sources for LDAP Simpletests:

-- ldap_test/<module_name>.conf.inc (e.g. ldap_servers.conf.inc) contain functions such as ldap_test_ldap_servers_data() that return arrays of configuration data keyed a test id.
-- ldap_test/test_ldap/<ldap data id> (e.g. ldap_test/test_ldap/hogwarts) contain the data used to populate the ldap.  The stucture of the actual ldap array depends on which server configuration if driving it.  For example if the ldap server configuration has a memberof attribute, the memberof attribute will be populated in the users.
