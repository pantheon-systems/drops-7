reCAPTCHA for Drupal
====================

The reCAPTCHA module uses the reCAPTCHA web service to
improve the CAPTCHA system and protect email addresses. For
more information on what reCAPTCHA is, please visit:
    https://www.google.com/recaptcha


DEPENDENCIES
------------

* reCAPTCHA depends on the CAPTCHA module.
  https://drupal.org/project/captcha
* Some people have found that they also need to use jQuery Update module.
  https://drupal.org/project/jquery_update


CONFIGURATION
-------------

1. Enable reCAPTCHA and CAPTCHA modules in:
       admin/modules

2. You'll now find a reCAPTCHA tab in the CAPTCHA
   administration page available at:
       admin/config/people/captcha/recaptcha

3. Register for a public and private reCAPTCHA key at:
       https://www.google.com/recaptcha/whyrecaptcha

4. Input the keys into the reCAPTCHA settings. The rest of
   the settings should be fine as their defaults.

5. Visit the Captcha administration page and set where you
   want the reCAPTCHA form to be presented:
       admin/config/people/captcha


MAILHIDE INPUT FORMAT
---------------------

The reCAPTCHA module also comes with an input format to
protect email addresses. This, of course, is optional to
use and is only there if you want it. The following is how
you use that input filter:

1. Enable the reCAPTCHA Mailhide module:
       admin/modules

2. Head over to your text format settings:
       admin/config/content/formats

3. Edit your default input format and add the reCAPTCHA
   Mailhide filter.

4. Click on the Configure tab and put in a public and
   private Mailhide key obtained from:
       https://www.google.com/recaptcha/mailhide/apikey

5. Use the Rearrange tab to rearrange the weight of the
   filter depending on what filters already exist.  Make
   sure it is before the URL Filter.

Note: You will require the installation of the mcrypt
      PHP module in your web server for Mailhide to work:
         http://php.net/manual/en/ref.mcrypt.php


MULTI-DOMAIN SUPPORT
--------------------

Since reCAPTCHA uses API keys that are unique to each
domain, if you're using a multi-domain system using the
same database, the reCAPTCHA module won't work when
querying the reCAPTCHA web service.  If you put the
following into your sites/mysite/settings.php file for
each domain, it will override the API key values and make
it so multi-domain systems are capable.

  $conf = array(
    'recaptcha_public_key' =>  'my other public key',
    'recaptcha_private_key' =>  'my other private key',
  );


CUSTOM RECAPTCHA THEME
----------------------

You can create a custom reCAPTCHA theme widget by setting
the theme of the reCAPTCHA form to "custom" in the
reCAPTCHA administration page.  This will output a custom
form that is themeable through the theme function:
  theme_recaptcha_custom_widget().

If you don't implement this function, it is still quite
easily customizable through manipulating the CSS.

For more information on this, visit:
https://developers.google.com/recaptcha/docs/customization


THANK YOU
---------

 * Thank you goes to the reCAPTCHA team for all their
   help, support and their amazing Captcha solution
       https://www.google.com/recaptcha
