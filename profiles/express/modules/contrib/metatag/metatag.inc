<?php

/**
 * @file
 * Metatag primary classes.
 */

/**
 * The master interface for all tags.
 */
interface DrupalMetaTagInterface {

  /**
   * Constructor.
   *
   * @param array $info
   *   The information about the meta tag from metatag_get_info().
   */
  function __construct(array $info, array $data = array());

  public function getForm();

  //function validateForm();

  //function processForm();

  public function getValue();

  function getWeight();

  function getElement();

  function tidyValue($value);

  function convertUrlToAbsolute($url);

  function truncate($value);

  function maxlength();

  static function text_summary($text, $size);

}

/**
 * The default meta tag class from which all others inherit.
 */
class DrupalDefaultMetaTag implements DrupalMetaTagInterface {

  /**
   * All of the basic information about this tag.
   *
   * @var array
   */
  protected $info;

  /**
   * The values submitted for this tag.
   *
   * @var array
   */
  protected $data = array('value' => '');

  /**
   * This item's weight; used for sorting the output.
   *
   * @var float
   */
  protected $weight = 0;

  /**
   * Constructor.
   */
  function __construct(array $info, array $data = NULL) {
    $this->info = $info;
    if (isset($data)) {
      $this->data = $data;
    }
  }

  /**
   * Calculate the weight of this meta tag.
   *
   * @return int
   *   Weight.
   */
  function getWeight() {
    static $counter = 0;

    // If no weight value is found, stack this meta tag at the end.
    $weight = 100;
    if (!empty($this->info['weight'])) {
      $weight = $this->info['weight'];
    }

    return $weight + ($counter++ * 0.1);
  }

  /**
   * Build the form for this meta tag.
   *
   * @return array
   *   A standard FormAPI array.
   */
  public function getForm(array $options = array()) {
    return array();
  }

  /**
   * Get the string value of this meta tag.
   *
   * @return string
   *   The value of this meta tag.
   */
  public function getValue(array $options = array()) {
    $value = $this->tidyValue($this->data['value']);

    // Translate the final output string prior to output. Use the
    // 'output' i18n_string object type, and pass along the meta tag's
    // options as the context so it can be handled appropriately.
    $value = metatag_translate_metatag($value, $this->info['name'], $options, NULL, TRUE);

    return $this->truncate($this->tidyValue($this->data['value']));
  }

  /**
   * Get the HTML tag for this meta tag.
   *
   * @return array
   *   A render array for this meta tag.
   */
  public function getElement(array $options = array()) {
    $value = $this->getValue($options);
    if (strlen($value) === 0) {
      return array();
    }

    // The stack of elements that will be output.
    $elements = array();

    // Dynamically add each option to this setting.
    $base_element = isset($this->info['element']) ? $this->info['element'] : array();

    // Single item.
    if (empty($this->info['multiple'])) {
      $values = array($value);
    }

    // Multiple items.
    else {
      $values = array_filter(explode(',', $value));
    }

    // Loop over each item.
    if (!empty($values)) {
      foreach ($values as $ctr => $value) {
        $value = trim($value);

        // Some meta tags must be output as secure URLs.
        if (!empty($this->info['secure'])) {
          $value = str_replace('http://', 'https://', $value);
        }

        // Combine the base configuration for this meta tag with the value.
        $element = $base_element + array(
          '#theme' => 'metatag',
          '#tag' => 'meta',
          '#id' => 'metatag_' . $this->info['name'] . '_' . $ctr,
          '#name' => $this->info['name'],
          '#value' => $value,
          '#weight' => $this->getWeight(),
        );

        // Add header information if desired.
        if (!empty($this->info['header'])) {
          $element['#attached']['drupal_add_http_header'][] = array($this->info['header'], $value);
        }

        $elements[] = array($element, $element['#id']);
      }
    }

    if (!empty($elements)) {
      return array(
        '#attached' => array('drupal_add_html_head' => $elements),
      );
    }
  }

