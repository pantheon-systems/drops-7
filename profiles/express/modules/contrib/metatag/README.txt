Metatag
-------
This module allows you to automatically provide structured metadata, aka "meta
tags", about your website and web pages.

In the context of search engine optimization, providing an extensive set of
meta tags may help improve your site's & pages' ranking, thus may aid with
achieving a more prominent display of your content within search engine
results. Additionally, using meta tags can help control the summary content
that is used within social networks when visitors link to your site,
particularly the Open Graph submodule for use with Facebook, Pinterest,
LinkedIn, etc (see below).

This version of the module only works with Drupal 7.40 and newer.

For additional information, see the online documentation:
  https://www.drupal.org/docs/7/modules/metatag


Features
--------------------------------------------------------------------------------
The primary features include:

* The current supported basic meta tags are ABSTRACT, DESCRIPTION, CANONICAL,
  GENERATOR, GEO.PLACENAME, GEO.POSITION, GEO.REGION, ICBM IMAGE_SRC, KEYWORDS,
  PUBLISHER, REFRESH, REVISIT-AFTER, RIGHTS, ROBOTS, SHORTLINK, and the page's
  TITLE tag.

* Multi-lingual support using the Entity Translation module.

* Translation support using the Internationalization (i18n) module of the global
  configurations, the values for all three submodules (Metatag:Context,
  Metatag:Panels, Metatag:Views), and the final meta tags being output.

* Full support for entity revisions and workflows based upon revision editing,
  including compatibility with the Revisioning and Workbench Moderation modules.

* Automatically extracts URLs from image fields, no need for extra modules.

* String-based meta tags may be automatically trimmed to a certain length, and
  the lengths may be easily customized to accommodate changes in search engine
  algorithms.

* A custom pager string may be added to meta tags by inserting the token
  [current-page:pager] into e.g. page titles, description tags, etc. The
  replacement string may be customized from the settings page.

* Per-path control over meta tags using the "Metatag: Context" submodule
  (requires the Context module).

* Integration with the Views module allowing meta tags to be controlled for
  individual Views pages, with each display in the view able to have different
  meta tags, by using the "Metatag: Views" submodule.

* Integration with the Panels module allowing meta tags to be controlled for
  individual Panels pages, by using the "Metatag: Panels" submodule.

* The fifteen Dublin Core Basic Element Set 1.1 meta tags may be added by
  enabling the "Metatag: Dublin Core" submodule.

* Forty additional Dublin Core meta tags may be added by enabling the "Metatag:
  Dublin Core Advanced" submodule.

* The Open Graph Protocol meta tags, as used by Facebook, Pinterest, LinkedIn
  and other sites, may be added by enabling the "Metatag: Open Graph" submodule.

* Twenty six additional Open Graph Protocol meta tags are provided for
  describing products in the "Metatag: Open Graph Products" submodule.

* The Twitter Cards meta tags may be added by enabling the "Metatag: Twitter
  Cards" submodule.

* Certain meta tags used by Google+ may be added by enabling the "Metatag:
  Google+" submodule.

* Facebook's fb:app_id, fb:admins and fb:pages meta tags may be added by
  enabling the "Metatag: Facebook" submodule. These are useful for sites which
  are using Facebook widgets or are building custom integration with Facebook's
  APIs, but they are not needed by most sites and have no bearing on the
  Open Graph meta tags.

* The App Links meta tags may be added by enabling the Metatag: App Links
  submodule.

* Site verification meta tags can be added, e.g. as used by the Google search
  engine to confirm ownership of the site; see the "Metatag: Verification"
  submodule.

* The Metatag: Mobile & UI Adjustments submodule adds the MobileOptimized,
  HandheldFriendly, viewport, cleartype, theme-color, format-detection,
  apple-mobile-web-app-capable, apple-mobile-web-app-status-bar-style, the
  android-app and ios-app alternative link meta tags, and the Android manifest
  tag.

* The hreflang meta tags are available via the Metatag:hreflang submodule.

* Support for meta tags specific to Google Custom Search Appliance are available
  in the "Metatag: Google Custom Search Engine (CSE)" submodule.

