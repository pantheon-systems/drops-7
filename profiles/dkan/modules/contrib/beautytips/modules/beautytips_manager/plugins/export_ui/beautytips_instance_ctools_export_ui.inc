<?php
/*
 * Plugin definition for Ctools Export UI
 */
$plugin = array(
  // TODO: Add load callback to get default values and to unserialize.
  'schema' => 'beautytips_instances',
  // TODO: Access
  'access' => 'administer content',
  'menu' => array(
    // TODO: Add menu prefix
    'menu item' => 'beautytips',
    'menu description' => 'Manage Beautytips',
    // Add beautytips specific own menu callbacks.
    /*'items' => array(
      'instance' => array(
        'path' => '%ctools_export_ui/instance',
        'title' => 'Custom Beautytip',
        'page callback' => 'ctools_export_ui_switcher_page',
        'page arguments' => array('beautytips_ctools_export_ui', 'instance', 4),
        'load arguments' => array('beautytips_ctools_export_ui'),
        //'access arguments' => array('administer services'),
        'type' => MENU_LOCAL_TASK,
      ),
    ),*/
  ),
  'load callback' => 'beautytips_manager_instance_load', 
  'form' => array(
    'settings' => 'beautytips_manager_ctools_instance_form',
    'validate' => 'beautytips_manager_custom_tips_form_validate',
    'submit' => 'beautytips_manager_custom_tips_form_submit',
  ),
  'handler' => array(
    'class' => 'beautytips_instance_export_ui',
    'parent' => 'ctools_export_ui',
  ),
  'title singular' => t('beautytip'),
  'title plural' => t('beautytips'),
  'title singular proper' => t('Beautytips instance'),
  'title plural proper' => t('Beautytips instance'),
  'data' => array(
    'element' => '',
    'content_type' => '',
    'content' => '',
    'disable_link' => 0,
    'trigger_on' => 0,
    'trigger_off' => 0,
    'style' => 'default',
    'shrink' => FALSE,
    'positions' => '',
    'animation_on' => '',
    'animation_off' => '',
    'visibility' => 0,
    'pages' => '',
  ),
);

/**
 * Form for configuring custom beautytips.
 */
