<?php

/**
 * @file
 * Page callbacks for the Marketplace module.
 */

/**
 * Page callback: Returns the marketplace frontpage.
 */
function commerceguys_marketplace_frontpage() {
  global $base_url;
  $profile_token = drupal_get_profile();
  $commerceguys_marketplace_tokens = array(
    'utm_source' => $profile_token,
    'utm_medium' => 'connector',
    'utm_campaign' => 'Connector+Marketplace+Links',
  );

  // Initialize the managers.
  $addon_manager = commerceguys_marketplace_get_manager('addon');
  $training_manager = commerceguys_marketplace_get_manager('training');
  // Get the data to be shown.
  $promotions = $addon_manager->getPromotions();
  $addons = $addon_manager->query('', TRUE);
  $categories = $addon_manager->getCategories();
  $trainings = $training_manager->query();

  // Hardcode the header menu.
  $commerceguys_marketplace_menu = array(
    l('Platform', COMMERCEGUYS_MARKETPLACE_URL . '/platform', array('query' => $commerceguys_marketplace_tokens, 'attributes' => array('target' => '_blank'))),
    l('Add-ons', COMMERCEGUYS_MARKETPLACE_URL . '/add-ons', array('query' => $commerceguys_marketplace_tokens, 'attributes' => array('target' => '_blank'))),
    l('Support', COMMERCEGUYS_MARKETPLACE_URL . '/support', array('query' => $commerceguys_marketplace_tokens, 'attributes' => array('target' => '_blank'))),
    l('Training', COMMERCEGUYS_MARKETPLACE_URL . '/training', array('query' => $commerceguys_marketplace_tokens, 'attributes' => array('target' => '_blank'))),
  );
  // Prepare a list of linked categories for the sidebar.
  $category_list = array();
  foreach ($categories as $category) {
    $category_url = COMMERCEGUYS_MARKETPLACE_URL . '/add-ons?f[0]=field_category%3A' . $category->id . '&utm_source=' . $profile_token . '&utm_medium=connector&utm_campaign=Connector+Marketplace+Links';
    $category_list[] = l($category->name, $category_url, array('attributes' => array('target' => '_blank')));
  }
  // Remove trainings that have no events.
  foreach ($trainings['results'] as $index => $training) {
    if (empty($training->events)) {
      unset($trainings['results'][$index]);
    }
  }

  $page = array(
    '#theme_wrappers' => array('container'),
    '#attributes' => array('class' => array('marketplace')),
  );
  $page['header'] = array(
    '#theme_wrappers' => array('container'),
    '#attributes' => array('class' => array('marketplace-header')),
    'logo' => array(
      '#theme' => 'image',
      '#path' => drupal_get_path('module', 'commerceguys_marketplace') . '/logo.png',
      '#alt' => 'Commerce Marketplace',
      '#prefix' => '<div class="logo">',
      '#suffix' => '</div>',
    ),
    'menu' => array(
      '#theme' => 'item_list',
      '#items' => $commerceguys_marketplace_menu,
    ),
  );

  $header_message = '';
  // No client found. Show a message to the user.
  $client = commerceguys_marketplace_get_client();
  if (!$client) {
    $options = array(
      'absolute' => TRUE,
      'query' => array(
        'site_name' => variable_get('site_name', 'Drupal'),
        'site_url' => $base_url,
        'utm_source' => $profile_token,
        'utm_medium' => 'connector',
        'utm_campaign' => 'Connector+Sign+Up',
      ),
    );
    $link = url(COMMERCEGUYS_MARKETPLACE_URL . '/marketplace/connect', $options);
    $header_message = '<div class="status-container not-registered clearfix"><a href="' . $link . '">Sign Up</a><div class="message">' . t('Your site is not registered on Commerce Marketplace.') . '</div></div>';
  }
  elseif (!empty($_GET['connected'])) {
    // The connect process has just been completed. Notify the user.
    $header_message = '<div class="status-container success clearfix"><div class="message">' . t('Your site has been successfully configured.') . '</div></div>';
  }

  $page['content'] = array(
    '#theme_wrappers' => array('container'),
    '#attributes' => array('class' => array('marketplace-content')),
    'status' => array(
      '#theme_wrappers' => array('container'),
      '#attributes' => array('class' => array('marketplace-status', 'clearfix')),
      '#markup' => $header_message,
    ),
    'promotions' => array(
      '#theme_wrappers' => array('container'),
      '#attributes' => array('class' => array('marketplace-promotions', 'clearfix')),
      '#markup' => '',
    ),
    'sidebar' => array(
      '#theme_wrappers' => array('container'),
      '#attributes' => array('class' => array('marketplace-sidebar')),
      '#markup' => '<div class="item-title">' . t('Browse categories') . '</div>' . theme('item_list', array('items' => $category_list)),
    ),
    'addons' => array(
      '#theme_wrappers' => array('container'),
      '#theme' => 'commerceguys_marketplace_addons',
      '#addons' => $addons['results'],
      '#mode' => 'featured',
      '#attributes' => array('class' => array('marketplace-addons-wrapper')),
    ),
  );
  $page['trainings'] = array(
    '#theme_wrappers' => array('container'),
    '#attributes' => array('class' => array('marketplace-trainings')),
    '#theme' => 'commerceguys_marketplace_trainings',
    '#trainings' => $trainings['results'],
  );

  $page['content']['promotions']['promotion']['#markup'] = '<ul id="commerceguys-marketplace-slideshow">';
  foreach ($promotions as $variables) {
    $promotion_image_url = $variables->image;
    $promotion_token = urlencode($variables->addon->title);
    $promotion_link = $variables->addon->url . '?utm_source=' . $profile_token . '&utm_medium=connector&utm_campaign=' . $promotion_token;
    $promotion_image = '<a href="' .$promotion_link . '" target="_blank"><img src="' . $promotion_image_url . '"></a>';
    $page['content']['promotions']['promotion']['#markup'] .= '<li>' . $promotion_image . '</li>';
  }
  $page['content']['promotions']['promotion']['#markup'] .= '</ul>';
  return $page;
}

