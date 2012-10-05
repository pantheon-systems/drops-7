INTRODUCTION
========================
Field Collection Bulkupload allows users to upload multiple files using the robust plupload
interface into file/image fields located within a field collection.  This module may be helpful
if you have a field structure like:

Photo Gallery
  title
  body
  field_collection images (multivalue)
    image (single value)
    description
    photo credit


INSTALLATION
========================
This module requires:
Plupload module + library installed http://drupal.org/project/plupload
Field Collection http://drupal.org/project/field_collection

Install the required modules and clear cache.  You should then see the bulk upload area above any
field collections which contain an image or file field.

ISSUES
========================
If you find bugs or have features which you would like to see in this module please post in the issue
queue on the module page.