function beautytips_manager_ctools_instance_form(&$form, &$form_state) {
  // TODO: This css should be attached to the form.
  drupal_add_css(drupal_get_path('module', 'beautytips') . '/css/beautytips.css');
  if (!empty($form_state['item']->data)) {
    $tip = (object) array_merge($form_state['object']->plugin['data'], unserialize($form_state['item']->data));
  }
  else {
    $tip = $form_state['object']->plugin['data'];
  }

  $form['tip'] = array(
    '#type' => 'markup',
    '#value' => '',
    '#tree' => TRUE,
  );
  $form['tip']['element'] = array(
    '#type' => 'textfield',
    '#title' => t('Element'),
    '#required' => TRUE,
    '#maxlength' => 255,
    '#default_value' => is_object($tip) ? $tip->element : '',
  );
  $content_options = array('attribute' => 'attribute', 'text' => 'text', 'ajax' => 'ajax');
  $description = t('Content to display in the beautytip.  The data entered here depends on the Content Type.');
  $types = array();
  $types[0] = t('attribute - Enter the attribute of the element that should be displayed. (If empty, the title will be selected.)');
  $types[0] .= '<br />' . t('ex. "alt"');
  $types[1] = t('text - Enter the text that should be displayed with in the beautytip.');
  $types[1] .= '<br />' . t('ex. "This is my beautytip!"');
  $types[2] = t('ajax - This will grab the page from the "href" attribute and display that page.  Enter css selectors to narrow the down the content from that page.');
  $types[2] .= '<br />' . t('ex. "#my-id .my-class"');
  if (user_access('use Javascript for custom beautytip display')) {
    $content_options['js'] = 'js';
    $types[3] = 'js - Directly enter javascript to select the content.';
    $types[3] .= '<br />' . t('ex. "$(this).next(\'.description\').html()"');
  }
  $form['tip']['content_type'] = array(
    '#type' => 'radios',
    '#title' => t('Type of Content'),
    '#description' => t('This helps determine from where to pull the content to be displayed.'),
    '#options' => $content_options,
    '#default_value' => is_object($tip) ? $tip->content_type : 0,
  );  
  $form['tip']['content'] = array(
    '#type' => 'textarea',
    '#title' => t('Content to Display'),
    '#description' => theme('item_list', array('items' => $types)),
    '#default_value' => is_object($tip) ? $tip->content : '',
  );
  $triggers = beautytips_manager_get_triggers();
  $form['tip']['trigger_on'] = array(
    '#type' => 'select',
    '#title' => t('Trigger On'),
    '#description' => t('Not all events are available for all elements. See jQuery <a href="@events">events documentation</a> for details.', array('@events' => 'http://docs.jquery.com/Events')),
    '#options' => $triggers,
    '#default_value' => is_object($tip) ? $tip->trigger_on : 0,
    '#prefix' => '<div class="beautytips-triggers">',
  );
  $form['tip']['trigger_off'] = array(
    '#type' => 'select',
    '#title' => t('Trigger Off'),
    '#options' => $triggers,
    '#suffix' => '</div>',
    '#default_value' => is_object($tip) ? $tip->trigger_off : 0,
  );

  $form['tip']['disable_link'] = array(
    '#type' => 'checkbox',
    '#title' => t('Disable Link Click'),
    '#description' => t('If you have chosen ajax as the type of content, and you would like to prevent the link from working, then check this box.'),
    '#default_value' => is_object($tip) ? $tip->disable_link : 0,
  );

  $styles = beautytips_get_styles();
  foreach ($styles as $key => $style) {
    $style_options[$key] = $key;
  }
  $form['tip']['style'] = array(
    '#type' => 'select',
    '#title' => t('Style'),
    '#options' => $style_options,
    '#default_value' => is_object($tip) ? $tip->style : 'default',
  );
  $form['tip']['shrink'] = array(
    '#type' => 'checkbox',
    '#title' => t('Shrink to Fit'),
    '#description' => t('Shrink the beautytip to the size of the content. This can sometimes help with sizing problems and is good for tips with just one line.'),
    '#default_value' => is_object($tip) ? $tip->shrink : FALSE,
  );

  $positions = is_object($tip) ? explode(',', $tip->positions) : array();
  $form['tip']['positions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Positions'),
    '#description' => t("Optionally enter the order of positions in which you want the tip to appear.  It will use first in order with available space. The last value will be used if others don't have enough space. If no entries, then the tip will be placed in the area with the most space. Only enter an order for those you wish to use"), 
    '#tree' => TRUE,
  );
  $positions_list = array('top', 'bottom', 'left', 'right');
  foreach ($positions_list as $position) {
    $form['tip']['positions'][$position] = array(
      '#type' => 'textfield',
      '#title' => t($position),
      '#default_value' => (array_search($position, $positions) !== FALSE) ? array_search($position, $positions) : '', 
      '#size' => 1,
      '#maxlength' => 1,
      '#prefix' => '<div class="beautytips-positions">',
      '#suffix' => '</div>',
    );
  }
  
  $form['tip']['animation_on'] = array(
    '#type' => 'select',
    '#title' => t('Animation (On)'),
    '#options' => array('' => '<none>', 'fadeIn' => 'fadeIn'),
    '#description' => t("These animations will be applied to the tip when it is turn on or off."),
    '#default_value' => is_object($tip) ? $tip->animation_on : '',
    '#prefix' => '<div class="beautytips-animations">', 
  );
  $form['tip']['animation_off'] = array(
    '#type' => 'select',
    '#title' => t('Animation (Off)'),
    '#options' => array('' => '<none>', 'fadeOut' => 'fadeOut', 'slideOut' => 'slideOut'),
    '#default_value' => is_object($tip) ? $tip->animation_off : '',
    '#suffix' => '</div>',
  );

  $options = array(t('Show on every page except the listed pages.'), t('Show on only the listed pages.'));
  $description = t("Enter one page per line as Drupal paths. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

  $form['tip']['visibility'] = array(
    '#type' => 'radios',
    '#title' => t('Show beautytip on specific pages'),
    '#options' => $options,
    '#default_value' => is_object($tip) ? $tip->visibility : 0,
    '#prefix' => '<div id="edit-tip-visibility-wrapper">',
    '#suffix' => '</div>',
  );
  $form['tip']['pages'] = array(
    '#type' => 'textarea',
    '#title' => t('Pages'),
    '#default_value' => is_object($tip) ? $tip->pages : '',
    '#description' => $description,
  );
}

/**
 * Validation callback for beautytips manager custom tips form.
 */
function beautytips_manager_custom_tips_form_validate($form, &$form_state) {
  $positions = $form_state['values']['tip']['positions'];
  foreach ($positions as $position => $order) {
    if ($order !== '' && !is_numeric($order)) {
      form_set_error("tip][positions][$position", t("You must enter a numeric value for position order (Or leave it blank)."));
    }
  }
}

/**
 * Submission callback for beautytips manager custom tips form.
 */
function beautytips_manager_custom_tips_form_submit($form, &$form_state) {
  $tip = $form_state['values']['tip'];
  $positions = array();
  foreach ($tip['positions'] as $position => $order) {
    if ($order !== '') {
      while (isset($positions[$order])) {
        $order++;
      }
      $positions[$order] = $position;
    }
  }
  ksort($positions);
  $tip['positions'] = (count($positions)) ? implode(',', $positions) : '';
  $form_state['values']['data'] = serialize($tip); 

  cache_clear_all('beautytips:beautytips-ui-custom-tips', 'cache');
  $form_state['redirect'] = 'admin/config/user-interface/beautytips/custom-tips';
}