  /**
   * Remove unwanted formatting from a meta tag.
   *
   * @param string $value
   *   The meta tag value to be tidied up.
   *
   * @return string
   *   The meta tag value after it has been tidied up.
   */
  public function tidyValue($value) {
    // Check for Media strings from the WYSIWYG submodule.
    if (module_exists('media_wysiwyg') && strpos($value, '[[{') !== FALSE) {
      // In https://www.drupal.org/node/2129273 media_wysiwyg_filter() was
      // changed to require several additional arguments.
      $langcode = language_default('language');
      $value = media_wysiwyg_filter($value, NULL, NULL, $langcode, NULL, NULL);
    }

    // Specifically replace encoded spaces, because some WYSIWYG editors are
    // silly. Do this before decoding the other HTML entities so that the output
    // doesn't end up with a bunch of a-circumflex characters.
    $value = str_replace('&nbsp;', ' ', $value);

    // Decode HTML entities.
    $value = decode_entities($value);

    // Remove any HTML code that might have been included.
    $value = strip_tags($value);

    // Strip errant whitespace.
    $value = str_replace(array("\r\n", "\n", "\r", "\t"), ' ', $value);
    $value = str_replace('  ', ' ', $value);
    $value = str_replace('  ', ' ', $value);
    $value = trim($value);

    return $value;
  }

  /**
   * Make sure a given URL is absolute.
   *
   * @param string $url
   *   The URL to convert to an absolute URL.
   *
   * @return string
   *   The argument converted to an absolute URL.
   */
  function convertUrlToAbsolute($url) {
    // Convert paths relative to the hostname, that start with a slash, to
    // ones that are relative to the Drupal root path; ignore protocol-relative
    // URLs.
    if (strpos($url, base_path()) === 0 && strpos($url, '//') !== 0) {
      // Logic:
      // * Get the length of the base_path(),
      // * Get a portion of the image's path starting from the position equal
      //   to the base_path()'s length; this will result in a path relative
      //   to the Drupal installation's base directory.
      $len = strlen(base_path());
      $url = substr($url, $len);
    }

    // Pass everything else through file_create_url(). The alternative is to
    // use url() but it would insert '?q=' into the path.
    return file_create_url($url);
  }

  /**
   * Shorten a string to a certain length using text_summary().
   *
   * @param string $value
   *   String to shorten.
   *
   * @return string
   *   Shortened string.
   */
  function truncate($value) {
    $maxlength = $this->maxlength();

    if (!empty($value) && $maxlength > 0) {
      $value = $this->text_summary($value, $maxlength);
    }

    return $value;
  }

  /**
   * Identify the maximum length of which strings will be allowed.
   *
   * @return int
   *   Maxlenght.
   */
  function maxlength() {
    if (isset($this->info['maxlength'])) {
      return intval(variable_get('metatag_maxlength_' . $this->info['name'], $this->info['maxlength']));
    }
    return 0;
  }

