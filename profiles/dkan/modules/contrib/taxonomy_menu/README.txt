TAXONOMY MENU
=============
(README.txt: 13th of April 2009, Version 6.x-2.3, indytechcook + ksc)

-------------- Content ------------------
INTRO
INSTALLATION
- New
- Upgrade
CONFIGURATION
- Where to find the configuration screen?
- Adjustments and options
PATH TYPES and INTEGRATION WITH VIEWS MODULE
- Menu Path Type: Default
- Menu Path Type: Hierarchy
- Menu Path Type: Custom
INTEGRATION WITH OTHER MODULES
- TAXONOMY MANGAGER
- TAXONOMY REDIRECT
- CONTENT TAXONOMY
- TAXONOMY BREADCRUMBS
- HIERARCHICAL SELECT
- i18n
- DOMAIN ACCESS
- PATHAUTO
ADDITIONAL NOTES
PROSPECT TO PLANNED FUNCTIONS
------------- End Content -----------------


INTRO
=====
This module adds links (menu entries) to taxonomy terms to the global navigation menu.
With the current version users can create one group of menu entries and add specific options 
for each vocabulary. More functionality is beeing planned with further versions. 

INSTALLATION
============

NEW 

1) Place this module directory in your "modules" folder (this will usually be
   "sites/all/modules/"). Don't install your module in Drupal core's "modules"
   folder, since that will cause problems and is bad practice in general. If
   "sites/all/modules" doesn't exist yet, just create it.

2) Enable the Taxonomy Menu module in Drupal at:
   administration -> site configuration -> modules (admin/build/modules)
   The Drupal core taxonomy module is required.
   The modules Taxonomy Menu Custom Path and Taxonomy Menu Hierarchy provide
   additional path configuration types (see the "INTEGRATION WITH VIEWS MODULE" section below). 

3) Create a new vocabulary or edit an excisting one.
   
4) Choose which vocabularies to appear in the menu at:
   administration -> content management -> taxonomy
   (admin/content/taxonomy)

UPGRADE
Please read UPGRADE.txt


CONFIGURATION
=============

LOCATION OF CONFIGURATION SCREEN
 All configuration options are on the vocabulary's edit screen: 
  admin/content/taxonomy   (or)
  admin/content/taxonomy/edit/vocabulary/$vid

