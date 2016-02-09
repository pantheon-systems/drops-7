Description
-----------
Gives a site owner options to disable specific messages shown to end users.  
The core drupal message system as offered by drupal_set_message
is an excellent way for modules to send out messages to the end users.
However not all drupal site owners are keen to show all the messages
sent out by drupal core and all modules to their users. This module
gives site administrators a reasonably powerful way to filter out
messages shown to the end users.
 
Features
--------
1. Filter out messages that match a full text string exactly.
2. Filter out messages that match a regular expression.
3. Permissions to specifically hide all messages of a given type from 
    any role.
4. Disable all filtering for specific users.
5. Disable all filtering for specific paths.
6. Apply filtering only for specific paths.
7. Debug system to get messages in the HTML without showing it to the end users.

Installation
------------
1. Extract the tar.gz into your 'modules' or directory.
2. Enable the module at 'administer >> modules'.
   Note: The installation process will grant view permissions
   for all three types of messages (warning, status, and error)
   when it is first installed.  If you wish to hide certain types
   of messages by user role, you can do so by removing these
   permissions.
3. Configure options in Administration >> Configuration >>
   Development >> Disable Messages.
4. Configure permissions in Administration >> People >> Permissions.

Configuration
-------------
1. Visit the configuration page at:
   'Administration >> Configuration >> Development >> Disable Messages'

2. Add the specific messages you wish to filter out to the 'Messages
   to be disabled' text area.  These messages should be in the form
   of Regular Expressions, with one entered per line.  You do not
   have to include the opening and closing forward slashes for each
   regular expression. The system will automatically add /^ and $/
   at the beginning and end of the pattern to ensure that the match is
   always a full match instead of a partial match. This will help
   prevent unexpected filtering of messages. So if you want to filter
   out a specific message ensure that you add the full message including
   any punctuation and additional HTML if any.

   If you are familiar with wildcard searches using *, and not Regular
   Expressions, you can achieve the exact same thing by using .* as your
   wildcard character.  For example, you could wildcard filter out
   any Article creation messages using the following Regular Expression:
     Article .* has been created.

3. Next configure 'Page and User Level Filtering Options'.  By default,
   filtering is enabled for all users on all pages.  Here you can
   specify the pages where filtering should be applied or excluded by
   setting the 'Apply filters by page' radio and textarea and entering
   page paths, one per line.  These standard visibility controls work
   just like the core Block system's.

   You may also turn filtering off for certain Drupal User ID's (uid).
   This can be useful to turn off filtering for the Admin user uid of 1.
   You can also turn off filtering for Anonymous users, whose uid is 0.

4. If you are setting up the module for the first time, you should
   enable one or both of the checkboxes under 'Debug options'.  These
   will output information about which messages are being excluded,
   and why.  If you are on a development site, check both boxes and
   the debugging output will be printed at the bottom of each page.

5. Hit 'Save Configuration' to save the settings.

6. Visit 'Administration >> People >> Permissions' to set permissions.
   When the module is first enabled it will granted permissions
   to view all message types to each site role.
   Assign the 'view <type> message' to roles who should be able to see
   the given <type> of messages. Users who do not have the permissions
   to see a given type of messages will not be able to see any of the
   messages of the given type. Useful to hide warning and error
   messages from end users on a production site.

Uninstallation
--------------
1. Disable the module.
2. Uninstall the module

Credits
-------
Written by Zyxware, http://www.zyxware.com/
