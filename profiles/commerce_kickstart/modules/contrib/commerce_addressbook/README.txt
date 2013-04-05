Commerce Addressbook is a module that allows authenticated customers to reuse
previously entered addresses during checkout.
They can manage all entered addresses in their user panel (user/%user/addressbook).

Note that for data consistency reasons editing a previously entered address
won't change it on previously made orders.

Installation
============
* Enable the Commerce Addresbook module
* Visit the admin/commerce/config/checkout page and configure any customer
  profile checkout panes (such as "Billing information") to use the addressbook.
* The "Addresses on File" select list should now automatically be attached to the checkout form.

Updating from Addressbook 1.x
=============================
Don't try to disable the module. Keep it enabled and follow the next steps:
1. Replace the 1.x files with the 2.x files.
2. Run update.php to install the "7200" update.
3. Go to admin/commerce/config/checkout and enable the addressbook for the
desired checkout panes.

Note that the information on which user profile is the default will be lost.
The user will need to go to his addressbook (user/%user/addressbook) and set
the default again.
