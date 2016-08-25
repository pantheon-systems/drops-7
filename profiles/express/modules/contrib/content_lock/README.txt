
The "Content Lock" module

SUMMARY
=======
The purpose of this module is to avoid the situation where two people
are editing a single node at the same time. On busy sites with dynamic
content, edit collisions are a problem and may frustrate editors with
an error stating that the node was already modified and can't be
updated. This module implements a pessimistic locking strategy, which
means that content will be exclusively locked whenever a user starts
editing it. The lock will be automatically released when the user
submits the form or navigates away from the edit page.

Content locks that have been "forgotten" can be automatically released
after a configurable time span using the bundled content_lock_timeout
sub module.

Installation and configuration
==============================
https://www.drupal.org/project/content_lock

LINKS
=====

For a full description, visit the project page
http://drupal.org/project/content_lock

Bug reports, feature suggestions, and latest developments:
http://drupal.org/project/issues/content_lock
