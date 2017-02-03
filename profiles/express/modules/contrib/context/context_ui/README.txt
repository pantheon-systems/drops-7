
Context UI
----------
Context UI provides an administrative interface for managing and editing
Contexts. It is not necessary for the proper functioning of contexts once they
are built and can be turned off on most production sites.


Requirements
------------
- Context, Context UI modules enabled (`admin/modules`)


Basic usage
-----------
As a site administrator you can manage your site's contexts at
`admin/structure/context`. The main page will show you a list of the contexts
on the site and give you some options for managing each context.

When editing or adding a new context, you will be presented with a form to
manage some basic information about the context and then alter its conditions
and reactions.

- `name`: The name of your context. This is the main identifier for your context
  and cannot be changed after you've created it.
- `description`: A description or human-readable name for your context. This is
  displayed in the inline editor if available instead of the name.
- `tag`: A category for organizing contexts in the administrative context
  listing. Optional.

**Conditions**

When certain conditions are true, your context will be made active. You can
customize the conditions that trigger the activation of your context.

- **Condition mode**: you can choose to have your context triggered if **ANY**
  conditions are met or only active when **ALL** conditions are met.
- **Adding/removing conditions**: you can add or remove to the conditions on
  your context using the conditions dropdown.
- **Individual settings**: most conditions provide a simple form for selecting
  individual settings for that condition. For example, the node type condition
  allows you to choose which node types activate the context.

**Reactions**

Whenever a particular context is active, all of its reactions will be run.
Like conditions, reactions can be added or removed and have settings that can
be configured.

- **Reaction Block Groupings**: You can influence what "group" a block appears
  in when listing all blocks available to be added to a region.  This is done
  by specifying $block->context_group via hook_block_info.  If no group is
  specified it will default to the module name, but if a group is specified
  it will be grouped under that group name.



Using the inline editor
-----------------------
The inline editor allows you to manage the block reaction for active
contexts within the context of a page rather than through the admin
interface. This can also be helpful when managing block ordering among
multiple contexts.

1. As an administrative user go to `admin/structure/context/settings`.
2. Check the 'Use Context Editor Dialog' block and save. You should also
   check the show all regions box.
3. When viewing a page with one or more active contexts, you will see
   the option to configure layout in the contextual links on all blocks
   on the page. This will allow you to manage the blocks placed by the
   block reaction for contexts.
4. You can use the context editor to adjust the conditions under which each
   context is active and alter its reactions.
