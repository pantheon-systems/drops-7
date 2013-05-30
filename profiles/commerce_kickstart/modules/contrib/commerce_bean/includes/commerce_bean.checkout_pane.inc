<?php
/**
 * @file
 * Commerce checkout pane handler for bean.
 */

/**
 * Checkout pane callback: returns the bean pane's settings form.
 */
function commerce_bean_contents_pane_settings_form($checkout_pane) {
  $form = array();
  $options = array();
  $bean = bean_load_delta($checkout_pane['pane_id']);

  $entity_info = entity_get_info('bean');
  foreach ($entity_info['view modes'] as $name => $info) {
    $options[$name] = $info['label'];
  }

  $form['commerce_bean_view_mode_' . $checkout_pane['pane_id']] = array(
    '#type' => 'select',
    '#title' => t('Select the view mode for the bean'),
    '#options' => $options,
    '#default_value' => variable_get('commerce_bean_view_mode_' . $checkout_pane['pane_id'], 'default'),
  );
  $form['commerce_bean_view_mode_review_' . $checkout_pane['pane_id']] = array(
    '#type' => 'select',
    '#title' => t('Select the view mode for the bean in the review pane'),
    '#options' => $options,
    '#states' => array(
      'visible' => array(
        ':input[name="review"]' => array('checked' => TRUE),
      ),
    ),
    '#default_value' => variable_get('commerce_bean_view_mode_review_' . $checkout_pane['pane_id'], 'default'),
  );

  if (bean_access('edit', $bean)) {
    $form['edit_bean'] = array(
      '#type' => 'markup',
      '#markup' => l(t('Edit the bean'), 'block/' . $checkout_pane['pane_id'] . '/edit'),
    );
  }

  return $form;
}

/**
 * Checkout pane callback: returns the bean for inclusion in the checkout form.
 */
function commerce_bean_contents_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  $pane_form = array();
  if (($bean = bean_load_delta($checkout_pane['pane_id'])) && bean_access('view', $bean)) {
    $view_mode = variable_get('commerce_bean_view_mode_' . $checkout_pane['pane_id'], $bean->view_mode);

    // Add extra properties for best use, order and checkout pane.
    $bean->order = $order;
    $bean->checkout_pane = $checkout_pane;
    $content = $bean->view($view_mode, $order);

    $pane_form[$checkout_pane['pane_id']] = array(
      '#markup' => drupal_render($content),
      '#attached' => _commerce_bean_get_element_properties($content, '#attached'),
    );
  }

  return $pane_form;
}

/**
 * Helper function to get a given property out of the build array.
 */
function _commerce_bean_get_element_properties($content, $property) {
  $result = isset($content[$property]) ? $content[$property] : array();
  foreach($content as $value) {
    if (is_array($value)) {
      $result = array_merge(_commerce_bean_get_element_properties($value, $property), $result);
    }
  }
  return $result;
}

/**
 * Checkout pane callback: returns the bean contents in the checkout pane.
 */
function commerce_bean_contents_pane_review($form, $form_state, $checkout_pane, $order) {
  if (($bean = bean_load_delta($checkout_pane['pane_id'])) && bean_access('view', $bean)) {
    $view_mode = variable_get('commerce_bean_view_mode_review_' . $checkout_pane['pane_id'], $bean->view_mode);

    // Add extra properties for best use, order and checkout pane.
    $bean->order = $order;
    $bean->checkout_pane = $checkout_pane;

    $content = $bean->view($view_mode);
    return drupal_render($content);
  }
}
