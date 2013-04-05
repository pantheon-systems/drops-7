CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Installation
 * User permissions


INTRODUCTION
 ------------

 Current Maintainer:
 * Julien Dubreuil (JulienD) - http://drupal.org/user/519520

 The commerce_hosted_pci modules integrates Hosted Pci into the Drupal Commerce
 payment and checkout systems. This module is an on-site payment solution and
 allow to react to authorizations, captures, voids, and refunds features.

 Authorization only transactions can be captured later on the site instead of
 the authorization plus capture which capture directly the amount to the
 customer.


REQUIREMENTS
------------

 Hosted Pci account (http://www.hostedpci.com)


INSTALLATION
------------

 1. Download and extract the module's tarball (*.tar.gz archive file) into
    your Drupal site's contributed/custom modules directory:

    /sites/all/modules

 2. Enable the module from the site's module page:

    Administration > Modules

 3. Configure the payment method on the payment methods lists:

    Administration > Store settings > Advanced store settings > Payment methods

    Click edit on the rules titled "Hosted PCI - Credit Card".

    Click on the edit button located in the "Action" area at the bottom of the
    page to access to the payment configuration.

    Fill out the form with your credentials under "Payment settings" and save
    the form.

 4. Enable the Hosted Pci payment methode by clicking the enable link on the
    payment method overview

    Administration > Store settings > Advanced store settings > Payment methods


USER PERMISSIONS
----------------

 The module provides user/role permission which can be granted at:

      Administration > People > Permissions
