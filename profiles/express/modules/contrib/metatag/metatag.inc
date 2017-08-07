<?php
/**
 * @file
 * Metatag primary classes.
 */

interface DrupalMetaTagInterface {

  /**
   * Constructor
   *
   * @param array $info
   *   The information about the meta tag from metatag_get_info().
   */
  function __construct(array $info, array $data = array());

  function getForm();

  //function validateForm();

  //function processForm();

  function getValue();

  function getWeight();

  function getElement();

  function tidyValue($value);

  function convertUrlToAbsolute($url);

}

class DrupalDefaultMetaTag implements DrupalMetaTagInterface {

  protected $info;
  protected $data = array('value' => '');
  protected $weight = 0;

  function __construct(array $info, array $data = NULL) {
    $this->info = $info;
    if (isset($data)) {
      $this->data = $data;
    }
  }

  /**
   * Calculate the weight of this meta tag.
   *
   * @return integer
   */
  public function getWeight() {
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

    return $value;
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
   * @param $value string
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
      // empty values.
      $values = array_filter(explode(',', $value));

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

    $value = $this->tidyValue($value);

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
      $element['#attached']['drupal_add_http_header'][] = array('Link', '<' . $value . '>;' . drupal_http_header_attributes(array('rel' => $element['#name'])), TRUE);
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
      $element['#attached']['metatag_set_preprocess_variable'][] = array('html', 'head_title', $value);
      $element['#attached']['metatag_set_preprocess_variable'][] = array('html', 'head_array', array('title' => $value));
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

    // Translate the final output string prior to output. Use the
    // 'output' i18n_string object type, and pass along the meta tag's
    // options as the context so it can be handled appropriately.
    $value = metatag_translate_metatag($value, $this->info['name'], $options, NULL, TRUE);

    return $value;
  }

}
