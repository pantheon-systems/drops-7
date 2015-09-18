DraggableViews
==============

This module provides dragging entities and saving their order.

Quick install:
 1) Activate Draggableviews module at admin/modules.
 2) Navigate to view edit-page, click on the first link at the Format section and then choose style "table".
 3) Click Add button at the "Fields" section and choose field "Content:title", add and apply.
 4) Click Add button at the "Fields" section and choose field "Draggableviews: Content", add apply.
 5) Click Add button at the "Sort criteria" section and choose field "Draggableviews: Weight", add and choose sort asc, then apply.
 6) Save the view and you're done.

In the case of table standard drupal tabledrag.js JavaScript is used.

We also support jQuery UI Sortable JavaScript. In order to use it please set display style HTML List.
By default HTML list is displayed like grid. If you would like it to be displayed as list override
CSS styles for example in following way:
  .draggableviews-processed li.views-row { float: none; width: 100%; margin-left: 0; }

One view/display to set order another to display
================================================

You can create one view to set the order and another view to display the order. Or even
create one view with two separate displays. In a view that displays the order there
should be no draggableviews field (that makes view sortable), then in the settings of
the "draggableviews weight" sorting criteria there will be selectbox "Display sort as"
where you can choose the source view of your weights. This is applicable when you use
 Native handler.

Step by Step Guide for Creating a New View with 2 Displays:
===========================================================
Requirements: Draggableviews 7.x-2.x, Views 7.x-3.x, Views UI module enabled.

 1) Activate Draggableviews module at admin/modules.
 2) Create a new view
    - Goto '/admin/structure/views/add' on your site.
    - Check off 'Create a page'.
    - Check off 'Create a block'.
    - Set the 'Display format' for the page to what you desire.
    - Set the "'Display format' of" to fields.
    - Set the 'Display format' for the block to table.
    - Fill in the rest of the views information.
    - Click Continue & edit button.
 3) Under the "FIELDS" section, do you see "Content: Title (Title)"?  If you do not:
    - Click 'add' button at the "Fields" section and choose field "Content:title", add and apply.
 4) Click on 'Block' under the 'Display', to change the view display to the block display.
 5) Add the Draggableviews Field:
    - Click Add button at the "FIELDS" section.
    - At the top of the overlay, Change "For: 'All displays'" to 'This block (override)'.
      - If you do not do this then the field will be add to all displays and will prevent your
        page display from using the block display to sort the order.
 5) Click Add button at the "SORT CRITERIA" section choose field "Draggableviews: Weight", add and choose sort asc, then apply.
 6) Under the "SORT CRITERIA" section, do you see "Content: Post date (asc)"?  If you do:
    - Click on it.  At the bottom, click the 'Remove' button.
      - An alternative is to rearrange the "SORT CRITERIA" order, making sure 'Draggableviews: Weight (asc)
        appears first (or on top).
 7) Save the view and you're done.*
*Things to confirm after you saved your new view.
- In the Administrative Views UI, Go back to your View's 'page' display.
  -> Click 'Draggableviews: Weight (asc)' under 'SORT CRITERIA'
  -> You should see:

  Display sort as:
  <title of view> (<display title>)

  This should the view and block display you just create.

  FYI - This is also where you can change it to another view.

Permissions
===========

Add "Access draggable views" permission to users who should be able to reorder views.  If a user does not have this
permission they can still see the view, however they will not be able to reorder it.

If you want only want the order view visible to users with "Access draggable views" then set the Access to
"Permission: Access draggable views".

When users have the "Access draggable views" and "Use contextual links" permission, they will see
a contextual link from the non-reordering view to the ordering view.

Arguments handling
==================

Every time we save the order of a view, current set of arguments are saved with order.
You can see this in draggableviews_structure table "args" column. By default when we display order we use all
currently passed arguments to a view to "match" arguments in "args" column. This means that we can create
a view with contextual filter or exposed filter criteria and save different orders for different sets of arguments.

Using the "Do not use any arguments (use empty arguments)" option will completely ignore passed arguments used
in the Arguments handling of Sort criteria Draggable views weight. Be aware that in this case empty arguments set
will be used. So you can set order for a view when no arguments passed and then whatever arguments passed,
empty set will be used.

Using the "Prepare arguments with PHP code" option will let you alter arguments before they passed to
"matching" with "args" column. For us this means that we can create, for example, several exposed filters,
but pass values of only one of values of exposed filters instead of all of them (like we create two exposed
filters: author and node type, but take into account for ordering only node type).
Please be aware that in PHP code arguments are passed as $arguments variable and you should return an array.
IE return array('status' => 1, 'user' => 2);  or return $arguments; // $arguments is already an array

When using arguments,  make sure your ordering view display has the same arguments as the display you want to show
the end user.  If they do not match, then your ordering will not match.

Using hook_draggableviews_handler_native_arguments_alter(&$arguments, $view, &$form_values) {} You may remove or change
the arguments save to the database, just as the "Prepare arguments with PHP code" option. See draggavleviews.api.php
for more details.

In the $arguments array, Contextual filters are number keyed and exposed filters are name keyed.

Removed Arguments:
- The pager 'item_per_page' exposed filter will never be saved.


Contextual link "Order view"
============================

If there is view with sort order draggableviews weight and the order is set by another view we show "Order view"
contextual link for opening a view that sets the order.


Troubleshooting Drag n' drop Not Showing
========================================
1. Make sure JavaScript is turned on and loading property.  Double check your source code.  For tables (D7) its <root>/misc/tabledrag.js.
2. Make sure you have draggableviews permission for the correct role.
3. Select 'show row weights'.  By default, this is located at the top right of the table. See http://drupal.org/files/draggableviews-1978526-hode-row-weights.png" for a visual image.
4. 'Show row weights' is a global variable/setting.  If you turn it off for 1 table, then all tables, across all pages, across all users, will not see it.  To fix this in the UI, you have to 'hide row weights' on another page/table, such as admin/structure/block (D7) or admin/build/block (D6), or go into the variables table in the database.
