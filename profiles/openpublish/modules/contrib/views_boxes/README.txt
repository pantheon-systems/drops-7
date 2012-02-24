Using Views Boxes 
--------------------------------------

I. Basic use:
--------------------------------------
* Enter a couple of article nodes (just set the titles for now).

* Make sure you have Views UI enabled and then create a new view
  (called list_articles) with a block display that returns the titles
  of your published articles.

* Go back to your test page and reclick 'Configure Layout'.

* Select Boxes in the editor's dropdown, and then drag and drop
  'Add custom view box' into a region.

* You'll be prompted to set the box's description (which is used
  for admin UI purposes) and its title (which is actually rendered
  and shown to end users). Set the title to <none> and the description
  to 'Views Box - Articles' and then click Continue.

* The 'View' dropdown shown then allows you to pick the block display
  you want this box instance to use. For now, we just have list_articles
  which doesn't have any exposed or contextual filters, so we can go
  ahead and save our settings.

* You'll now see your list of articles.

II. Exposed and contextual filters:
--------------------------------------
Views Boxes ships with some filters that are particularly useful for 
allowing extra levels of control over your view while still storing
the configuration data in the box itself. We're going to cover the ability
to curate this list_articles so that we can manually sort the article nodes
through the box. For this, we're going to install the Views Arguments Extras
module as it provides some filters that we'll need.

* Go edit the list_articles view.

* Add a contextual filter and select 'Content: Nid'. Before you save it,
  scroll down and open up the 'More' options.

* The administrative title can be used for two purposes: displaying what it
  is that we'll be selecting (Article nodes) to the admin when using the box,
  and to limit the autocomplete fields we'll have to specific entities and
  bundles. By default the autocomplete field will display all nodes, but if
  we want to restrict it to articles, we'd enter: 
  Article nodes [node] {bundles:article} here

  [ENTITY_TYPE] will tell the autocomplete which entity type to look up
  {bundles:COMMASEPERATEDLISTOFBUNDLES} will limit the auto complete by bundle

* Check the 'Allow multiple values' option and save your changes.

* Add a sort criteria and select 'Arguments: Multi-item Argument Order'.
  This insures that we're actually sorting using the nodes we'll be selecting
  through the contextual filter in the box. Make sure that this sort criteria
  comes before the existing 'Content: Post date' and it's set to
  'sort descending'. Save your changes to the view and go back to your page
  node with the list_articles view block.

* Edit the box by clicking the gear icon that appears when you hover over it
  and then selecting 'Edit Box'. Notice that you don't necessarily need to be
  in Context Editor mode to edit existing boxes.

* You'll notice that you now get 'Article Nodes' under settings for the box,
  which allows you to pick article nodes using the autocomplete fields and to
  order them as you'd like. Once you're done with this, save the changes and
  you'll get your manually reordered list.

* You can reuse the same box instance or another page if you want to keep the
  same ordered list, but you can also create a new custom view box that reuses
  the same view display but with a different selection of nodes. This allows
  for a pretty powerful way to make views configurable through the box interface
  while still keeping the benefits of reusing the same displays (templates
  for examples). Other filters can be exposed too and used through the box
  interface, and having a view box filter on a specific taxonomy term can be
  achieved in a similar manner.

Documentation written by tirdadc - http://drupal.org/user/383630
