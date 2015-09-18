ABOUT Panelizer 7.x-3.x

Panelizer allows you to treat supported entity as panels, giving options
for different default panels on a per bundle (node type/taxonomy vocabulary)
basis.

Panelizer currently contains support for Node, Taxonomy Term and User
entities. However, this is constructed as a plugin and more may be available
in the future.

INSTALLING

Install this through the normal Drupal method of putting the module in
sites/all/modules and going to admin/modules to activate it.

It requires Panels and Page Manager.

INITIAL CONFIGURATION

Visit Configuration >> Panelizer to enable the module for the entities you
need. You may need to visit Site Building >> Pages and enable the appropriate
pages for supported entities.

Note that in all cases, modifying Panelizer settings for an entity requires
update privileges for that entity.

Panelizer operates in four basic modes:

- No Default, No Choice
  In this mode, the given bundle is panelized, but there is no default panel
  associated or selectable. In this case, each entity has a small form on
  the Panelizer tab that says to 'Panelize it!'. When this button is clicked,
  a default panel is attached to the entity, and the panel may then be
  customized.

- With Default, No Choice
  In this mode, all entities of given bundle are given the default panel
  automatically. Users with appropriate privileges may then customize the
  panel for that node. Once customized, the default is no longer applied
  and changes to the default will not propagate downstream.

- No default, With Choice
  In this mode, all entities will be given a selector to choose which
  panel to display on the entity page. The default choice will be 
  "No Panel". When a panel has been chosen, users with permission can then
  customize that panel. Once this is done, the default choice will no
  longer be associated with the panel and a choice can no longer be made.
  The "Reset" button on the Panelizer settings tab for that entity can
  return the entity to a default state and restore the choice.

- With default, With Choice
  Like above, entities will have a selector to choose which panel to use.
  However, unlike above, entities that have never made a selection will
  automatically be given the default panel. All configured entities will 
  have some kind of panel, whether it is one of the choices or a customized
  panel for that entity.

VIEW MODES

Panelizer will allow you panelize each view mode individually. One view mode,
the "Full page override" is not actually a view mode; it uses Page Manager to
completely override the output of the page the entity is viewed one. This will
often conflict somewhat with the Default view mode. It is recommended that you
do not panelize both the Default and the Full page override, but instead pick
whichever one you think is most needed. The actually difference between the two
are quite subtle: Placement of the title is different, and the comment form
will not appear in the default view mode but it will appear in the full page
override.

PERMISSIONS

Once Panelizer is enabled for an entity/bundle combination, it may be
necessary to visit People >> Permissions and give users appropriate
permissions. All of the Panelizer tabs have their own permission, and
if these are revoked it is possible to create panelized entities that can
only choose panels but not modify them.

CAVEATS

Panelizer currently uses the Page Manager to render the panelized entities.
At this time there is no direct support for view modes. This is a desired
feature, though we are somewhat hampered by Drupal only allowing 2 levels
of local tasks (tabs) where configuring for multiple view modes really would
prefer a third level.

Panelizer 7.x-3.x is Revision Aware. This has the downside that duplicating
panels for revisions can generate a lot of extra data. Please keep this in
mind -- it may be necessary to periodically clean up older revisions. Panels
will not duplicate a display if it thinks the display was not changed,
however.

API

Panelizer 7.x-3.x is constructed on an Object Oriented plugin. There is one
plugin per entity type and it MUST be named exactly the same as the entity
type. The easiest way to add Panelizer support for a custom entity is to 
copy the node entity. 

As a CTools plugin, you will be required to implement 
hook_ctools_plugin_directory. Then copy node.inc to your plugin directory
as MY_ENTITY_TYPE.inc and modify the name of the handler it uses. Copy
PanelizerEntityNode.class.php to MyModuleEntityMyEntity.class.php -- and
make sure the name of this file matches the handler in your .inc file.

The required implementation pieces are straightforward. You do need to set
a flag if the entity supports revisions so that Panelizer can write the
information.

If your entity does not support bundles, you can only panelize the entire
entity.

Future functionality
  - Implement panel subtabs. i.e, allow node/27/arbitrarylink to be a subtab
    of an entity, using Panelizer.
  - Allow some other way of panelizing entities other than bundles. Either
    that or find a contrib module to allow users to have bundles.
