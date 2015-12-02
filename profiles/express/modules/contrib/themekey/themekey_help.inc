<?php

/**
 * @file
 * Provides content for help pages.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 */


/**
 * Uses D rupal's form builder to format ThemeKey's help tutorials
 *
 * @see themekey_help()
 */
function themekey_help_tutorials_form($form, &$form_state, $collapsed) {
  $form['themekey_help_tutorials'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tutorials'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['themekey_help_tutorials']['Rule Chaining: Using a special theme for content creation depending on user roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Rule Chaining: Using a special theme for content creation depending on user roles'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_tutorials']['Rule Chaining: Using a special theme for content creation depending on user roles']['author'] = array(
    '#type' => 'item',
    '#title' => t('Author'),
    '#markup' => l(t('!path', array('!path' => 'mkalkbrenner')), 'http://drupal.org/user/124705'),
  );

  $img_path = base_path() . drupal_get_path('module', 'themekey') . '/img/tutorials/';

  $form['themekey_help_tutorials']['Rule Chaining: Using a special theme for content creation depending on user roles']['item'] = array(
    '#type' => 'item',
    '#markup' => '<p>' . t('Note: This tutorial uses some ThemeKey properties from an additional module called !themekey_properties_link.', array('!themekey_properties_link' => l(t('!path', array('!path' => 'ThemeKey Properties')), 'http://drupal.org/project/themekey_properties'))) . '</p>' .
'<p>' . t("In this tutorial you'll learn how to define ThemeKey rules and how to cascade them to achieve sophisticated rule chains.") . '</p>' .
'<p>' . t("The use case is to use a different theme during content creation for premium users, e.g., you don't want to show advertisements or you want to show some advanced help blocks ...") . '</p>' .
'<p>' . t('Therefore, you created two user roles called "premium user" and "standard user". Using ThemeKey it\'s easy to create a rule that switches the theme if a user\'s role is "premium user":') . '<br />' .
'<img src="' . $img_path . 'premium_user.png" alt="' . t('ThemeKey rule that switches the theme if the user\'s role is "premium user"') . '" /></p>' .
'<p>' . t('Similarly, we can create a rule that switches the theme if a content creation form is requested. To do this for any kind of content we use ThemeKey\'s drupal:path property and its wildcard feature:') . '<br />' .
'<img src="' . $img_path . 'create_content.png" alt="' . t('ThemeKey rule that switches the theme on content creation forms') . '" /></p>' .
'<p>' . t('But having these two separate rules is not what our use case specified. To implement our use case, we have to cascade or "chain" both rules. By clicking and dragging the cross icon in front of a rule, you can move any rule up and down, or indent it. The chain we need should like this:') . '<br />' .
'<img src="' . $img_path . 'create_content_premium_user.png" alt="' . t('ThemeKey rule chain that switches the theme on content creation forms for premium users') . '" /></p>' .
'<p>' . t('Now using this chain, ThemeKey only switches the theme on content creation forms if the current user\'s role is "premium user". Non-premium users will use the current system standard theme.') . '</p>' .
'<p>' . t('You might have noticed that the theme select box for the first rule disappeared as you indented the second one. That\'s because both separate rules became one.') . '</p>' .
'<p>' . t('Now it\'s possible to extend our chain to use another dedicated theme on content creation forms for "standard users".') . '<br />' .
'<img src="' . $img_path . 'create_content_premium_user_standard_user.png" alt="' . t('ThemeKey rule chain that switches to different themes on content creation forms for different user roles') . '" /></p>' .
'<p>' . t('If you have more than these two roles in your system and you simply want to use one theme for premium users and another for non-premium users on content creation pages, you don\'t need to add one rule per role, as decribed above. In this case, it\'s easier to change the latest rule we added, and say any other rule than "premium user", using ThemeKey\'s not operator "!".') . '<br />' .
'<img src="' . $img_path . 'create_content_premium_user_any_role.png" alt="' . t('Another ThemeKey rule chain that switches to different themes on content creation forms for different user roles') . '" /></p>' .
'<p>' . t('Now we have implemented our use case using chained ThemeKey rules, and you can add more rules to ThemeKey\'s Rule Chain to implement different use cases. The only decision you have to make is the order the rules are checked by ThemeKey on every page request. To demonstrate this, let\'s add another rule that switches the theme if the user uses an iPhone to access your page.') . '<br />' .
'<img src="' . $img_path . 'iphone_create_content_premium_user_any_role.png" alt="' . t('ThemeKey rule chain selecting special theme for iPhones') . '" /></p>' .
'<p>' . t('What happens now is that ThemeKey switches to a special theme for iPhones whenever the user accesses your page using such a device, because ThemeKey stops processing the rules if a rule, or one set of chained rules, matches. This means that content creation forms are shown using the iPhone theme, regardless of the role assigned to the current user.') . '</p>' .
'<p>' . t('I think that\'s a good choice. If you move the iPhone rule below the chain for the content cration form these forms will use the configured themes even on the iPhone, which might not be suitable for displaying them.') . '</p>' .
'<p>' . t('If you\'d like to treat node creation forms differently, even on the iPhone, you should "chain" a dedicated set of rules for that.') . '<br />' .
'<img src="' . $img_path . 'iphone_create_content_pages.png" alt="' . t('Sophisticated ThemeKey rule chain') . '" /></p>' .
'<p>' . t('Have Fun!') . '</p>',
  );

  $form['themekey_help_tutorials']['Rule Chaining: Using a special theme for content creation depending on user roles']['versions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Versions used to create this tutorial'),
    '#collapsible' => FALSE,
  );

  $form['themekey_help_tutorials']['Rule Chaining: Using a special theme for content creation depending on user roles']['versions']['themekey'] = array(
    '#type' => 'item',
    '#title' => t('ThemeKey'),
    '#markup' => '6.x-2.0',
  );

  $form['themekey_help_tutorials']['Rule Chaining: Using a special theme for content creation depending on user roles']['versions']['themekey_ui'] = array(
    '#type' => 'item',
    '#title' => t('ThemeKey Properties'),
    '#markup' => '6.x-2.0',
  );

  return $form;
}


