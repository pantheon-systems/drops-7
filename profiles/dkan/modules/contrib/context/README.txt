
Current state of Context for Drupal 7
-------------------------------------
Context for D7 is a straight port of Context 3.x from D6. There are no major
API changes and any exported contexts from D6 should be compatible with the D7
version. You will need the latest CTools (as of Sept. 16 2010) from here:

- http://github.com/sdboyer/ctools

### Working

- all conditions except node taxonomy condition
- all reactions
- context UI
- context layouts
- inline editor (see the context_ui README file for info on enabling)

### Expect API changes

- node taxonomy condition to generic field condition for entities


Context 3.x for Drupal 7.x
--------------------------
Context allows you to manage contextual conditions and reactions for
different portions of your site. You can think of each context as
representing a "section" of your site. For each context, you can choose
the conditions that trigger this context to be active and choose different
aspects of Drupal that should react to this active context.

Think of conditions as a set of rules that are checked during page load
to see what context is active. Any reactions that are associated with
active contexts are then fired.


Installation
------------
Context can be installed like any other Drupal module -- place it in
the modules directory for your site and enable it (and its requirement,
CTools) on the `admin/modules` page.

You will probably also want to install Context UI which provides a way for
you to edit contexts through the Drupal admin interface.


Example
-------
You want to create a "pressroom" section of your site. You have a press
room view that displays press release nodes, but you also want to tie
a book with media resources tightly to this section. You would also
like a contact block you've made to appear whenever a user is in the
pressroom section.

1. Add a new context on admin/structure/context
2. Under "Conditions", associate the pressroom nodetype, the pressroom view,
   and the media kit book with the context.
3. Under "Reactions > Menu", choose the pressroom menu item to be set active.
4. Under "Reactions > Blocks", add the contact block to a region.
5. Save the context.

For a more in-depth overview of the UI components, see the Context UI
`README.txt`.


Hooks
-----
See `context.api.php` for the hooks made available by context and `API.txt` for
usage examples.


Maintainers
-----------

- yhahn (Young Hahn)
- jmiccolis (Jeff Miccolis)
- Steven Jones


Contributors
------------

- alex_b (Alex Barth)
- dmitrig01 (Dmitri Gaskin)
- Pasqualle (Csuthy Bálint)
