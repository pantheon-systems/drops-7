Fieldable Panel Panes supports multiple bundles, which may be managed at
admin/structure/fieldable-panels-panes.

Bundles can also be created in a module via hook_entity_info_alter(). The code
will look something like this:

function MYMODULE_entity_info_alter(&$entity_info) {
  $entity_info['fieldable_panels_pane']['bundles']['my_bundle_name'] = array(
    'label' => t('My bundle name'),
    'pane category' => t('My category name'),
    'pane top level' => FALSE, // set to true to make this show as a top level icon
    'pane icon' => '/path/to/custom/icon/for/this/pane.png',
    'admin' => array(
      'path' => 'admin/structure/fieldable-panels-panes/manage/%fieldable_panels_pane_type',
      'bundle argument' => 4,
      // Note that this has all _ replaced with - from the bundle name.
      'real path' => 'admin/structure/fieldable-panels-panes/manage/my-bundle-name',
      'access arguments' => array('administer fieldable panels panes'),
    ),
  );
}

Fields are then added to your bundle as normal through the Manage Fields and
Display Fields tabs in the UI.

You can use this hook to rename or remove the default bundle but remember that
doing so will break any content currently using that bundle. If you do this
be sure to also fix any content already using it. It is recommended that you
use the bundle management UI in admin/structure/fieldable-panels-panes so you
don't have to maintain this yourself.


Installation notes
------------------
By default a Fieldable Panels Pane type called "Panels pane" will be created. To
skip this, set the variable "fieldable_panels_panes_skip_default_type" to TRUE
prior to installing the module.


A note about view modes
-----------------------
When viewing an FPP object on its own page, e.g.
admin/structure/fieldable-panels-panes/view/1, the 'preview' view mode will be
used if it has been customized, otherwise it defaults to 'default'.
