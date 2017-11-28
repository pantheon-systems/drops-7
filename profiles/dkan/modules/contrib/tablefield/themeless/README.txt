# TableField Themeless #

Provides themeless output of a node's tablefield on the path 'node/%/themeless'.


## INSTALLATION ##

- Enable the submodule at ../admin/modules.


## GET STARTED ##

- Go to ../admin/structure/types/manage/[your-content-type]/display/themeless
  and make sure it includes a TableField field.
- Choose the desired format and format settings.
- Update.
- Save.
- Visit a content page at ../node/%nid/themeless .


## TO KEEP IN MIND ##

- Only the first found TableField field will be included in the output (also
  multivalue).
- Enable https://www.drupal.org/project/subpathauto to have URLs with aliases
  accessible for the themeless output, e.g. ../my/custom/alias/themeless.
