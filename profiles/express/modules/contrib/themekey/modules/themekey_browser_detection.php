<?php

/**
 * @file
 * Derived from Browser Detection Class by Dragan Dinic <dragan@dinke.net>
 * and modified to fit the needs of ThemeKey Properties.
 * @see http://www.dinke.net/blog/2005/10/30/browser-detection/en/
 * @see themekey_properties.module
 *
 * Here's a snippet of Dragans mail response as we asked him to reuse his code:
 *
 * > Hello Markus,
 * >
 * > Thank you very much for contacting me. You can use Browser detection
 * > class in any project, no matter of its purpose (commercial or non
 * > commerical).
 * >
 * > I wasnt' sure which license allows that freedom of use (perhaps BSD
 * > license), if you know more about that please let me know.
 * >
 * > Best Regards,
 * > Dragan
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 */

/**
 * Browser Detection class
 * contains common static method for
 * getting browser version and OS
 *
 * It supports most popular browsers (IE, FF, Safari, Opera, Chrome ...),
 * as well as some not-so-popular (lynx etc.)
 * It doesn't recognize bots (like google, yahoo etc)
 *
 * usage
 * <code>
 * $browser = ThemekeyBrowserDetection::getBrowser($_SERVER['HTTP_USER_AGENT']);
 * $os = Browser_Detection::getOs($_SERVER['HTTP_USER_AGENT']);
 * </code>
 * @access public
 */
class ThemekeyBrowserDetection {