/**
 * Uses Drupal's form builder to format ThemeKey's help examples
 *
 * @see themekey_help()
 */
function themekey_help_examples_form($form, &$form_state, $collapsed) {

  $form['themekey_help_examples'] = array(
    '#type' => 'fieldset',
    '#title' => t('Examples'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['themekey_help_examples']['Set a special theme for site administrator'] = array(
    '#type' => 'fieldset',
    '#title' => t('Set a special theme for site administrator'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_examples']['Set a special theme for site administrator']['item'] = array(
    '#type' => 'item',
    '#markup' => t('Create a Theme Switching Rules:') . '<pre>
Property: user:uid
Operator: =
Value: 1
</pre>',
  );

  // TODO: add some examples using drupal:path and drupal:path:wildcard
  $form['themekey_help_examples']['TODO: add some examples using drupal:path and drupal:path:wildcard'] = array(
    '#type' => 'fieldset',
    '#title' => 'TODO: add some examples using drupal:path and drupal:path:wildcard',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_examples']['TODO: add some examples using drupal:path and drupal:path:wildcard']['item'] = array(
    '#type' => 'item',
    '#markup' => 'TODO' . '<pre>
</pre>',
  );

    $form['themekey_help_examples']['Select a theme for Firefox 3.0.x, but not Firefox 3.5.x'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select a theme for Firefox 3.0.x, but not Firefox 3.5.x'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_examples']['Select a theme for Firefox 3.0.x, but not Firefox 3.5.x']['item'] = array(
    '#type' => 'item',
    '#markup' => t('Cascade following Theme Switching Rules:') . '<pre>
Property: system:user_browser_simplified
Operator: =
Value: Mozilla Firefox

  Property: system:user_browser
  Operator: >
  Value: Mozilla Firefox 3.0

    Property: system:user_browser
    Operator: <
    Value: Mozilla Firefox 3.5
</pre>',
  );

  $form['themekey_help_examples']['Select a theme for IE 6'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select a theme for IE 6'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_examples']['Select a theme for IE 6']['item'] = array(
    '#type' => 'item',
    '#markup' => t('Cascade following Theme Switching Rules:') . '<pre>
Property: system:user_browser_simplified
Operator: =
Value: Internet Explorer

  Property: system:user_browser
  Operator: >
  Value: Internet Explorer 6

    Property: system:user_browser
    Operator: <
    Value: Internet Explorer 7
</pre>',
  );

  $form['themekey_help_examples']['Select a theme for Christmas 2009'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select a theme for Christmas 2009'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_examples']['Select a theme for Christmas 2009']['item'] = array(
    '#type' => 'item',
    '#markup' => t('Cascade following Theme Switching Rules:') . '<pre>
Property: system:date
Operator: >
Value: 2009-12-24

  Property: system:date
  Operator: <
  Value: 2009-12-27
</pre>',
  );

  $form['themekey_help_examples']['Select a theme for New Year 2010'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select a theme for New Year 2010'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_examples']['Select a theme for New Year 2010']['item'] = array(
    '#type' => 'item',
    '#markup' => t('Create a Theme Switching Rule:') . '<pre>
Property: system:date
Operator: =
Value: 2010-01-01
</pre>',
  );

  $form['themekey_help_examples']['Select a theme dedicated for your start page (front page, index page, ...)'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select a theme dedicated for use on your start page (front page, index page, ...)'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_examples']['Select a theme dedicated for your start page (front page, index page, ...)']['item'] = array(
    '#type' => 'item',
    '#markup' => t('Create a Theme Switching Rule:') . '<pre>
Property: drupal:is_front_page
Operator: =
Value: true
</pre>',
  );

  return $form;
}


/**
 * Uses Drupal's form builder to format ThemeKey's help properties
 *
 * @see themekey_help()
 */
function themekey_help_properties_form($form, &$form_state, $collapsed) {
  module_load_include('inc', 'themekey', 'themekey_admin');

  $page_cache_support_desriptions = themekey_get_page_cache_support_desriptions();

  $attributes = variable_get('themekey_attributes', array());

  $form['themekey_help_properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Properties explained'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  foreach ($attributes as $property => $attribute) {
    $form['themekey_help_properties'][$property] = array(
      '#type' => 'fieldset',
      '#title' => check_plain($property),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['themekey_help_properties'][$property]['item'] = array(
      '#type' => 'item',
      '#title' => t('Description'),
      '#markup' => '<div id="themekey-value-help-' . str_replace(array(':', '_'), array('-', '-'), $property) . '" style="display: none">' . htmlentities(strip_tags($attribute['description'])) . '</div>' . $attribute['description'],
    );

    $form['themekey_help_properties'][$property]['page_cache'] = array(
      '#type' => 'item',
      '#title' => t('Page Cache'),
      '#markup' => '<div id="themekey-page-cache-' . str_replace(array(':', '_'), array('-', '-'), $property) . '" style="display: none">' . theme('themekey_page_cache_icon', array('page_cache_support' => $attribute['page cache'])) . '</div>' . $page_cache_support_desriptions[$attribute['page cache']],
    );
  }

  return $form;
}


/**
 * Uses Drupal's form builder to format ThemeKey's help operators
 *
 * @see themekey_help()
 */
function themekey_help_operators_form($form, &$form_state, $collapsed) {
  $form['themekey_help_operators'] = array(
    '#type' => 'fieldset',
    '#title' => t('Operators explained'),
    '#collapsible' => TRUE,
    '#collapsed' => $collapsed,
  );

  $form['themekey_help_operators']['='] = array(
    '#type' => 'item',
    '#markup' => t('<strong>=</strong><br />equals (exact value of a property, consider ThemeKey Debug to get an impression of the possible values)'),
  );

  $form['themekey_help_operators']['!'] = array(
    '#type' => 'item',
    '#markup' => t('<strong>!</strong><br />not equals'),
  );

  $form['themekey_help_operators']['<'] = array(
    '#type' => 'item',
    '#markup' => t('<strong>&lt;</strong><br />less than (alphanumeric values are treated in alphabetical order: "A" is less than "B" but "B" is less than "a")'),
  );

  $form['themekey_help_operators']['<='] = array(
    '#type' => 'item',
    '#markup' => t('<strong>&lt;=</strong><br />less than or equal to (alphanumeric values are treated in alphabetical order: "a" is less than "b" but "B" is less than "a" and "A" is less than "a")'),
  );

  $form['themekey_help_operators']['>'] = array(
    '#type' => 'item',
    '#markup' => t('<strong>&gt;</strong><br />greater (alphanumeric values are treated in alphabetical order: "b" is greater than "a" but "a" is greater than "B")'),
  );

  $form['themekey_help_operators']['=>'] = array(
    '#type' => 'item',
    '#markup' => t('<strong>=&gt;</strong><br />greater than or equal (alphanumeric values are treated in alphabetical order: "b" is greater than "a" but "a" is greater than "B" and "a" is greater than "A")'),
  );

  $form['themekey_help_operators']['~'] = array(
    '#type' => 'item',
    '#markup' => t('<strong>~</strong><br />regular expression, including delimiters and modifiers (see !link)', array('!link' => l(t('PHP Manual'), 'http://php.net/manual/en/pcre.pattern.php'))),
  );

  return $form;
}