  /**
   * Copied from text.module with the following changes:.
   *
   * Change 1: $size is required.
   * Change 2: $format is removed.
   * Change 3: Don't trim at the end of short sentences
   *   (https://www.drupal.org/node/1620104).
   * Change 4: Word boundaries (https://www.drupal.org/node/1482178).
   * Change 5: Trim the final string.
   */
  static function text_summary($text, $size) {
    // if (!isset($size)) {
    //   // What used to be called 'teaser' is now called 'summary', but
    //   // the variable 'teaser_length' is preserved for backwards compatibility.
    //   $size = variable_get('teaser_length', 600);
    // }
    // Find where the delimiter is in the body.
    $delimiter = strpos($text, '<!--break-->');

    // If the size is zero, and there is no delimiter,
    // the entire body is the summary.
    if ($size == 0 && $delimiter === FALSE) {
      return $text;
    }

    // If a valid delimiter has been specified, use it to chop off the summary.
    if ($delimiter !== FALSE) {
      return substr($text, 0, $delimiter);
    }

    // We check for the presence of the PHP evaluator filter in the current
    // format. If the body contains PHP code, we do not split it up to prevent
    // parse errors.
    // if (isset($format)) {
    //   $filters = filter_list_format($format);
    //   if (isset($filters['php_code']) && $filters['php_code']->status && strpos($text, '<?') !== FALSE) {
    //     return $text;
    //   }
    // }
    // If we have a short body, the entire body is the summary.
    if (drupal_strlen($text) <= $size) {
      return $text;
    }

    // If the delimiter has not been specified, try to split at paragraph or
    // sentence boundaries.
    // The summary may not be longer than maximum length specified.
    // Initial slice.
    $summary = truncate_utf8($text, $size);

    // Store the actual length of the UTF8 string -- which might not be the same
    // as $size.
    $max_rpos = strlen($summary);

    // How much to cut off the end of the summary so that it doesn't end in the
    // middle of a paragraph, sentence, or word.
    // Initialize it to maximum in order to find the minimum.
    $min_rpos = $max_rpos;

    // Store the reverse of the summary. We use strpos on the reversed needle
    // and haystack for speed and convenience.
    $reversed = strrev($summary);

    // Build an array of arrays of break points grouped by preference.
    $break_points = array();

    // A paragraph near the end of sliced summary is most preferable.
    $break_points[] = array('</p>' => 0);

    // If no complete paragraph then treat line breaks as paragraphs.
    // $line_breaks = array('<br />' => 6, '<br>' => 4);
    // Newline only indicates a line break if line break converter
    // filter is present.
    // if (isset($filters['filter_autop'])) {
    //   $line_breaks["\n"] = 1;
    // }
    // $break_points[] = $line_breaks;
    // If the first paragraph is too long, split at the end of a sentence.
    // $break_points[] = array('. ' => 1, '! ' => 1, '? ' => 1, '。' => 0, '؟ ' => 1);
    // From https://www.drupal.org/node/1482178.
    // If the first sentence is too long, split at the first word break.
    $word_breaks = array(' ' => 0, "\t" => 0);
    $break_points[] = $word_breaks;

    // Iterate over the groups of break points until a break point is found.
    foreach ($break_points as $points) {
      // Look for each break point, starting at the end of the summary.
      foreach ($points as $point => $offset) {
        // The summary is already reversed, but the break point isn't.
        $rpos = strpos($reversed, strrev($point));
        if ($rpos !== FALSE) {
          $min_rpos = min($rpos + $offset, $min_rpos);
        }
      }

      // If a break point was found in this group, slice and stop searching.
      if ($min_rpos !== $max_rpos) {
        // Don't slice with length 0. Length must be <0 to slice from RHS.
        $summary = ($min_rpos === 0) ? $summary : substr($summary, 0, 0 - $min_rpos);
        break;
      }
    }

    // If the htmlcorrector filter is present, apply it to the generated
    // summary.
    // if (isset($filters['filter_htmlcorrector'])) {
    //   $summary = _filter_htmlcorrector($summary);
    // }
    return trim($summary);
  }

}

/**
 * Text-based meta tag controller.
 */
class DrupalTextMetaTag extends DrupalDefaultMetaTag {

