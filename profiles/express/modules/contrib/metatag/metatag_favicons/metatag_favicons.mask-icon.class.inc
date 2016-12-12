<?php
/**
 * @file
 * Custom class for the mask-icon meta tag's custom data.
 */

/**
 * Mask icon meta tag controller.
 */
class DrupalMaskIconMetaTag extends DrupalTextMetaTag {

  /**
   * {@inheritdoc}
   */
  public function getForm(array $options = array()) {
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => $this->info['label'],
      '#default_value' => isset($this->data['value']) ? $this->data['value'] : '',
      '#description' => isset($this->info['description']) ? $this->info['description'] : '',
      '#maxlength' => 1024,
      '#weight' => $this->getWeight(),
    );
    $form['color'] = array(
      '#type' => 'textfield',
      '#title' => t('Icon: SVG color'),
      '#default_value' => isset($this->data['color']) ? $this->data['color'] : '',
      '#description' => t('Provides a color for the SVG icon. Per <a href="@specs_url">Apple\'s specifications</a>, it may be a hexadecimal value (e.g. "#990000"), an RGB value (e.g. "rgb(153, 0, 0)"), or a recognized color-keyword (e.g. "red", "lime", "navy", etc). Will only be output if the SVG icon meta tag has a value.', array('@specs_url' => 'https://developer.apple.com/library/mac/releasenotes/General/WhatsNewInSafari/Articles/Safari_9_0.html#//apple_ref/doc/uid/TP40014305-CH9-SW20')),
      '#maxlength' => 10,
      '#weight' => $form['value']['#weight'] + 0.01,
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function getValue(array $options = array()) {
    $value = array(
      'value' => '',
      'color' => '',
    );

    // The 'color' attribute will only be output if the 'value' is present.
    if (!empty($this->data['value'])) {
      // Pass the 'value' through the parent logic.
      $value['value'] = parent::getValue($options);

      // Get the optional 'color' attribute.
      if (!empty($this->data['color'])) {
        $value['color'] = check_plain($this->tidyValue($this->data['color']));
      }
    }

    return $value;
  }

  /**
   * {@inheritdoc}
   */
  public function getElement(array $options = array()) {
    $value = $this->getValue($options);

    // This meta tag has two separate values.
    $color = $value['color'];
    $value = $value['value'];

    // Don't bother proceeding if the 'value' is empty.
    if (strlen($value) === 0) {
      return array();
    }

    // The stack of elements that will be output.
    $elements = array();

    // Dynamically add each option to this setting.
    $base_element = isset($this->info['element']) ? $this->info['element'] : array();

    // Combine the base configuration for this meta tag with the value.
    $element = $base_element + array(
      '#theme' => 'metatag',
      '#tag' => 'link',
      '#id' => 'metatag_' . $this->info['name'] . '_' . 1,
      '#rel' => $this->info['name'],
      '#name' => $this->info['name'],
      '#value' => $value,
      '#color' => $color,
      '#weight' => $this->getWeight(),
    );

    // Add header information if desired.
    if (!empty($this->info['header'])) {
      $element['#attached']['drupal_add_http_header'][] = array($this->info['header'], $value);
    }

    $elements[] = array($element, $element['#id']);

    return array(
      '#attached' => array('drupal_add_html_head' => $elements),
    );
  }

}
