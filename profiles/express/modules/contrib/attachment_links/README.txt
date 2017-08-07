Installation & Setup
=======================

1. Create a content type that will contain the files, for e.g., call it 'File'.
2. Add a file field to the content type. You'll probably want to set it to
   contain multiple values. Remember what you called it.
3. On the content type edit page at /admin/structure/types/manage/file (if
   you've called your type 'file'), click the 'Attachment links' section at the
   bottom. Select the file field that will be used by Attachment links when
   delivering the canonical file. This option exists as you may have multiple
   file fields on a single content type.
4. You can then create a file node and attach some files to it. When you view
   the node, you'll see the links to the two versions of the file provided in
   the content area. You can change this by heading to 'Display fields' section
   for the content type, and dragging to change it's position or hide it for
   the teaser or all modes like 'full', 'rss' and 'teaser'.
5. There is also an 'Attachment links' section on the node edit form that shows
   information, such as the currently used field and the links (in case these
   are not prominently displayed on the node view page.

Convenience aliases
========================

There is an option in the content type settings to switch on this alias
generation. It requires 'Private' file downloads to work - this is because
aliases can't be generated for public files, as the web server handles these
directly and Drupal doesn't have a chance to intercept the request.

Instead of having /node/xxx/attachment, we create an alias for this and use the
node's alias + the file extension to make a convenient alias for the file.

For example, you create a 'File' node, and alias it 'documents/meeting'. You
attach a file called 'meeting-2001-03-03.pdf' to the node. An alias will be
created: 'documents/meeting.pdf'. Going to this alias downloads the file.

It's a convenient way of handling attachments - you can view any node with
attachment links enabled and simply add the extension of the file to the url
to have it download.


Upgrading from Drupal 6
========================

The system updates will move your files from the Upload module's 'attached' list
to a file field named 'upload' automatically. Simply select this field on the
content type edit form under 'Attachment links' to re-enable this functionality.


Written and maintained by:
Four Kitchens
http://fourkitchens.com