  /**
   * {@inheritdoc}
   */
  public function getForm(array $options = array()) {
    $options += array(
      'token types' => array(),
    );

    $form['value'] = isset($this->info['form']) ? $this->info['form'] : array();

    $form['value'] += array(
      '#type' => 'textfield',
      '#title' => $this->info['label'],
      '#description' => !empty($this->info['description']) ? $this->info['description'] : '',
      '#default_value' => isset($this->data['value']) ? $this->data['value'] : '',
      '#element_validate' => array('token_element_validate'),
      '#token_types' => $options['token types'],
      '#maxlength' => 1024,
    );

    // Optional handling for items that allow multiple values.
    if (!empty($this->info['multiple'])) {
      $form['value']['#description'] .= ' ' . t('Multiple values may be used, separated by a comma. Note: Tokens that return multiple values will be handled automatically.');
    }

    // Optionally limit the field to a certain length.
    $maxlength = $this->maxlength();
    if (!empty($maxlength)) {
      $form['value']['#description'] .= ' ' . t('This will be truncated to a maximum of %max characters.', array('%max' => $maxlength));
    }

    // Optional handling for images.
    if (!empty($this->info['image'])) {
      $form['value']['#description'] .= ' ' . t('This will be able to extract the URL from an image field.');
    }

    // Optional handling for languages.
    if (!empty($this->info['is_language'])) {
      $form['value']['#description'] .= ' ' . t('This will not be displayed if it is set to the "Language neutral" (i.e. "und").');
    }

    // Optional support for select_or_other.
    if ($form['value']['#type'] == 'select' && !empty($this->info['select_or_other']) && module_exists('select_or_other')) {
      $form['value']['#type'] = 'select_or_other';
      $form['value']['#other'] = t('Other (please type a value)');
      $form['value']['#multiple'] = FALSE;
      $form['value']['#other_unknown_defaults'] = 'other';
      $form['value']['#other_delimiter'] = FALSE;
      $form['value']['#theme'] = 'select_or_other';
      $form['value']['#select_type'] = 'select';
      $form['value']['#element_validate'] = array('select_or_other_element_validate');
    }

    // Support for dependencies, using Form API's #states system.
    // @see metatag.api.php.
    // @see https://api.drupal.org/drupal_process_states
    if (!empty($this->info['dependencies'])) {
      foreach ($this->info['dependencies'] as $specs) {
        $form['value']['#states']['visible'][':input[name*="[' . $specs['dependency'] . '][' . $specs['attribute'] . ']"]'] = array(
          $specs['condition'] => $specs['value'],
        );
      }
    }

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function getValue(array $options = array()) {
    $options += array(
      'instance' => '',
      'token data' => array(),
      // Remove any remaining token after the string is parsed.
      'clear' => TRUE,
      'sanitize' => variable_get('metatag_token_sanitize', FALSE),
      'raw' => FALSE,
    );

    $value = $this->data['value'];

    if (empty($options['raw'])) {
      // Give other modules the opportunity to use hook_metatag_pattern_alter()
      // to modify defined token patterns and values before replacement.
      drupal_alter('metatag_pattern', $value, $options['token data'], $this->info['name']);
      $value = token_replace($value, $options['token data'], $options);
    }

    // Special handling for language meta tags.
    if (!empty($this->info['is_language'])) {
      // If the meta tag value equals LANGUAGE_NONE, i.e. "und", then don't
      // output it.
      if (is_string($value) && $value == LANGUAGE_NONE) {
        $value = '';
      }
    }

    // Special handling for images and other URLs.
    if (!empty($this->info['image']) || !empty($this->info['url'])) {
      // Support multiple items, whether it's needed or not. Also remove the
      // empty values and reindex the array.
      $values = array_values(array_filter(explode(',', $value)));

      // If this meta tag does *not* allow multiple items, only keep the first
      // one.
      if (empty($this->info['multiple']) && !empty($values[0])) {
        $values = array($values[0]);
      }

      foreach ($values as $key => &$image_value) {
        // Remove any unwanted whitespace around the value.
        $image_value = trim($image_value);

        // If this contains embedded image tags, extract the image URLs.
        if (!empty($this->info['image']) && strip_tags($image_value) != $image_value) {
          $matches = array();
          preg_match('/src="([^"]*)"/', $image_value, $matches);
          if (!empty($matches[1])) {
            $image_value = $matches[1];
          }
        }

        // Convert the URL to an absolute URL.
        $image_value = $this->convertUrlToAbsolute($image_value);

        // Replace spaces the URL encoded entity to avoid validation problems.
        $image_value = str_replace(' ', '%20', $image_value);
      }

      // Combine the multiple values into a single string.
      $value = implode(',', $values);
    }

    // Clean up the string a bit.
    $value = $this->tidyValue($value);

    // Optionally truncate the value.
    $value = $this->truncate($value);

    // Translate the final output string prior to output. Use the
    // 'output' i18n_string object type, and pass along the meta tag's
    // options as the context so it can be handled appropriately.
    $value = metatag_translate_metatag($value, $this->info['name'], $options, NULL, TRUE);

    return $value;
  }

}

/**
 * Link type meta tag controller.
 */
class DrupalLinkMetaTag extends DrupalTextMetaTag {