* A variety of favicon sizes and styles can be added to the global configuration
  using the Metatag: Favicons submodule.

* An API allowing for additional meta tags to be added, beyond what is provided
  by this module - see metatag.api.php for full details.

* Support for the Migrate module for migrating data from another system - see
  metatag.migrate.inc for full details.

* Support for the Feeds module for importing data from external data sources or
  file uploads.

* Support for the Search API module for indexing of keywords.

* Integrates with Devel_Generate, part of the Devel module, to automatically
  generate meta tags for generated nodes, via the Metatag:Devel submodule.

* Integrates with Workbench Moderation (v1) allowing meta tags on nodes to be
  managed through the workflow process; this custom support is not needed in
  Workbench Moderation v3 so the extra logic is automatically ignored.

* The Transliteration module (see below) is highly recommended when using image
  meta tags, e.g. og:image, to ensure that filenames are HTML-safe.

* Adds an extra item to the "Flush all caches" menu for the Admin Menu module,
  allowing for a quick way to clear the Metatag module's custom caches.

* A custom pane, called "Node form meta tags", is available for adding the meta
  tags fieldset when the node_edit page is customized using Panels; the
  Metatag: Panels submodule does not need to be enabled in order for this to
  work.

* Several advanced options may be controlled via the Settings page.

* An import script is provided in the Metatag:Importer submodule for sites that
  need to import data from Metatags Quick, Nodewords (Drupal 6 only), or Page
  Title.

* If the Media module (v2) is installed, the Media WYSIWYG submodule will be
  used to automatically filter out Media's embed codes.


Configuration
--------------------------------------------------------------------------------
 1. On the People Permissions administration page ("Administer >> People
    >> Permissions") you need to assign:

    - The "Administer meta tags" permission to the roles that are allowed to
      access the meta tags admin pages to control the site defaults.

    - The "Edit meta tags" permission to the roles that are allowed to change
      meta tags on each individual page (node, term, etc).

 2. The main administrative page controls the site-wide defaults, both global
    settings and defaults per entity (node, term, etc), in addition to those
    assigned specifically for the front page:
      admin/config/search/metatags

 3. The list of supported entity types (nodes, taxonomy terms, etc) and bundles
    (content types, vocabularies, etc) may be controlled from the Settings page:
      admin/config/search/metatags/settings

 4. In order to provide a specific configuration per entity bundle (content
    type, vocabulary, etc), click "Add default meta tags".

 5. Each supported entity object (nodes, terms, users) will have a set of meta
    tag fields available for customization on their respective edit page, these
    will inherit their values from the defaults assigned in #2 above. Any
    values that are not overridden per object will automatically update should
    the defaults be updated.

 6. As the meta tags are output using Tokens, it may be necessary to customize
    the token display for the site's entities (content types, vocabularies,
    etc). To do this go to e.g., admin/structure/types/manage/article/display,
    in the "Custom Display Settings" section ensure that "Tokens" is checked
    (save the form if necessary), then to customize the tokens go to:
    admin/structure/types/manage/article/display/token


Internationalization with the Translation (core) and Entity Translation modules
--------------------------------------------------------------------------------
The module works with the core Translation module, allowing the meta tags for a
specific entity (node, term, etc) to be tied to a specific language. It also
supports the Entity Translation module, which may work better thank the basic
Translation module depending upon the site's desired functionality. This
integration means that content creators can customize an entity's meta tags for
each language supported on the site, and that the correct meta tags should
always be displayed for each locale.


Internationalization with the i18n modules
--------------------------------------------------------------------------------
Using the String Translation (i18n_string) submodule of the Internationalization
(i18n) module package it is possible to translate meta tags:

* All default configurations (admin/config/search/metatag) are translatable.
  When a configuration is created or updated it will pass the values to the
  i18n_string system. Additionally it is possible to bulk update them via the
  string translation page (admin/config/regional/translate/i18n_string).

* Meta tags for all submodules (Metatag:Context, Metatag:Panels, Metatag:Views)
  are translatable. Similar to the default configurations, these meta tags are
  made available when they are created and/or update, and may also be bulk
  updated.

* Meta tags from entities (nodes, terms, etc) are not directly translatable.