ADJUSTMENTS AND OPTIONS
 Menu: Select under which menu the vocabulary's terms should appear. 
  With the current version users can create one group of menu entries for each vocabulary.
 
 Menu Path Type: Select how the url for the term path should be created. 
  Included are Default, Hierarchy and Custom Path.
  To use Hierarchy and/or Custom Path you need to enable the related modules first.  
  Menu Path Type = Default: The path will be taxonomy/term/% while Term ID will be passed as argument.
  (multiple arguments possible). This path type uses standard taxonomy display - views is not needed.
  
  For other path types and their options see: INTEGRATION WITH VIEWS MODULE
  For developers: This is extendable using hook_taxonomy_menu_path().
  See developers documentation for more information. (http://drupal.org/node/380652)
  
 Syncronise changes to this vocabulary: If selected, the menu will auto update when you
  change a node or term. Recommened to always have this selected.
  When you change the generated menu with the core menu function, i.e. move it or change the structure, 
  these changes most probably get lost when adding a new taxonomy term because Taxonomy Menu rebuilds
  the menu without knowing about the changes made elsewhere.
 
 Display Number of Nodes: Displays the number of nodes next to the term in the menu.
  If option "Display Descendants" is enabled also descendents will be counted.
 
 Hide Empty Terms: Does not create menu links for terms with no nodes attached to them.
 
 Item for Vocabulary: Create a menu link for the vocabulary.  
  This will be the parent menu item.
 
 Auto Expand Menu Item: Enables the 'Expand' option when creating the menu links.  
  This is useful if using 'suckerfish' menus (pull down menus) in the primary links.
 
 Display Descendants:  Alters the URL to display all of child terms. 
 <base path>/$tid $tid $tid $tid
  When this is set, the Path Alias (module PATHAUTO) is not applied.
 
 Select to rebuild the menu on submit: Deletes all of menu items and relationships between 
  the menu and terms and recreates them from the vocabulary's terms.  
  This will create new mlid's for each item, so be careful if using other modules to extend 
  the menu functionality.

     
PATH TYPES and INTEGRATION WITH VIEWS MODULE
============================================

MENU PATH TYPE: DEFAULT
 The path will be taxonomy/term/% while Term ID will be passed as argument (multiple arguments possible).
 This path type can be used without having the VIEWS module installed.
 VIEWS provides a view named taxonomy_term (A view to emulate Drupal core's handling of taxonomy/term)
 The path of the view is 'taxonomy/term/%', the argument is 'Term ID (with depth)' and 'Depth Modifier' - 
 but only TERM ID will be passed as an argument.
 One can adjust this view to ones needs - but it might be a problem, to have only one view for all all 
 taxonomy menu links. So it is recommended to use the option MENU PATH TYPE: CUSTOM for individual 
 views per vocabulary.    

MENU PATH TYPE: CUSTOM
 With this path type, one can create individual views for each vocabulary.
 You need to have a view (page) with path 'custom path/%' and an argument 'Term ID' BEFORE you create 
 the taxonomy menu. Enable the option "Allow multiple terms per argument" while adding the argument 
 'Term ID' and choose a title like "Terms". Other options should be left by default unless really 
 needs to change sth..
 Fields and filters can be added and options can be set according to ones needs.
 Back to Taxonomy Menu:
 Enter your 'custom path' in the field "Base Path for Custom Path:" - leave out '/%'. 
 For example when your view path is 'interests/%' you enter only 'interests' here.   
 To use the 'Display Depth in Custom Path:' option, you need to have 'Taxonomy: Term ID depth modifier' 
 as second argument within your view.  

MENU PATH TYPE: HIERARCHY
 This path type is mainly beeing created for developers use.
 This should only be applied if you have custom code or a block that relies on the category/vid/tid/tid/tid.
 If you would like the url to be this path, the recomendation is to use PathAuto with 
 'category/[vocab-raw]/[copath-raw]'. Use the field "Base Path for Hierarchy Path" to see the base URL 
 that will match the veiw or page callback. The view or pagecall back MUST be created before the taxonomy menu.
 
 --- How to set up a view for MENU PATH TYPE: HIERARCHY (and only for this!)-----
 
 The vocabulary might be like:
 Vocabulary
 Term-1
 -- Term-1.1
 -- --Term-1.1.1
 -- --Term-1.1.2
 -- Term-1.2
 Term-2
 -- Term-2.1

 What is needed:
 Modules: TAXONOMY MENU with TAXONOMY MENU HIERARCHY and VIEWS

 Steps:
 * Create a view with a path: category/% (where the term "category" can be chosen)
 * Add fields and filters according your needs
 * Add the following arguments:
   - Vocabulary ID (Title: %1)
   - Term ID (Title: %2)
   - Term ID (Title: %3)
   - Term ID (Title: %4)
   - More arguments of this types might be needed, if the vocabulary has a greater depth than 3. 

 * Go to admin/content/taxonomy
   - Select the vocabulary you want to have a menu for.
   - Select "Menu:" (where the menu should show up)
   - Select "Menu Path Type: Hierarchy"
   - Enter "Base Path for Hierarchy Path: category" (or what you have chosen as path for view)
   - Optional: Display Number of Nodes / Auto Expand Menu Item
   - Check "Item for vocabulary"
   - Do NOT check "display descendants"  

 After saving the menu should appear.
 Now comes the BUT: most probably you don´t see any nodes when klicking the menu items.
 For Term-1 the path is: ..category/vid/tid, for Term-1.1.1 it is category/vid/tid/tid/tid
 Everything behind "category" will be taken as arguments in views.
 So only those nodes will be shown that are linked to the taxonomy terms Term-1 AND Term-1.1 
 AND Term1.1.1. within the vocabulary (it is a logical AND function, whereas multiple arguments 
 TermID TermID TermID is a logical OR function).
 Once you have linked your nodes to the taxonomy terms in the described way, they will be shown 
 when clicking on the menu items. It produces nice breadcrumbs and page titles (remember to set 
 the titles for the arguments in views as described) - and it always displays descendants.
 The only module that supports the saving of a whole term lineage when selecting a deep level 
 item seems to be HIERACHICAL SELECT. See chapter INTEGRATION WITH OTHER MODULES
 
INTEGRATION WITH OTHER MODULES
==============================

TAXONOMY MANGAGER
Helpful to organize taxonomy terms - Taxonomy Menu module does not interfere with it functions.
(http://drupal.org/project/taxonomy_manager)

TAXONOMY REDIRECT
Changes the taxonomy default URL to match the custom Taxonomy Menu Path can be controlled 
by Taxonomy Redirect. 
(http://drupal.org/project/taxonomy_redirect)
 
CONTENT TAXONOMY
It is a nice and very helpful module to link taxonomy terms to nodes. 
Taxonomy Menu does not interface with the content taxonomy tables, so be sure to enable the option 
"Save values additionally to the core taxonomy system (into the 'term_node' table)" 
otherwise the related taxonomy terms will not be accessable for Taxonomy Menu.
(http://drupal.org/project/content_taxonomy)
 
HIERARCHICAL SELECT with submodule HS_TAXONOMY
Supports the selection of terms in an hierarchical structured vocabulary. 
For using "MENU PATH TYPE: HIERARCHY" within Taxonomy Menu the HS options "Save term lineage" 
and "Force the user to choose a term from a deepest level" should be enabled.
(http://drupal.org/project/hierarchical_select) 
 
TAXONOMY BREADCRUMBS
Advanced breadcrumbs can be controlled by this module.
(http://drupal.org/project/taxonomy_breadcrumbs)

MENU BREADCRUMBS
Helpful to create menu breadcrumps outside the main navigation menu especially 
when using Custom or Hierarchical path. 
(http://drupal.org/project/menu_breadcrumbs)

i18n
At the momement the multiple language support seems to work only when the Taxonomy Menu option 
"item for vocabulary" is disabled. 

DOMAIN ACCESS
....

PATHAUTO
 Menu Items are Path Alias aware and compatible with PATHAUTO.
 Have a look at the various path types, which URL is passed to the code.
 Delault is taxonomy/term/$tid.
 (http://drupal.org/project/pathauto)
 
 
ADDITIONAL NOTES
================
 * Taxonomy Menu does not handle the menu call backs. It only creates the links to the menus.
   This means that everythign that is displayed on the page (including title, content, breadcrumbs, etc)
   are not controled by Taxonony Menu.
 * The router item must be created before Taxonomy Menu creates the links.  Failure to so so 
   will cause the menu items to not be created.
 * Router items can be created by either a view or another modules hook_menu.

 
PROSPECT TO PLANNED FUNCTIONS (6.x-3.0 version)
===============================================
- concept of "Menu Groups"
- any number of Menu Groups per vocabulary
- more than one vocabulary within one Menu Group
- more options to define the url path
- other options
