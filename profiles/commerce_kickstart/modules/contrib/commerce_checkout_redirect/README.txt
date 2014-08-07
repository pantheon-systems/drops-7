README file for Commerce Checkout Redirect

CONTENTS OF THIS FILE
---------------------
* Introduction
* Requirements
* Installation
* Configuration
* How it works
* Extra


INTRODUCTION
------------
This module redirects anonymous users to a page where they can login or create
a new account when they try to checkout.


REQUIREMENTS
------------
This module requires the following modules:
* Commerce Cart (Drupal Commerce submodule)
Submodule of Drupal Commerce package (https://drupal.org/project/commerce)


INSTALLATION
------------
* Install as you would normally install a contributed drupal module.
  See: https://drupal.org/documentation/install/modules-themes/modules-7
  for further information.


CONFIGURATION
-------------
* Configure the Checkout by Amazon settings in
  Administration > Store > Configuration > Checkout Redirect
  Settings available:
  - Checkout redirect path: Set the checkout redirect path for an anonymous user.
    Leave blank to use the default 'user/login' page. If you redirect to other page
    then make sure you add user login block on that page;
  - Checkout redirect login message: The message that should be displayed for the
    login page in the checkout process;
  - Use username as order email: This will provide the Anonymous checkout as alternative
    to login in the login form (radio options);
  - Reset password checkout redirect message: The message that should be displayed for the
    reset password page for new account in the checkout process.

HOW IT WORKS
------------
* Checkout redirection
  When an anonymous user tries to access the checkout will be redirect to
  the default login page or custom path if was set in the module configuration,
  If set a message could be displayed to the user.
  There are several posibilities for the user:
  - User login
  - Continue as anonymous
  - Register
  - Request password
* Back to checkout
  Based on the options from above the user will be redirect back
  to the checkout page.
  There is an extra step when the user needs to check the email for
  the reset password link, for Request password and Register with administrator approval
  or e-mail verification.

EXTRA
-----
* "commerce_checkout_redirect" User entity property available.
  This property says if the user is in the checkout redirect process.
  Useful for resolving redirect conflicts, using Rules.
  @see https://www.drupal.org/node/2122783
* Anonymous checkout option could be used togheter with Commerce Checkout Complete Registration
 (https://www.drupal.org/project/commerce_checkout_complete_registration),
 which provides as registration option at the end of the checkout process.
