BUEditor Plus replaces the default method of selecting which editor to display
with an all new system. Editors are displayed based upon field settings and
profiles.

FEATURES:
  - Create multiple profiles to control which editor is used.
  - Editors are changed on the fly dependent upon input format.
  - Assign a different profile to each text field.
  - Editor path visibility and element ID disable still works, allowing to
    help set up an alternative editor on each profile.

DEPENDENCIES:
  - BUEditor

TO USE:
  - Go to admin/modules and enable BUEditor Plus under the Content Authoring
    category.
  - Go to admin/config/content/bueditor and click the "Add new profile" link.
  - Give your profile a name that makes it easy to identify by you.
  - Specify the default and alternative editor for each input format.
  - Save the new profile.

  Once you have your profiles created, then you need to assign the profiles to
  the text fields defined in your entities and bundles.

TO ASSIGN A PROFILE TO THE ARTICLE NODE TYPE:
  - Go to admin/structure/types/manage/article.
  - Click the Edit link for the Body field.
  - Under the settings section select the profile you wish to use in the BUEditor
    Profile select.

SPECIAL GLOBAL PROFILE:
  Any profile can be used as a global profile. This allows you to set the
  special "global profile" to any fields and changes to that single profile
  will migrate through all fields with the global profile assigned to it.

NOTE:
  This module will only allow BUEditor to appear on fields that have text
  processing enabled. This also helps with security as any user inputed content
  should always be sanitized through Drupal's input filters.
