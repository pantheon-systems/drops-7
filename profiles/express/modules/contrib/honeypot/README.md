
# Honeypot

[![Build Status](https://travis-ci.org/geerlingguy/drupal-honeypot.svg?branch=7.x-1.x)](https://travis-ci.org/geerlingguy/drupal-honeypot)


## Installation

To install this module, place it in your sites/all/modules folder and enable it
on the modules page.


## Configuration

All settings for this module are on the Honeypot configuration page, under the
Configuration section, in the Content authoring settings. You can visit the
configuration page directly at admin/config/content/honeypot.

Note that, when testing Honeypot on your website, make sure you're not logged in
as an administrative user or user 1; Honeypot allows administrative users to
bypass Honeypot protection, so by default, Honeypot will not be added to forms
accessed by site administrators.


## Use in Your Own Forms

If you want to add honeypot to your own forms, or to any form through your own
module's hook_form_alter's, you can simply place the following function call
inside your form builder function (or inside a hook_form_alter):

    honeypot_add_form_protection(
      $form,
      $form_state,
      array('honeypot', 'time_restriction')
    );

Note that you can enable or disable either the honeypot field, or the time
restriction on the form by including or not including the option in the array.


## Testing

Honeypot includes a `docker-compose.yml` file that can be used for testing purposes. To build a Drupal 8 environment for local testing, do the following:

  1. Make sure you have Docker for Mac (or for whatever OS you're using) installed.
  2. Add the following entry to your `/etc/hosts` file: `192.168.22.33   local.drupalhoneypot.com`
  3. Run `docker-compose up -d` in this directory.
  4. Install Drupal: `docker exec honeypot install-drupal 7.x` (optionally provide a version after `install-drupal`).
  5. Link the honeypot module directory into the Drupal modules directory: `docker exec honeypot ln -s /opt/honeypot/ /var/www/drupalvm/drupal/web/sites/all/modules/honeypot`
  6. Visit `http://local.drupalhoneypot.com/user` and log in using the admin credentials Drush displayed.

> Note: If you're using a Mac, you may also need to perform additional steps to get the hostname working; see [Managing your hosts file](http://docs.drupalvm.com/en/latest/other/docker/#managing-your-hosts-file) in the Drupal VM documentation.


## Credit

The Honeypot module was originally developed by Jeff Geerling of Midwestern Mac,
LLC (midwesternmac.com), and sponsored by Flocknote (flocknote.com).
