reCAPTCHA for Drupal
====================

The reCAPTCHA module uses the reCAPTCHA web service to
improve the CAPTCHA system and protect email addresses. For
more information on what reCAPTCHA is, please visit:
    https://www.google.com/recaptcha

This version of the module uses the new Google No CAPTCHA reCAPTCHA API.

DEPENDENCIES
------------

* reCAPTCHA module depends on the CAPTCHA module.
  https://drupal.org/project/captcha


CONFIGURATION
-------------

1. Enable reCAPTCHA and CAPTCHA modules in:
       admin/modules

2. You'll now find a reCAPTCHA tab in the CAPTCHA
   administration page available at:
       admin/config/people/captcha/recaptcha

3. Register your web site at
       https://www.google.com/recaptcha/admin/create

4. Input the site and private keys into the reCAPTCHA settings.

5. Visit the Captcha administration page and set where you
   want the reCAPTCHA form to be presented:
       admin/config/people/captcha

KNOWN ISSUES
------------

- The PHP setting 'arg_separator.output' set by Drupal core causes conflicts
  with the Google reCAPTCHA library. This setting is and was never used by
  Drupal core, but still exist in "settings.php" and need to be removed.
    
  See https://www.drupal.org/node/2476237 for more information.

- cURL requests fail because of outdated root certificate. The reCAPTCHA module
  may not able to connect to Google servers and fails to verify the answer.
  
  See https://www.drupal.org/node/2481341 for more information.

- If reCAPTCHA is assigned to user login form and user login block is used in
  narrow sidebars the widget may overflow the sidebar. Google is aware of this
  problem, but they currently have the wide widget with fixed size only.
  See https://github.com/google/recaptcha/issues/47

THANK YOU
---------

 * Thank you goes to the reCAPTCHA team for all their
   help, support and their amazing Captcha solution
       https://www.google.com/recaptcha
