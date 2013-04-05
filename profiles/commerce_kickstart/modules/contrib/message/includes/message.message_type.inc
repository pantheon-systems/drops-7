<?php

/**
 * @file
 * A class used for message types.
 */

class MessageType extends Entity {

  /**
   * The machine name of the message type.
   *
   * @var string
   */
  public $name;

  /**
   * The human-redable name of the message type.
   *
   * @var string
   */
  public $description = '';

  /**
   * Array with the argument keys.
   *
   * This is used to allow creation of entity metadata properties based
   * on the argument keys.
   *
   * @see MessageMetadataController::entityPropertyInfo()
   *
   * @var array
   */
  public $argument_keys = array();

  /**
   * Array with the arguments and their replacement value, or callacbks.
   *
   * The argument keys will be replaced when rendering the message, and it
   * sohuld be prefixed by @, %, ! - similar to way it's done in Drupal
   * core's t() function.
   *
   * @code
   *
   * // Assuming out message-text is:
   * // %user-name created <a href="@node-url">@node-title</a>
   *
   * $message_type->arguments = array(
   *   // Hard code the argument.
   *   '%user-name' => 'foo',
   *
   *   // Use a callback, and provide callbacks arguments.
   *   // The following example will call Drupal core's url() function to
   *   // get the most up-to-date path of node ID 1.
   *   '@node-url' => array(
   *      'callback' => 'url',
   *      'callback arguments' => array('node/1'),
   *    ),
   *
   *   // Use callback, but instead of passing callback argument, we will
   *   // pass the Message entity itself.
   *   '@node-title' => array(
   *      'callback' => 'example_bar',
   *      'pass message' => TRUE,
   *    ),
   * );
   * @endcode
   *
   * Arguments assigned to message-type can be overriden by the ones
   * assigned to the message.
   *
   * @see message_get_property_values()
   *
   * @var array
   */
  public $argument = array();

  /**
   * Set the default message type category of the message type.
   *
   * @var string
   */
  public $category = 'message_type';

  /**
   * Serialized array with misc options.
   *
   * Purge settings (under $message_type->data['purge]). Note that the
   * purge settings can be added only to the message-type.
   * - 'enabled': TRUE or FALSE to explicetly enable or disable message
   *    purging. IF not set, the default purge settings defined in the
   *    "Message settings" will apply.
   * - 'quota': Optional; Maximal (approximate) amount of allowed messages
   *    of the message type. IF not set, the default purge settings defined in the
   *    "Message settings" will apply.
   * - 'days': Optional; Maximal message age in days. IF not set, the default purge settings defined in the
   *    "Message settings" will apply.
   *
   * Token settings:
   * - 'token replace': Indicate if message's text should be passed
   *    through token_replace(). defaults to TRUE.
   * - 'token options': Array with options to be passed to
   *    token_replace().
   *
   * Tokens settings assigned to message-type can be overriden by the ones
   * assigned to the message.
   *
   * @see message_get_property_values()
   *
   * @var array
   */
  public $data = array();

  public function __construct($values = array()) {
    parent::__construct($values, 'message_type');
  }

  /**
   * Retrieves the configured message text in a certain language.
   *
   * @param $langcode
   *   The language code of the Message text field, the text should be
   *   extracted from.
   * @param $options
   *   Array of options to pass to the metadata-wrapper:
   *   - 'field name': The name of the Message text field, text should be
   *     extracted from.
   *   - 'sanitize': Indicate if text should be escaped.
   *
   * @return
   *   A string with the text from the field, with all the tokens
   *   converted into their actual value.
   */
  public function getText($langcode = LANGUAGE_NONE, $options = array()) {
    // Set default values.
    $options += array(
      // As the text is already sanitized, it does not really matter if we
      // enable sanitizing, as it would be default. However, one can change the
      // field instance (e.g. to have no text processing) we make sure we still
      // properly sanitize the value.
      'sanitize' => TRUE,
      // The field name from which the text should be extracted.
      'field name' => MESSAGE_FIELD_MESSAGE_TEXT,
      // Determine if the text of just a single delta should be returned.
      'partials' => FALSE,
      'partial delta' => 0,
    );

    $field_name = $options['field name'];
    $params = array('%field' => $field_name);
    if (!$field = field_info_field($field_name)) {
      throw new MessageException(format_string('Field %field does not exist.', $params));
    }

    if (empty($field['settings']['message_text'])) {
      throw new MessageException(format_string('Field %field is not a message-text.', $params));
    }

    $property = entity_metadata_wrapper('message_type', $this);

    if (empty($langcode) && module_exists('locale')) {
      // Get the langcode from the current language.
      global $language;
      $langcode = $language->language;
    }

    // Let the metadata wrapper deal with the language.
    $property = $property->language($langcode)->$options['field name'];

    $delta = $options['partial delta'];
    $count = $field['cardinality'] == 1 ? 1 : $property->count();

    if (!empty($options['partials']) && $delta >= $count) {
      // Delta is bigger than the existing field, so return early, to
      // prevent an error.
      return;
    }

    if (!empty($options['partials'])) {
      // Get partial, not the whole text.
      $property_item = $this->getValue($property, $delta, $options);
      return $property_item->value($options);
    }
    elseif ($property instanceof EntityListWrapper) {
      // Multiple value field.
      $output = '';
      foreach (array_keys($property->value($options)) as $delta) {
        $property_item = $this->getValue($property, $delta, $options);
        $output .= $property_item->value($options);
      }
      return $output;
    }
    else {
      // Single value field.
      $property_item = $this->getValue($property, $delta, $options);
      return $property_item->value($options);
    }
  }

  public function save() {
    parent::save();
    // Clear the entity property info cache, as changed argument-keys result
    // in different entity property info.
    entity_property_info_cache_clear();

    // Clear the field info cache, so extra fields info is purged.
    _field_info_collate_fields(TRUE);
  }

  /**
   * Helper function to get the value from a property.
   *
   * If the property is of type 'text_formatted' get the processed text
   * value.
   *
   * @param $property
   *   The wrapped property object.
   * @param $delta
   *   The delta of the field.
   * @param $options
   *   Array of options that might be needed to get the field value.
   *
   * @return
   *   The wrapped property that can be used to get the text value of the
   *   field (i.e. safe-value or plain text).
   */
  protected function getValue($property, $delta, $options) {
    if ($property instanceof EntityStructureWrapper && isset($property->value) && $property->value($options)) {
      // Single value field.
      $property = $property->value;
    }
    elseif ($property instanceof EntityListWrapper && $property->get($delta)->value($options) && $property->get($delta) instanceof EntityStructureWrapper && isset($property->get($delta)->value)) {
      // Multiple value field.
      $property = $property->get($delta)->value;
    }
    return $property;
  }
}