* The final output meta tags are passed through the translation system when the
  page is being loaded. It is not possible to use the strings bulk updater to
  spool all pages on the site, to do so it would be necessary to spool the page
  using a separate script or tool.

Additionally, certain variables are available for translation using the Variable
Translation submodule of the i18n package:

* metatag_pager_string - The custom pager string.


Internationalization with the Smartling module
--------------------------------------------------------------------------------
The Smartling translation service may be used with the Metatag module provide an
improved UX around the meta tag translation process. In order to do this, the
Smartling Interface Translation (smartling_interface_translation) module must
be enabled.

For further details see the module's project page:
  https://www.drupal.org/project/smartling


Search API integration
--------------------------------------------------------------------------------
Entity meta tag values can be made searchable using the Search API module
(https://www.drupal.org/project/search_api).

 1. Select "Meta tags" under "Data alterations" in the filters for the
    index:
      admin/config/search/search_api/index/INDEX NAME/workflow
 2. Meta tag fields will now appear under "Fields" and can be enabled there:
      admin/config/search/search_api/index/INDEX NAME/fields


Fine tuning & suggestions
--------------------------------------------------------------------------------
* There are many options available on the settings page to control how Metatag
  works:
    admin/config/search/metatags/settings

* It is possible to "disable" the meta tags provided by Drupal core, i.e.
  "generator", "canonical URL" and "shortlink", though it may not be completely
  obvious. Metatag takes over the display of these tags, thus any changes made
  to them in Metatag will supercede Drupal's normal output. To hide a tag, all
  that is necessary is to clear the default value for that tag, e.g. on the
  global settings for nodes, which will result in the tag not being output for
  those pages.

* When using Features to export Metatag configurations, it is suggested to
  override all of the default configurations and then disable the default
  configurations via the advanced settings page; doing so will avoid potential
  conflicts of the same configurations being loaded by both the Metatag module
  and the new Features-based modules.

* Using fields to automatically fill in values for image meta tags is the
  recommended way of inserting images - the module will automatically extract
  the URL from the value. However, by default this forces social networks,
  search engines and certain browsers to download the original version of the
  image, which could be multiple megabytes. The alternative is to use the
  Imagecache_Token module to instead load meta tags via a specific image style.
  As an example, in order to load an image from a node field named
  "field_meta_tag_image" using the "seo_thumbnail" style, the following token
  would be used:
    [node:field_meta_tag_image:seo_thumbnail:uri]
  or
    [node:field_meta_tag_image:seo_thumbnail]
  (They give the same results)
  Additionally, dimensions of the image may be obtained from the following:
    [node:field_meta_tag_image:seo_thumbnail:width]
    [node:field_meta_tag_image:seo_thumbnail:height]


Developers
--------------------------------------------------------------------------------
Full API documentation is available in metatag.api.php.

It is not necessary to control Metatag via the entity API, any entity that has
view modes defined and is not a configuration entity is automatically suitable
for use.

The meta tags for a given entity object (node, etc) can be obtained as follows:
  $metatags = metatags_get_entity_metatags($entity_id, $entity_type, $langcode);
The result will be a nested array of meta tag structures ready for either output
via drupal_render(), or examining to identify the actual text values.


Troubleshooting / known issues
--------------------------------------------------------------------------------
* When using custom page template files, e.g., page--front.tpl.php, it is
  important to ensure that the following code is present in the template file:
    <?php render($page['content']); ?>
  or
    <?php render($page['content']['metatags']); ?>
  Without one of these being present the meta tags will not be displayed.
* An alternative method to fixing the missing-tags problem is to change the page
  region used to output the meta tags. The region used may be controlled from
  the settings page, it is recommended to test different options to identify the
  one that works best for a specific site.
* Versions of Drupal older than v7.17 were missing necessary functionality for
  taxonomy term pages to work correctly.
* Using Metatag with values assigned for the page title and the Page Title
  module simultaneously can cause conflicts and unexpected results. It is
  strongly recommended to convert the Page Title settings to Metatag and just
  uninstall Page Title entirely. See https://www.drupal.org/node/2774833 for
  further details.
* When customizing the meta tags for user pages, it is strongly recommended to
  not use the [current-user] tokens, these pertain to the person *viewing* the
  page and not e.g., the person who authored a page.
* Certain browser plugins, e.g., on Chrome, can cause the page title to be
  displayed with additional double quotes, e.g., instead of:
    <title>The page title | My cool site</title>
  it will show:
    <title>"The page title | My cool site"</title>
  The solution is to remove the browser plugin - the page's actual output is not
  affected, it is just a problem in the browser.
* Drupal core versions before v7.33 had a bug which caused validation problems
  in the Open Graph output if the RDF module was also enabled. The solution is
  to update to core v7.33 or newer.
* If the Administration Language (admin_language) module is installed, it is
  recommended to disable the "Force language neutral aliases" setting on the
  Admin Language settings page, i.e. set the "admin_language_force_neutral"
  variable to FALSE. Failing to do so can lead to data loss in Metatag.
* If Entity Token is installed (a dependency for Rules, Commerce and others) it
  is possible that the token browser may not work correctly and may either
  timeout or give an error instead of a browsable list of tokens. This is a
  limitation of the token browser.


Related modules
--------------------------------------------------------------------------------
Some modules are available that extend Metatag with additional or complimentary
functionality:

* Schema.org Metatag
  https://www.drupal.org/project/schema_metatag
  Extensive solution for adding schema.org / JSON-LD support to Metatag.

* Transliteration
  https://drupal.org/project/transliteration
  Tidies up filenames for uploaded files, e.g. it can remove commas from
  filenames that could otherwise break certain meta tags.

* Imagecache Token
  https://www.drupal.org/project/imagecache_token
  Use tokens to load images via image styles, rather than forcing meta tags to
  use the original image.

* Alternative hreflang
  https://www.drupal.org/project/hreflang
  An alternative to the Metatag:hreflang module. Automatically outputs
  <link rel="alternate" hreflang="x" href="http://" /> meta tags on every page
  for each language/locale available on the site. Also does not provide any way
  of overriding the values or setting the x-default value.

* Domain Meta Tags
  https://drupal.org/project/domain_meta
  Integrates with the Domain Access module, so each site of a multi-domain
  install can separately control their meta tags.

* Select or Other
  https://drupal.org/project/select_or_other
  Enhances the user experience of the metatag_google_plus and metatag_opengraph
  submodules by allowing the creation of custom itemtype and og:types values.

* Node Form Panes
  https://drupal.org/project/node_form_panes
  Create custom node-edit forms and control the location of the Metatag fields.

* Textimage
  https://drupal.org/project/textimage
  Supports using Textimage's custom tokens in meta tag fields.

* Field Multiple Limit
  https://drupal.org/project/field_multiple_limit
  Allows control over how many items are output in a multi-item field, useful
  with meta tags that only allow for one item but which are assigned from fields
  which accept multiple items, e.g. og:audio and og:video.

* Real-time SEO for Drupal
  https://www.drupal.org/project/yoast_seo
  Uses the YoastSEO.js library and service (https://yoast.com/) to provide
  realtime feedback on the meta tags.

* Parse.ly Publishing Analytics
  https://www.drupal.org/project/parsely
  Automatically generates meta tags for the Parse.ly service.

* Metatag Cxense
  https://www.drupal.org/project/metatag_cxense
  Adds support for the Cxense meta tags used by their DMP and Insight services.


Credits / contact
--------------------------------------------------------------------------------
Currently maintained by Damien McKenna [1] and Dave Reid [2]; all initial
development was by Dave Reid.

Ongoing development is sponsored by Mediacurrent [3] and Lullabot [4]. All
initial development was sponsored by Acquia [5] and Palantir.net [6].

The best way to contact the authors is to submit an issue, be it a support
request, a feature request or a bug report, in the project issue queue:
  https://www.drupal.org/project/issues/metatag


References
--------------------------------------------------------------------------------
1: https://www.drupal.org/u/damienmckenna
2: https://www.drupal.org/u/dave-reid
3: https://www.mediacurrent.com/
4: https://www.lullabot.com/
5: https://www.acquia.com/
6: https://www.palantir.net/
