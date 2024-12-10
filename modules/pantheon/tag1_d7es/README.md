# tag1_d7es

Drupal 7 module to support Tag1's D7ES offering.

Installation and use of this module constitutes your acceptance of and agreement
to be bound by Tag1 Consulting Inc's Terms of Service and Privacy Policy,
as published at D7ES.Tag1.com.

## Requirements
* A properly configured [cron job](https://www.drupal.org/docs/7/setting-up-cron/overview)
* An active customer account created at https://d7es.tag1.com/

## Configuration
1. Download and enable the module as usual.
2. Visit the module's configuration page at `/admin/config/system/tag1-d7es` and
enter the Billing email address used when creating the customer account. You
will also need to supply one or more email addresses that should be notified of
available security updates.
3. Save the form. Status messages will let you know if the site is authenticated
and sending data successfully.

## How does it work?
This module will report the following information about your site to the Tag1
D7ES team:

* the version of Drupal core
* name and version of contributed modules and themes
* database type and version
* PHP version
* the URL of your website
* the billing account email address
* list of email addresses who should be notified when a security update is available

This report is submitted via a cron job a maximum of once per 24 hours.

The full JSON payload sent by your site may be previewed from the module's
configuration page under the "Debugging" fieldset.

## For extra security

For extra security, you may wish to configure the "Billing email address" and
"Notification email addresses" outside of the site's database. You can use the
variables `tag1_d7es_billing_email` and `tag1_d7es_email_addresses` to define
these values directly in settings.php, or by calling an environment variable.

```php
// Including value in code.
$conf['tag1_d7es_billing_email'] = 'my-billing@email.com';

// Sourcing from an environment variable.
$conf['tag1_d7es_billing_email'] = $_ENV['MY_BILLING_EMAIL_VARIABLE'];
```
