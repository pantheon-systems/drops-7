Installing MailUp:

There are 2 modules in this project. "mailup" and "mailup_subscribe".
Enable both for full functionality.

If you are a developer looking for API functionality only, enable "mailup".


Configuring:

Account settings
---------------------

Visit
Administration >> Configuration >> Web services >> MailUp Settings

If you're new to MailUp, you can easily create a new demo account, or otherwise add an existing account here.

Choose tracked lists
---------------------

Click the "Lists" tab.

"Refresh List Options" to see the remote lists available.
"Import" the lists you want to track (Tracking maintains subscription status for users, and allows recipient data transfer to happen).

Setup Recipient Fields
----------------------

Click the "Recipient Fields" tab.

When you are tracking a list, Drupal pushes field data to MailUp for any recipients who have accounts on your website. This happens automatically on Cron run.

To access all your remote fields, click "Import Recipient Field List from MailUp". Fields are configurable on Mailup, and you will need to click this to work with any newly added fields.

Data is set by using Tokens. These tokens should come from the "Users" fieldset, or global tokens. (tokens beginning with [user: ... ])


Allowing users to subscribe/unsubscribe
---------------------------------------

To allow users to update subscription preferences, you need to create "Subscription Targets" for those tracked lists.

A subscription target is a combination of List/Groups, that will be made available to users. Without a subscription target, recipient data will be pushed to mailup, but the user will not be able to alter their subscription.

This module adds a single field to the User Account entity, which displays subscription options based on this configuration. (See: Administration >> Configuration >> People >> Account settings, "manage fields" tab)

Because it works as a standard drupal field, you are free to expose this on the User Registration form, or any other place in which a User entity form is displayed.

For example, use Commerce User Profile Pane (https://www.drupal.org/project/commerce_user_profile_pane), to add the subscription preferences field to the Drupal Commerce checkout process.


Keeping subscription statuses up to date
----------------------------------------

Configure "Webhooks" following the instructions on the "Webhooks" tab, to make sure that your website keeps in sync with any changes to subscription statuses.


Advanced Theming and other advanced configuration
-------------------------------------------------

Both the display of subscription statuses, and the form itself are passed through the theme layer and have templates available for customisation.

These templates are "mailup-subscription-form.tpl.php" and "mailup-subscription.tpl.php"

Copy these to your active theme to override the default display.


Subscription Targets are Entities - This allows you to add any field in the "Manage Fields" sub-tab of the "Lists" page.
The rendered entity will be displayed alongside the form element in the "Subscription" field widget. This allows you to add further info relating to a list, for example, a link to example email content.
