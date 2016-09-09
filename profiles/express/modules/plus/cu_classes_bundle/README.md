Before opening an issue, please review and sign this project's <a href="https://www.clahub.com/agreements/CuBoulder/cu_classes_bundle">Contributor License Agreement</a>.

# CU Classes Bundle

The Classes Bundle incldes a few different components:
- cu_classes_bundle - Defines the secure_permissions and dependencies so the functionality can be added with https://www.drupal.org/project/profile_module_manager
- cu_course - Defines a course entity.  These are primarily used to group classes for import and display.
- cu_class - Defines the class entity.
- cu_class_fields - Feature export of fields.  Machine names match XML element names in API.  Additional fields can be added at "admin/structure/class/manage/fields"
- cu_class_views - Provides "courses/search"
- cu_class_views_admin - Provides "admin/content/classes"
- cu_class_import - Provides UI to import class data from UIS or Primer

# Getting Started

Configure importer at admin/config/system/class-import

![import ui](https://github.com/CuBoulder/cu_classes_bundle/blob/master/cu_class_import_ui.png)

- **Base URL of API** - Used to switch between dev and test instances of API.
- **API Username and Password** - Provided by UIS. This is not a restful API and requires a POST that includes authentication. This is also used to track concurrent requests. UIS reserves the option of disabling API requests for any account disrupting the service, but it is unclear how this is monitored, who would be contacted, or what steps would be required before an account would be unblocked.
- **Import Delay** - This adds a delay between each request during batch import. This is necessary for both the API and Primer, but for different reasons.  On the initial import, saving each course or class to the database adds a delay. But when the import is only diff'ing a cached class entity with the API or Primer response, it can process these fast enough to exceed the rate limits set by UIS for the API or OIT for the Primer running on Express.
- **Use Primer** - A Primer can be created for any class data using PSQuery export by enabling the CU Class Primer Bundle on a Drupal site.  The Primer bundle can be enabled on the same site as the Classes bundle, but that is NOT recommended.  The option to toggle between the Primer and API allows both to remain configured.
- **Show Debug Messages** - When enabled, the location the import is trying to pull from is shown as well as whether a course or class was created or updated. During batch imports, this can generate thousands of messages.

Regarldess of whether you are using the UIS API or a Primer, you start the process of importing with courses.  Import courses by subject and term at courses/import

Once courses are populated, you can import classes for every course Drupal is aware of at class/import

Both class and course provide batch imports at courses/import/batch and class/import/batch.  

# Published vs. Unpublished

[This documentation is avialable on Web Central](https://www.colorado.edu/webcentral/tutorials/managing-classes).

# Cancelled Classes

By default both active and cancelled classes are imported. Cancelled classes include a date in the CANCEL_DT value. Cancelled classes are still returned in the search for 7 days and available via direct links for 14 days. This is to prevent confusion when looking at a specific class one day and is no longer there the next.  There is also an option in both the course search (courses/search) and course management (admin/content/classes) interfaces to include or exclude cancelled classes.

Cancelled classes can be unpublished manually or in bulk.

