README file for Commerce Checkout by Amazon

CONTENTS OF THIS FILE
---------------------
* Introduction
* Requirements
* Installation
* Configuration
* How it works
* Troubleshooting
* Maintainers

INTRODUCTION
------------
This project integrates Checkout by Amazon payment into
the Drupal Commerce payment and checkout systems.
https://payments.amazon.co.uk/business/api-integration
* For a full description of the module, visit the project page:
  https://www.drupal.org/project/commerce_cba
* To submit bug reports and feature suggestions, or to track changes:
  https://drupal.org/project/issues/commerce_cba


REQUIREMENTS
------------
This module requires the following modules:
* Commerce Cart (Drupal Commerce submodule)
* Commerce Customer (Drupal Commerce submodule)
* Commerce Payment (Drupal Commerce submodule)
All are submodules of Drupal Commerce package (https://drupal.org/project/commerce)


INSTALLATION
------------
* Install as you would normally install a contributed drupal module.
  See:    https://drupal.org/documentation/install/modules-themes/modules-7
  for further information.


CONFIGURATION
-------------
* Configure user permissions in Administration > People > Permissions:
   - Checkout by Amazon access
     Users in roles with the "Checkout by Amazon access" permission will see
     the Checkout by Amazon button in the cart.
   - Access the Checkout by Amazon debug log
     Users in roles with the "Access the Checkout by Amazon debug log" permission
     will be able the see the debug messages if they are enabled.
* Configure the Checkout by Amazon settings in
  Administration > Store > Configuration > Checkout by Amazon
  Settings available:
  - Merchant ID: Amazon merchant id from Amazon Merchant account;
  - Amazon public key: Amazon merchant public key from Amazon Merchant account;
  - Amazon secret key: Amazon merchant secret key from Amazon Merchant account;
  - Method for the requests: HTTPs requests method to be used, 
    POST or GET;
  - Country: The country  for which it was made the Amazon merchant account,
    related with currency intended to be used in the store.
    Available options:
    United Kingdom (gb) for GBP, Germany (de) for EUR, United States (us) for USD;
  - Mode: either if a testing/development store or a production one.
    Available options: Sandbox (for testing) and Live (for production);
  - Checkout type: Amazon provides 2 checkout methods.
    Available options:
    - Inline: provides an Inline checkout widget for Amazon authentication,
      and distinct widgets address (for delivery address) and 
      wallet (for payment) to be used "inline" in edit mode (for selection)
      in the store checkout process;
      
    - Express: provides an Express checkout widget, all widgets at one time,
      authentication, address (for delivery address) and wallet (for payment) selections,
      The address and wallet widgets are still available in the store checkout
      process but in the "Read" mode (with info).
  - Amazon IOPN Help: just information How to set Amazon Instant Order Processing Notification Service
    in the Amazon merchant account;
  - Amazon widget settings: settings about the display of the "Pay with Amazon" button;
    Available settings:
    - Size of the button: Medium (126x24), Large (151x27), Extra large (173x27);
    - Color of the button: Orange, Tan;
    - Background of the button: White, Dark, Light;
    - Width: dimmension for the inline widgets.
    - Height: dimmension for the inline widgets.
  - Hide the regular checkout cart button: if the "Pay with Amazon" should be
    the only checkout button available.
  - Debugging: Settings about debugging data,
    either to be displayed as page messages or to be logged in system log,
    requires some permissions (see permissions configuration).


HOW IT WORKS
------------

* General considerations:
  - Shop owner must have an Amazon merchant account
    Sign up now
    USA : https://sellercentral.amazon.com/
    UK : https://payments.amazon.co.uk/business/pre-registration?ld=SPEXUKCBADrupal
    DE : https://payments.amazon.de/business/pre-registration?ld=SPEXDECBADrupal
  - Customer should have an Amazon account
    USA : https://www.amazon.com/account
    DE : https://payments.amazon.de/personal
    UK : https://payments.amazon.co.uk/personal
  - Customer authenticates to Amazon account and use the Address and Wallet credentials
    full integrated with the Drupal commerce shop.
    https://payments.amazon.com
    Customer can use any existing world-wide Amazon buyer account or
    create a new one during the checkout process.

* Checkout workflow:
  1. Inline checkout
  - (Cart page/block) Inline checkout button
  - Amazon authentication (popup)
  - (Checkout page) Amazon Address widget to select addresses from Amazon
    to be used for the Order - data will be saved as data of Drupal customer profiles
  - Other checkout pages
  - (Payment page) Amazon wallet widget to select a wallet to be used for payment
  - (Checkout complete page) Amazon order details widget available to display amazon data about the order.

  2. Express checkout
  - (Cart page/block) Express checkout button
  - Amazon authentication (popup)
    (still popup) Select the address and the wallet to be used
  - (Checkout page) Amazon Address widgets in "Read" mode with the selected address from Amazon,
    will be used for Drupal customer profiles?
  - Other checkout pages
  - (Payment page) Amazon Wallet widget in "Read" mode with selected Amazon wallet data.
  - (Checkout complete page) Amazon order details widget available to display amazon data about the order.

  The suggested checkout pages, encloses in brackets, are just for a standard Drupal commerce store.
  They could differ depends on Drupal commerce store checkout configuration.

* IOPN service
  Amazon Instant Order Processing Notification Service.
  You have to configure the Merchant URL for Amazon IOPN Service
  in Amazon Seller Central under "Settings >
  Checkout Pipeline Settings, Instant Order Processing Notification Settings >
  Merchant URL".
  Example URL to configure is https://yourdomain/commerce-cba-iopn.
  A valid SSL certificate is required to use the IOPN Service.
  Make sure you configure in "Production View" (and "Sandbox View" if you want to test in sandbox).
  Order Notification Types:
  Order notifications are HTTPS POST requests containing the notification data formatted as XML.
  You can receive one of three types of notifications:
  1. New Order Notification
     Amazon sends to Drupal this notification when a new order is placed.
     Note: Do not ship an order when you receive the New Order notification.
           Please wait until you receive the Order Ready-to-Ship notification.
     Rules event invoked, default module for this event rule will complete the shipping address
     if exists and will set the Order on "Pending, Amazon - New order" (state, status).
  2. Order Ready-to-Ship Notification
     Amazon sends this notification when it authorizes the buyer's payment method.
     This notification indicates that you can fulfill the order (send the items to the buyer).
     Rules event invoked, default module for this event rule will fill the billing address
     from Amazon and will set the Order on "Pending, Amazon - Ready to Ship" (state, status).
  3. Order Canceled Notification
     Amazon sends this notification when the order is canceled. An order can be canceled
     by the seller, the buyer, or Amazon Payments.
     Rules event invoked, default module for this event rule will cancel the order.


TROUBLESHOOTING
---------------
* In order to work the store order in the cart should have the same currency
  as the Amazon mechant account:
  - EUR for a German account.
  - GBP for an UK account.
  No Amazon checkout button if the currency will not match.
  Helper currency module for multicurrencies: Commerce Multicurrency
  (https://www.drupal.org/project/commerce_multicurrency)
  which enhances some of the multi-currency capabilities of Drupal Commerce.


MAINTAINERS
-----------
Current maintainers:
* Tavi Toporjinschi (vasike) - https://www.drupal.org/u/vasike

Past maintainers:
* Julien Dubreuil (JulienD) - https://www.drupal.org/user/519520
* Pedro Cambra (pcambra) - https://www.drupal.org/u/pcambra

This project has been developed by:
* Commerce Guys
  Commerce Guys are the creators of and experts in Drupal Commerce,
  the eCommerce solution that capitalizes on the virtues and power of Drupal,
  the premier open-source content management system.
  We focus our knowledge and expertise on providing online merchants with
  the powerful, responsive, innovative eCommerce solutions they need to thrive.
  Visit https://commerceguys.com/ for more information.
