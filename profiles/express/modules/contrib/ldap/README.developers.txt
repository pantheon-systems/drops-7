

--------------------------------------------------------
Case Sensitivity and Character Escaping in LDAP Modules
--------------------------------------------------------

The function ldap_server_massage_text() should be used for dealing with case sensitivity
and character escaping consistently.

The general rule is codified in ldap_server_massage_text() which is:
- escape filter values and attribute values when querying ldap
- use unescaped, lower case attribute names when storing attribute names in arrays (as keys or values), databases, or object properties.
- use unescaped, mixed case attribute values when storing attribute values in arrays (as keys or values), databases, or object properties.

So a filter might be built as follows:

  $username = ldap_server_massage_text($username, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)
  $objectclass = ldap_server_massage_text($objectclass, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)
  $filter = "(&(cn=$username)(objectClass=$objectclass))";


The following functions are also available:
ldap_pear_escape_dn_value()
ldap_pear_unescape_dn_value()
ldap_pear_unescape_filter_value()
ldap_pear_unescape_filter_value()
