Simple Google Maps module

This module is about the simplest way to get a Google Map, or a link to a
Google Map, displayed with your content. It simply provides a field formatter
for Drupal 7 text fields. With this formatter, you can enter a single-line
address that Google Maps would recognize into a plain text field, such as:
   100 Madison Ave, New York, NY
And then on your Manage Display screen (or in Views field setup), you can choose
to display the field with an embedded iframe Google Map, a link to a Google Maps
map, or both; with or without the original address text.

No Google Maps API, JavaScript downloads, etc. are required. This just uses
Google Maps' iframe embedding capability to embed a map at a given address that
Google Maps can recognize, or to make a link to Google Map.

To install and use:
- Upload/unzip to your Drupal 7 sites/all/modules directory (or
  sites/default/modules or whatever).
- Enable the module.
- Add a plain Text field to your content type. You probably should add some
  Help text to the field to explain that a one-line address that Google Maps can
  recognize needs to be entered, and that the output will be formatted with a
  map (or a link or both, depending on how you are using this field).
- On the Manage Display screen, or when adding this field to Views, choose the
  provided mapping formatter.
- If desired, click the button to change the field display settings.

Note: There are no field validation steps in this module, and it just uses a
regular Text field rather than defining its own field (this is the "simple" part
of this module). The display settings let you choose the size of the map, the
text for the map link, the map zoom level (applies to both embedded and linked
map), and to turn on/off the various things you can display. That's pretty much
it (remember: simple is in the name!).