  /**
   * {@inheritdoc}
   */
  public function getElement(array $options = array()) {
    $element = isset($this->info['element']) ? $this->info['element'] : array();

    $value = $this->getValue($options);
    if (strlen($value) === 0) {
      return array();
    }

    $element += array(
      '#theme' => 'metatag_link_rel',
      '#tag' => 'link',
      '#id' => 'metatag_' . $this->info['name'],
      '#name' => $this->info['name'],
      '#value' => $value,
      '#weight' => $this->getWeight(),
    );

    if (!isset($this->info['header']) || !empty($this->info['header'])) {
      // Also send the generator in the HTTP header.
      // @todo This does not support 'rev' or alternate link headers.
      $element['#attached']['drupal_add_http_header'][] = array(
        'Link', '<' . $value . '>;' . drupal_http_header_attributes(array('rel' => $element['#name'])), TRUE,
      );
    }

    return array(
      '#attached' => array('drupal_add_html_head' => array(array($element, $element['#id']))),
    );
  }

}

/**
 * Title meta tag controller.
 *
 * This extends DrupalTextMetaTag as we need to alter variables in
 * template_preprocess_html() rather output a normal meta tag.
 */
class DrupalTitleMetaTag extends DrupalTextMetaTag {

  /**
   * {@inheritdoc}
   */
  public function getElement(array $options = array()) {
    $element = array();
    if ($value = $this->getValue($options)) {
      $element['#attached']['metatag_set_preprocess_variable'][] = array(
        'html',
        'head_title',
        decode_entities($value),
      );
      $element['#attached']['metatag_set_preprocess_variable'][] = array(
        'html',
        'head_array',
        array('title' => $value),
      );
    }
    return $element;
  }

}

/**
 * Multiple value meta tag controller.
 */
class DrupalListMetaTag extends DrupalDefaultMetaTag {

  /**
   * {@inheritdoc}
   */
  function __construct(array $info, array $data = NULL) {
    // Ensure that the $data['value] argument is an array.
    if (empty($data['value'])) {
      $data['value'] = array();
    }
    $data['value'] = (array) $data['value'];

    parent::__construct($info, $data);
  }

  /**
   * {@inheritdoc}
   */
  public function getForm(array $options = array()) {
    $form['value'] = isset($this->info['form']) ? $this->info['form'] : array();

    $form['value'] += array(
      '#type' => 'checkboxes',
      '#title' => $this->info['label'],
      '#description' => !empty($this->info['description']) ? $this->info['description'] : '',
      '#default_value' => isset($this->data['value']) ? $this->data['value'] : array(),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function getValue(array $options = array()) {
    $values = array_keys(array_filter($this->data['value']));
    sort($values);
    $value = implode(', ', $values);
    $value = $this->tidyValue($value);

    // Translate the final output string prior to output. Use the
    // 'output' i18n_string object type, and pass along the meta tag's
    // options as the context so it can be handled appropriately.
    $value = metatag_translate_metatag($value, $this->info['name'], $options, NULL, TRUE);

    return $value;
  }

}

/**
 * Date interval meta tag controller.
 */
class DrupalDateIntervalMetaTag extends DrupalDefaultMetaTag {

  /**
   * {@inheritdoc}
   */
  public function getForm(array $options = array()) {
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => t('!title interval', array('!title' => $this->info['label'])),
      '#default_value' => isset($this->data['value']) ? $this->data['value'] : '',
      '#element_validate' => array('element_validate_integer_positive'),
      '#maxlength' => 4,
      '#description' => isset($this->info['description']) ? $this->info['description'] : '',
    );
    $form['period'] = array(
      '#type' => 'select',
      '#title' => t('!title interval type', array('!title' => $this->info['label'])),
      '#default_value' => isset($this->data['period']) ? $this->data['period'] : '',
      '#options' => array(
        '' => t('- none -'),
        'day' => t('Day(s)'),
        'week' => t('Week(s)'),
        'month' => t('Month(s)'),
        'year' => t('Year(s)'),
      ),
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function getValue(array $options = array()) {
    $value = '';
    if (!empty($this->data['value'])) {
      $interval = intval($this->data['value']);
      if (!empty($interval) && !empty($this->data['period'])) {
        $period = $this->data['period'];
        $value = format_plural($interval, '@count ' . $period, '@count ' . $period . 's');
      }
    }

    // Translate the final output string prior to output. Use the 'output'
    // i18n_string object type, and pass along the meta tag's options as the
    // context so it can be handled appropriately.
    $value = metatag_translate_metatag($value, $this->info['name'], $options, NULL, TRUE);

    return $value;
  }

}
