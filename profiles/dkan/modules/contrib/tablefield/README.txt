# TableField #

Provides a simple, generic form/widget to input tabular data. The form allows
the user to select the number of rows/columns in the table, then enter the data
via textfields. Since this is a field it is revision capable,multi-value
capable and has integration with Views.


## INSTALLATION ##

- Copy tablefield directory to ../sites/all/modules.
- Enable module at ../admin/modules.


## GET STARTED ##

Add a tablefield to any entity:
- For nodes at ../admin/structure/types. Then click 'manage fields' for the
  desired content type.
- For users at ../admin/config/people/accounts/fields.
- For taxonomies at ../admin/structure/taxonomy. Then click 'edit vocabulary'
  for the desired vocabulary.
- For files using the File Entity (fieldable files) module
  (https://www.drupal.org/project/file_entity) at
  ../admin/structure/file-types. Then click 'manage fields' for the desired
  file type.


## FEATURES ##

### Per table (in Edit mode) ###

- Change number of rows/columns per table (even within multi-value instances).
  Optionally restict that to users with the permission 'Rebuild tablefied'
  (see field settings below).
- Rows can be rearranged with drag and drop.
- Upload a CSV file to be converted into a table on the fly. The used
  separator can be defined at ../admin/config/content/tablefield.
- Copy paste tables e.g. from Excel.
- Add a table caption.
- Easily remove tables from a multi-value field with the button. Just install
  and enable https://www.drupal.org/project/multiple_fields_remove_button.


### Per field (field settings) ###

For nodes field settings for some of the below options can be found through
'manage fields' at ../admin/structure/types. For others entities see above.

- Restrict rebuilding to users with the permission "rebuild tablefield".
- Lock table header so default values cannot be changed.
- Input type: textfield or textarea.
- Maximum cell length in characters (integer >= 1, max 999999).
- Table cell processing (radios).
  * Plain text
  * Filtered text (user selects input format)
- Default value for example to create a header on new tables. Can be locked
  selecting the appropriate checkbox mentiond above.


### Per display mode (display settings) ###

For nodes display settings for the below options can be found through
'manage display' at ../admin/structure/types. For others entities see above
under 'GET STARTED'. Display options can be set per view mode e.g. 'Default' or
'Teaser'.

The options of all of the below settings are 'Yes'/'No' (checkbox) unless
stated otherwise.


#### Tabular view ####

- Sticky header
- Sortable (install and enable https://drupal.org/project/tablesorter)
- Hide first row
- Hide empty columns ignoring column header
- Trim empty trailing columns
- Trim empty trailing rows
- Hide empty rows
- Hide empty columns
- Show link to export table data as CSV depending on permission


#### Raw data (JSON or XML) ####

This format is intended to provide table data as a service:

- directly by enabling the submodule TableField Themeless. It provides
  themeless output of a node's tablefield on the path 'node/%/themeless' (HTML,
  JSON or XML).
- using a View (e.g. with https://www.drupal.org/project/views_datasource) that
  outputs JSON or XML. The Views field settings includes 'Formatter'.
- using a custom service (e.g. with https://www.drupal.org/project/services).


When choosing 'Raw data (JSON or XML)' it shows the below options:

- Wrapper for table data (if applicable)
  * tabledata (fixed string)
  * Label: [the actual field label]
  * Machine name: [the actual field machine name without field_ prefix]
  * To provide a custom value install and enable the 'Select (or other)'
    module (https://www.drupal.org/project/select_or_other).
- Use first row/column values as array keys (if not empty). (select)
  * No
  * Header only
  * Both first row and first column (two headers)
- Row identifier key
- Vertical header (first column instead of first row)
- Table data only (no caption)
- Encode numeric strings as numbers (for JSON only)
- XML instead of JSON
- How to make field values XML safe? (for XML only)
  * Convert special characters to HTML entities (htmlspecialchars)
  * Represent field values that contain special characters as a CDATA section
  * Represent all field values as a CDATA section

Using this format for a display mode for a node content type will display the
JSON or XML in pretty print. More logical is to use the regular 'Tabular view'
for the node display and use the 'Raw dat (JSON)' diplay only for a service.
That would expose the data of published tables on a site automatically as a
service.


### Themeless output ###

Enabling the submodule TableField Themeless provides themeless output of a
node's tablefield on the path 'node/%/themeless' (HTML, JSON or XML). This is
useful to embed the table's HTML elsewhere (as an iFrame) or to provide the
table data as a service (JSON or XML) directly without the need of Views or a
Service.

- Enable the submodule TableField Themeless.
- Go to ../admin/structure/types/manage/[your-content-type]/display.
- Uncollapse the CUSTOM DISPLAY SETTINGS and select 'Themeless'.
- Save.
- Now a new display mode appears besides Default and Teaser. Go and configure.
- Save.

Install and enable https://www.drupal.org/project/subpathauto to have the
themeless output available under the alias path like 'some/alias/themeless'
besides 'node/%/themeless'.


## CREDITS ##

- Original author: Kevin Hankens (http://www.kevinhankens.com)
- Maintainer: vitalie (https://www.drupal.org/u/vitalie)
- Maintainer: jenlampton (https://www.drupal.org/u/jenlampton)
- Maintainer D7: Martin Postma (https://www.drupal.org/u/lolandese)
