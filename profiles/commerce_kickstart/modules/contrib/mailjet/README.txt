---
This module for Drupal 7.x. provides complete control of Email settings with
Drupal and Mailjet.
It simply replaces your default SMTP settings in order to make all your emails
go through Mailjet: this will improve your deliverability and allow you
to optimize your campaigns.

Mailjet helps to send and track emails in real time,
while ensuring their deliverability.
You'll take advantage of our reporting tools and get advanced statistics
to monitor and optimize your emails.

Prerequisites
-------------

The Mailjet plugin relies on the PHPMailer v5.2.21 for sending emails.
This script must be uplodaded manually by the user in the Drupal instalation.
1) Get the PHPMailer v5.2.21 from GitHub here:
http://github.com/PHPMailer/PHPMailer/archive/v5.2.21.zip
2) Extract the archive and rename the folder "PHPMailer-5.2.21" to "phpmailer".
3) Upload the "phpmailer" folder to your server inside
DRUPAL_ROOT/sites/all/libraries/.
4) Verify that the file class.phpmailer.php is correctly located at this
path: DRUPAL_ROOT/sites/all/libraries/phpmailer/class.phpmailer.php
* Note: Libraries API can be used to move the library to an alternative
location, if needed, e.g. for use in a distribution.

Installation
------------

1) Upload all content in your Drupal sites/all/modules/ directory.
2) Log in as administrator in Drupal.
3) Enable the Mailjet settings module on the Administer > Site building > Modules page.
4) Fill in required settings on the Administer > Site configuration > Mailjet settings page

Author
------
Mailjet SAS
plugins@mailjet.com
