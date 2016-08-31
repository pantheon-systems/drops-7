Panelizer
---------
The Panelizer module allows supported entities to be treated as Panels [1],
giving options for different default displays on a per bundle basis. For
example, this allows each node display to be customized individually.

Panelizer is a descendent and replacement for the "Panel Nodes" module bundled
with Panels; no upgrade path is available as of yet.


Features
--------------------------------------------------------------------------------
* Supports all of Drupal core's included entities - nodes, taxonomy terms, users
  and comments. (More may be available in the future)

* Can be easily extended to support additional entities, e.g. Fieldable Panels
  Panes [2] and Bean [3] includes full support.

* Each entity bundle (content type, vocabulary, etc) may have each view mode
  configured individually, allowing for each to be tailored separately.

* Each entity bundle may optionally have multiple displays defined; if this
  option is enabled, each entity of that type/bundle will have an option to
  select which is used.

* Each entity bundle / view mode combination may its default display controlled.

* Full support for content revisions, and provides integration with both
  Revisioning and Workbench Moderation.


Requirements
--------------------------------------------------------------------------------
CTools v7.x-1.9 or newer.
Panels v7.x-3.6 or newer.


Configuration & Usage
--------------------------------------------------------------------------------
Visit Structure >> Panelizer to enable the module for the entities you need;
these may also be controlled via the entity bundle's settings page, e.g. for an
"Article" content type it may be enabled at the following page:
  admin/structure/types/manage/article
You may need to visit Site Building >> Pages and enable the appropriate pages
for supported entities to use their "Full page override" view mode.

Note that in all cases, modifying Panelizer settings for an entity requires the
corresponding 'update' permission for that entity.

Panelizer operates in four basic modes:

- No Default, No Choice
  In this mode, the given bundle is panelized, but there is no default panel
  associated or selectable. In this case, each entity has a small form on the
  "Customize display" tab that says to 'Panelize it!'. When this button is
  clicked, a default panel is attached to the entity and the display may then
  be customized.

- With Default, No Choice
  In this mode, all entities of the given bundle are given the default panel
  automatically. Users with appropriate privileges may then customize the panel
  for that node. Once customized, the default is no longer applied and changes
  to the default will not propagate downstream.

- No Default, With Choice
  In this mode, all entities of this bundle will be given a selector to choose
  which panel to display on the entity page. The default choice will be
  "No Panel". When a panel has been chosen, users with permission can then
  customize that panel. Once this is done, the default choice will no longer be
  associated with the panel and a choice can no longer be made. The "Reset"
  button on the Panelizer settings tab for that entity can return the entity to
  a default state and restore the choice.

- With Default, With Choice
  Like above, entities will have a selector to choose which panel to use.
  However, unlike above, entities that have never made a selection will
  automatically be given the default panel. All configured entities will have
  some kind of panel, whether it is one of the choices or a customized panel for
  that entity.


Entity View Modes
--------------------------------------------------------------------------------
Panelizer will allow you panelize each view mode individually. One view mode,
the "Full page override" is not actually a view mode - it uses Page Manager to
completely override the output of the page the entity is viewed one. This will
often conflict somewhat with the Default view mode. It is recommended that you
do not panelize both the Default and the Full page override, but instead pick
whichever one you think is most needed. The actually difference between the two
are quite subtle: Placement of the title is different, and the comment form
will not appear in the Default view mode but it will appear in the Full Page
Override.


Permissions
--------------------------------------------------------------------------------
Once Panelizer is enabled for an entity/bundle combination, it may be necessary
to visit People >> Permissions and give users appropriate permissions. All of
the Panelizer tabs have their own permission, and if these are revoked it is
possible to create panelized entities that can only choose panels but not modify
them.


Known Issues / Caveats
--------------------------------------------------------------------------------
Panelizer currently uses the Page Manager to render the panelized entities. At
this time there is no direct support for view modes. This is a desired feature,
though we are somewhat hampered by Drupal only allowing 2 levels of local tasks
(tabs) where configuring for multiple view modes really would prefer a third
level.

Panelizer 7.x-3.x is Revision Aware. This has the downside that duplicating
panels for revisions can generate a lot of extra data. Please keep this in mind
-- it may be necessary to periodically clean up older revisions.


API
--------------------------------------------------------------------------------
Panelizer 7.x-3.x is constructed on an Object Oriented plugin. There is one
plugin per entity type and it MUST be named exactly the same as the entity
type. The easiest way to add Panelizer support for a custom entity is to copy
the node entity. 

As a CTools plugin, you will be required to implement 
hook_ctools_plugin_directory. Then copy node.inc to your plugin directory as
MY_ENTITY_TYPE.inc and modify the name of the handler it uses. Copy
PanelizerEntityNode.class.php to MyModuleEntityMyEntity.class.php -- and make
sure the name of this file matches the handler in your .inc file.

The required implementation pieces are straightforward. You do need to set a
flag if the entity supports revisions so that Panelizer can write the
information.

If the entity does not support bundles it can only panelize the entire entity.


Troubleshooting / Known Issues
--------------------------------------------------------------------------------
* When using older releases of Pathauto it was possible that saving an entity's
  overridden Panelizer display (i.e. nodes, etc) would cause the entity's path
  alias to be reset. This was a bug in Pathauto prior to v7.x-1.3 and can be
  fixed by updating that module to the latest release.
* Revisions handling using Workbench Moderation and the Panels IPE (In-Place
  Editor) is problematic. This problem is being collaborated on in the following
  issue:
  https://www.drupal.org/node/2457113


Credits / Contact
--------------------------------------------------------------------------------
Currently maintained by Damien McKenna [4]. Originally written by merlinofchaos
[5], with many contributions by dww [6] and awebb [7].

Ongoing development is sponsored by Mediacurrent [8].

The best way to contact the author is to submit an issue, be it a support
request, a feature request or a bug report, in the project issue queue:
  https://www.drupal.org/project/issues/panelizer


References
--------------------------------------------------------------------------------
1: https://www.drupal.org/project/panels
2: https://www.drupal.org/project/fieldable_panels_panes
3: https://www.drupal.org/project/bean
4: https://www.drupal.org/u/damienmckenna
5: https://www.drupal.org/u/merlinofchaos
6: https://www.drupal.org/u/dww
7: https://www.drupal.org/u/awebb
8: https://www.mediacurrent.com/
