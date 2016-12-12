Description
===========
Devel Debug Log is a developer module that provides a way for developers to
save and display debug messages on a separate page in the web browser. It
serves as an alternative to using drupal_set_message() or watchdog() for
debugging, and a complementary module to Devel for those who find viewing
messages in the browser easier than looking for them, say, in a file.

The module provides the ddl($message) function, which the developer can use to
save a debug message. If an object or array is supplied as $message, it will be
displayed using the Krumo debugging tool. Messages can be viewed at
Reports > Debug Messages.

Installation
============
Standard module installation procedure. Copy the module to modules directory,
and enable.

Use the ddl($message) function in your code to send a debug message.
