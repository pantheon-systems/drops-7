Provides autocomplete widget for CCK Link field and additional functionality 
over standard field:

 * Variants of URL filtering (external or internal).
 * Additional filter options by using URL patterns.
 * Autocomplete link field for internal mode.

Use cases
---------
   
 * You want allow users specify link only to several sites 
   (i.e. specify link only to facebook or twitter)
 * You want restrict users to specify only internal or external links
 * You want to use autocomplete suggestion when specifying internal links


Dependencies
------------
 * Link

Install
-------

1) Copy the advanced_link folder to the modules folder in your installation.

2) Enable the module using Administer -> Modules
   (/admin/modules).

3) Create a new link field using Field interface. Visit Administer ->
   Structure -> Content types (dmin/structure/types), then click
   Manage fields on the type you want to add an advanced link field. Select
   "Link" as the field type and "Advanced Link" as the widget type to 
   create a new field.

4) Customize Advanced field settings through field settings form.

Contact
-------

Developed by: www.druplaldriven.com
Email: pr@drupaldriven.com