/**
 * Page callback: Returns a paginated, facetted list of addons.
 */
function commerceguys_marketplace_addons_page() {
  if (commerceguys_marketplace_get_client()) {
    return drupal_get_form('commerceguys_marketplace_listing_form', 'addon');
  }
  else {
    return array(
      '#markup' => '<div class="empty-message marketplace">' . t('Your site is not registered on Commerce Marketplace.') . '</div>',
    );
  }
}

/**
 * Form callback: Provides a facetted list of items.
 *
 * @param $type
 *   The type of items to show (addon, training, etc).
 */
function commerceguys_marketplace_listing_form($form, &$form_state, $type) {
  $addon_manager = commerceguys_marketplace_get_manager('addon');
  $categories = $addon_manager->getCategories();
  $category_list = array();
  foreach ($categories as $category) {
    $category_list[$category->id] = $category->name;
  }

  $form['#tree'] = TRUE;
  $defaults = !empty($form_state['values']) ? $form_state['values']['filters'] : array();
  $category = !empty($defaults['category']) ? $defaults['category'] : NULL;

  $results = $addon_manager->getLicenses($category);
  if ($results['result_count']) {
    $form['filters']['category'] = array(
      '#type' => 'select',
      '#title' => t('Category'),
      '#title_display' => 'invisible',
      '#options' => array('' => 'Category') + $category_list,
      '#default_value' => $category,
      '#prefix' => '<h1>Your Marketplace Add-ons</h1><div class="filter">',
    );
    $form['filters']['submit'] = array(
      '#type' => 'submit',
      '#value' => 'Filter',
      '#suffix' => '</div>',
    );
    $form['results'] = array(
      '#markup' => theme('commerceguys_marketplace_addon_licenses', array('addons' => $results['results'])),
    );
  }
  else {
    $form['results'] = array(
      '#markup' => '<div class="empty-message">' . t('You don\'t have any Marketplace installed add-ons yet. ') . l('Visit the Marketplace', COMMERCEGUYS_MARKETPLACE_URL . '/add-ons', array('attributes' => array('target' => '_blank'))) . t(' to find everything you need to improve and strengthen your online store.') . '</div>',
    );
  }
  return $form;
}

/**
 * Submit callback for commerceguys_marketplace_listing_form.
 */
function commerceguys_marketplace_listing_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

/**
 * Page callback: Stores the provided api key and redirects to the frontpage.
 */
function commerceguys_marketplace_connect_page() {
  if (empty($_GET['api_key'])) {
    return drupal_not_found();
  }

  $api_key = $_GET['api_key'];
  variable_set('commerceguys_marketplace_api_key', $api_key);
  drupal_goto('commerceguys-marketplace', array('query' => array('connected' => 1)));
}

/**
 * Form callback: Provides a Marketplace settings form.
 */
function commerceguys_marketplace_settings_form($form, &$form_state) {
  $form['commerceguys_marketplace_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API key'),
    '#description' => t('Used to authenticate the site to the marketplace.'),
    '#default_value' => variable_get('commerceguys_marketplace_api_key', ''),
  );

  return system_settings_form($form);
}
