PUBLISH CONTENT MODULE

The Publish Content module allows users to publish and unpublish nodes,
on a per "node type" basis or for all "node types", without granting users
the very broad "administer nodes" permission.

It allows easily to create editor or moderator roles by granting them 
either publishing or unpublishing permissions, or both.

This module is also integrated with the Views module:
you can add a publish/unpublish link on all your views, making it easy
to create lists for reviewers, editors and publishers.


INSTALLATION

Put the module in your drupal modules directory and enable it in admin/modules.

Then, you just need to go to the Drupal permissions page,
and set the various permissions:
- "publish all content": you can publish any node
- "publish 'nodetype' content": you can publish any node whose type is 'nodetype'
- "unpublish all content": you can unpublish any node
- "publish 'nodetype' content": you can publish any node whose type is 'nodetype'
- "un/publish editable content": publish or unpublish nodes where the user has
    full edit permissions of the node concerned (note: check text formats access)


USAGE

A tab button (like Edit and View) 'Publish' or 'Unpublish' should appear on the
node edit and view pages.
Click on 'Publish' to publish and 'Unpublish' to unpublish, it's that simple!

CONFIGURATION

admin/config/content/publishcontent
Here you can disable the display of Publish/Unpublish tabs.