  /**
   * Get browser name and version
   * @param string user agent
   * @return string browser name and version or 'unknown' if unrecognized
   * @static
   * @access public
   */
  function getBrowser($useragent) {
    // check for most popular browsers first
    // unfortunately, that's IE. We also ignore Opera and Netscape 8
    // because they sometimes send msie agent
    if (strpos($useragent, 'MSIE') !== FALSE && strpos($useragent, 'Opera') === FALSE && strpos($useragent, 'Netscape') === FALSE) {
      //deal with Blazer
      if (preg_match("/Blazer\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Blazer ' . $matches[1];
      }
      //deal with IE
      if (preg_match("/MSIE ([0-9]{1}\.[0-9]{1,2})/", $useragent, $matches)) {
        return 'Internet Explorer ' . $matches[1];
      }
    }
    elseif (strpos($useragent, 'Gecko')) {
      //deal with Gecko based

      //if firefox
      if (preg_match("/Firefox\/([0-9]{1,2}\.[0-9]{1,2}(\.[0-9]{1,2})?)/", $useragent, $matches)) {
        return 'Mozilla Firefox ' . $matches[1];
      }

      //if Netscape (based on gecko)
      if (preg_match("/Netscape\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Netscape ' . $matches[1];
      }

      //check chrome before safari because chrome agent contains both
      if (preg_match("/Chrome\/([^\s]+)/", $useragent, $matches)) {
        return 'Google Chrome ' . $matches[1];
      }

      //if Safari (based on gecko)
      if (preg_match("/Safari\/([0-9]{2,3}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Safari ' . $matches[1];
      }

      //if Galeon (based on gecko)
      if (preg_match("/Galeon\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Galeon ' . $matches[1];
      }

      //if Konqueror (based on gecko)
      if (preg_match("/Konqueror\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Konqueror ' . $matches[1];
      }

      // if Fennec (based on gecko)
      if (preg_match("/Fennec\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Fennec' . $matches[1];
      }

      // if Maemo (based on gecko)
      if (preg_match("/Maemo\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Maemo' . $matches[1];
      }

      //no specific Gecko found
      //return generic Gecko
      return 'Gecko based';
    }
    elseif (strpos($useragent, 'Opera') !== FALSE) {
      //deal with Opera
      if (preg_match("/Opera[\/ ]([0-9]{1}\.[0-9]{1}([0-9])?)/", $useragent, $matches)) {
        return 'Opera ' . $matches[1];
      }
    }
    elseif (strpos($useragent, 'Lynx') !== FALSE) {
      //deal with Lynx
      if (preg_match("/Lynx\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Lynx ' . $matches[1];
      }
    }
    elseif (strpos($useragent, 'Netscape') !== FALSE) {
      //NN8 with IE string
      if (preg_match("/Netscape\/([0-9]{1}\.[0-9]{1}(\.[0-9])?)/", $useragent, $matches)) {
        return 'Netscape ' . $matches[1];
      }
    }
    else {
      //unrecognized, this should be less than 1% of browsers (not counting bots like google etc)!
      return 'unknown';
    }
  }

  /**
   * Get browsername simplified
   * @param string browser
   * @return string browser name or 'unknown' if unrecognized
   * @static
   * @access public
   */
  function getBrowserSimplified($browser) {
    return trim(preg_replace('/[0-9.]/', '', $browser));
  }

  /**
   * Get operating system
   * @param string user agent
   * @return string os name and version or 'unknown' in unrecognized os
   * @static
   * @access public
   */
  function getOs($useragent) {
    $useragent = drupal_strtolower($useragent);

    //check for (aaargh) most popular first
    //winxp
    if (strpos($useragent, 'windows nt 5.1') !== FALSE) {
      return 'Windows XP';
    }
    elseif (strpos($useragent, 'windows nt 6.1') !== FALSE) {
      return 'Windows 7';
    }
    elseif (strpos($useragent, 'windows nt 6.0') !== FALSE) {
      return 'Windows Vista';
    }
    elseif (strpos($useragent, 'windows 98') !== FALSE) {
      return 'Windows 98';
    }
    elseif (strpos($useragent, 'windows nt 5.0') !== FALSE) {
      return 'Windows 2000';
    }
    elseif (strpos($useragent, 'windows nt 5.2') !== FALSE) {
      return 'Windows 2003 server';
    }
    elseif (strpos($useragent, 'windows nt') !== FALSE) {
      return 'Windows NT';
    }
    elseif (strpos($useragent, 'win 9x 4.90') !== FALSE && strpos($useragent, 'win me')) {
      return 'Windows ME';
    }
    elseif (strpos($useragent, 'win ce') !== FALSE) {
      return 'Windows CE';
    }
    elseif (strpos($useragent, 'win 9x 4.90') !== FALSE) {
      return 'Windows ME';
    }
    elseif (strpos($useragent, 'iphone') !== FALSE) {
      return 'iPhone';
    }
    // experimental
    elseif (strpos($useragent, 'ipad') !== FALSE) {
      return 'iPad';
    }
    elseif (strpos($useragent, 'webos') !== FALSE) {
      return 'webOS';
    }
    elseif (strpos($useragent, 'symbian') !== FALSE) {
      return 'Symbian';
    }
    elseif (strpos($useragent, 'android') !== FALSE) {
      return 'Android';
    }
    elseif (strpos($useragent, 'blackberry') !== FALSE) {
      return 'Blackberry';
    }
    elseif (strpos($useragent, 'mac os x') !== FALSE) {
      return 'Mac OS X';
    }
    elseif (strpos($useragent, 'macintosh') !== FALSE) {
      return 'Macintosh';
    }
    elseif (strpos($useragent, 'linux') !== FALSE) {
      return 'Linux';
    }
    elseif (strpos($useragent, 'freebsd') !== FALSE) {
      return 'Free BSD';
    }
    elseif (strpos($useragent, 'symbian') !== FALSE) {
      return 'Symbian';
    }
    else {
      return 'unknown';
    }
  }

  /**
   * Get operating system simplified
   * @param string os
   * @return string os name or 'unknown' in unrecognized os
   * @static
   * @access public
   */
  function getOsSimplified($os) {
    if (strpos($os, 'Windows') !== FALSE) {
      return 'Windows';
    }
    else {
      return $os;
    }
  }
}
