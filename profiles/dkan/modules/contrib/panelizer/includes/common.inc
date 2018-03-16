<?php
/**
 * @file
 * Contains common forms and routines that different object types use.
 *
 * The panelizer system has several different places that panels can be
 * edited. While they are all subtly different, they have a lot in common.
 */

/**
 * Form to configure basic panelizer settings.
 */
function panelizer_settings_form($form, &$form_state) {
  $panelizer = $form_state['panelizer'];

  if (!empty($form_state['has title'])) {
    $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Administrative title'),
      '#description' => t('This will appear in the administrative interface to easily identify it.'),
      '#default_value' => $panelizer->title,
    );
  }

  // We only allow this setting on the "full page override" because it
  // does not make sense in other modes.
  if ($form_state['view_mode'] == 'page_manager') {
    $form['no_blocks'] = array(
      '#type' => 'checkbox',
      '#default_value' => $panelizer->no_blocks,
      '#title' => t('Disable Drupal blocks/regions'),
      '#description' => t("Check this to have the display not show the theme's sidebars. Note that some themes support this setting better than others. If in doubt, try with stock themes to see."),
    );
  }

  ctools_include('plugins', 'panels');
  $pipelines = panels_get_renderer_pipelines();

  // If there are no pipelines, that probably means we're operating in
  // legacy mode.
  if (empty($pipelines)) {
    // We retain the original pipeline so we don't wreck things by installing
    // old modules.
    $form['pipeline'] = array(
      '#type' => 'value',
      '#value' => $panelizer->pipeline,
    );
  }
  else {
    $options = array();
    foreach ($pipelines as $name => $pipeline) {
      $options[$name] = check_plain($pipeline->admin_title) . '<div class="description">' . check_plain($pipeline->admin_description) . '</div>';
    }

    $form['pipeline'] = array(
      '#type' => 'radios',
      '#options' => $options,
      '#title' => t('Renderer'),
      '#default_value' => $panelizer->pipeline,
    );
  }

  if ($form_state['view_mode'] != 'page_manager') {
    $form['title_element'] = array(
      '#type' => 'textfield',
      '#default_value' => $panelizer->title_element,
      '#title' => t('Title element'),
      '#description' => t('The HTML element to use for the entity title. Typically this will be an H2.'),
    );

    $form['link_to_entity'] = array(
      '#type' => 'checkbox',
      '#default_value' => $panelizer->link_to_entity,
      '#title' => t('Link title to entity'),
      '#description' => t('If checked the title will be a link to the entity.'),
    );

    $form['css_class'] = array(
      '#type' => 'textfield',
      '#size' => 35,
      '#default_value' => $panelizer->css_class,
      '#title' => t('CSS class'),
      '#description' => t('The CSS class to apply to this display. You may use context substitions here.'),
    );
  }
  else {
    $form['css_class'] = array(
      '#type' => 'textfield',
      '#size' => 35,
      '#default_value' => $panelizer->css_class,
      '#title' => t('Body class'),
      '#description' => t('The CSS class to apply to the BODY tag. You may use context substitions here.'),
    );
  }

  $form['css_id'] = array(
    '#type' => 'textfield',
    '#size' => 35,
    '#default_value' => $panelizer->css_id,
    '#title' => t('CSS ID'),
    '#description' => t('The CSS ID to apply to this display.'),
  );

  $form['css'] = array(
    '#type' => 'textarea',
    '#title' => t('CSS code'),
    '#description' => t('Enter well-formed CSS code here; this code will be embedded into the display and should only be used for minor adjustments; it is usually better to try to put CSS for this display into the theme if possible. This CSS will be filtered for safety so some CSS may not work.'),
    '#default_value' => $panelizer->css,
  );

  panelizer_add_revision_info_form($form, $form_state);

  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit callback
 */
function panelizer_settings_form_submit(&$form, &$form_state) {
  $panelizer = $form_state['panelizer'];
  if ($form_state['view_mode'] == 'page_manager') {
    $panelizer->no_blocks = $form_state['values']['no_blocks'];
  }
  else {
    $panelizer->title_element = $form_state['values']['title_element'];
  }

  $panelizer->css_class = $form_state['values']['css_class'];
  $panelizer->css = $form_state['values']['css'];
  $panelizer->css_id = $form_state['values']['css_id'];
  $panelizer->pipeline = $form_state['values']['pipeline'];
  // Behind an if because this value does not always exist.
  if (isset($form_state['values']['link_to_entity'])) {
    $panelizer->link_to_entity = $form_state['values']['link_to_entity'];
  }
  if (!empty($form_state['has title'])) {
    $panelizer->title = $form_state['values']['title'];
  }
  // NOTE: We do not save in the submit so that the form can be re-used.
}

/**
 * Mini form to add revision settings to a panelizer form so that
 * we can properly handle revisioning.
 */
function panelizer_add_revision_info_form(&$form, &$form_state) {
  if (empty($form_state['entity'])) {
    return;
  }
  if (empty($form_state['revision info'])) {
    return;
  }

  $entity = $form_state['entity'];

  list($use_revisions, $control_revisions, $default_revision) = $form_state['revision info'];

  if ($use_revisions) {
    // @todo -- what if an entity uses some other flag to control revisioning?
    if (!isset($entity->revision)) {
      $entity->revision = $default_revision;
      $entity->log = '';
    }

    $form_state['use revisions'] = TRUE;
    $form['revision_information'] = array(
      '#weight' => 11,
    );

    $form['revision_information']['revision'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create new revision'),
      '#default_value' => !empty($entity->revision),
      '#id' => 'edit-revision',
      '#access' => $control_revisions,
    );

    if ($control_revisions || $entity->revision) {
      $form['revision_information']['log'] = array(
        '#type' => 'textarea',
        '#title' => t('Log message'),
        '#description' => t('Provide an explanation of the changes you are making. This will help other authors understand your motivations.'),
        '#default_value' => $entity->log,
      );

      if ($control_revisions) {
        $form['revision_information']['log']['#dependency'] = array('edit-revision' => array(1));
      }
    }

    // Don't override the existing submit, eh?
    if (!empty($form_state['input']['form_id']) && empty($form['#submit']) && function_exists($form_state['input']['form_id'] . '_submit')) {
      $form['#submit'][] = $form_state['input']['form_id'] . '_submit';
    }

    // The submit handler needs to go in a different location depending on
    // what form this is.
    $submit_handler = 'panelizer_add_revision_info_form_submit';
    if (!empty($form['buttons']['submit']['#submit'])) {
      $form['buttons']['submit']['#submit'][] = $submit_handler;
    }
    elseif (!empty($form['buttons']['return']['#submit'])) {
      $form['buttons']['return']['#submit'][] = $submit_handler;
    }
    else {
      $form['#submit'][] = $submit_handler;
    }
  }
}

/**
 * Form submission callback for panelizer_add_revision_info_form().
 */
function panelizer_add_revision_info_form_submit(&$form, &$form_state) {
  $entity = &$form_state['entity'];
  if (!empty($form_state['use revisions'])) {
    $entity->revision = $form_state['values']['revision'];
    $entity->log = $form_state['values']['log'];
  }
}

/**
 * Form to edit contexts that go with a Panelizer display.
 */
function panelizer_default_context_form($form, &$form_state) {
  ctools_include('context-admin');
  ctools_context_admin_includes();

  $panelizer = &$form_state['panelizer'];

  if (!empty($panelizer->cached)) {
    $form['markup'] = array(
      '#prefix' => '<div class="messages warning">',
      '#markup' => t('This form contains unsaved changes that will not be stored until the Save button is clicked.'),
      '#suffix' => '</div>',
    );
  }

  $form['right'] = array(
    '#prefix' => '<div class="clear-block"><div class="right-container">',
    '#suffix' => '</div>',
  );

  $form['left'] = array(
    '#prefix' => '<div class="left-container">',
    '#suffix' => '</div></div>',
  );

  $module = 'panelizer_context::' . $form_state['panelizer type'];
  ctools_context_add_context_form($module, $form, $form_state, $form['right']['contexts_table'], $panelizer, $form_state['cache key']);
  ctools_context_add_relationship_form($module, $form, $form_state, $form['right']['relationships_table'], $panelizer, $form_state['cache key']);

  // Set an additional description if CCK and Token are enabled, to notify of unlisted keywords
  if (!module_exists('token')) {
    $description = t('More keywords will be available if you install the Token module, see http://drupal.org/project/token.');
  }
  else {
    $description = '';
  }

  $form['left']['summary'] = array(
    '#prefix' => '<div class="page-manager-contexts">',
    '#suffix' => '</div>',
    '#markup' => theme('ctools_context_list', array(
      'object' => $panelizer,
      'header' => t('Summary of contexts'),
      'description' => $description,
    )),
  );

  panelizer_add_revision_info_form($form, $form_state);

  $form['actions'] = array(
    '#type' => 'actions'
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#write' => TRUE,
  );

  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Form used when an entity bundle is panelized but there is no default
 * display. Entities are then individually panelized.
 */
function panelizer_panelize_entity_form($form, &$form_state) {
  $entity_type = $form_state['panelizer']->panelizer_type;
  $entity_info = entity_get_info($entity_type);
  $form = array();

  $form['markup'] = array(
    '#markup' => '<p>' . t('This %entity is not currently panelized.', array('%entity' => $entity_info['label'])) . '</p>',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Panelize it!'),
  );

  return $form;
}

/**
 * Form used when an entity is panelized and is being reset.
 */
function panelizer_reset_entity_form($form, &$form_state) {
  $entity_type = $form_state['panelizer']->entity_type;
  $entity_info = entity_get_info($entity_type);
  $form = array();

  // Build the confirmation form.
  $form = confirm_form(
    $form,
    t('Are you sure you want to reset the panelizer status for this %entity?', array('%entity' => $entity_info['label'])),
    dirname(dirname($_GET['q'])),
    t('This %entity will be reset to the default panelizer state, discarding any modifications made to it. This action cannot be undone.', array('%entity' => $entity_info['label'])),
    t('Reset'),
    t('Cancel'),
    'panelizer_reset_entity_confirm');

  // Append the revision form.
  panelizer_add_revision_info_form($form, $form_state);

  return $form;
}

/**
 * Panelizer layout change form. If there is no content this will be
 * a 'choose' layout form. If there is content it will be a 'change' layout
 * form.
 *
 * @param $form_state
 *   The initial form state array to be used by the wizard. This *must* contain:
 *   - 'display': The display whose layout should be changed.
 *   - 'wizard path': The Drupal path where this wizard lives, so it knows where
 *     to redirect to. Note that the layout chosen will be appended to this
 *     path for the second step, so the page callback for this wizard needs to
 *     make sure to pass that through when teh function is called.
 *
 *   This can also contain:
 *   - 'allowed_layouts' => The key to the allowed layouts array to use.
 *   - 'finish' => The text to use on the save button.
 *
 * @param $step
 *   The wizard step that must be passed through. It should be in the %step
 *   portion of the path.
 *
 * @param $layout
 *   A layout that is chosen in the first step. It is passed through the URL
 *   so that no caching is needed. The caller needs to be sure to extract this
 *   from the URL.
 *
 * @return
 *   While the return value is render array, if $form_state['complete'] is
 *   true, then$form_state['display'] can be saved by the caller and
 *   redirection chosen. If $form_state['cancel'] is true, then the display
 *   should not be saved.
 */
function panelizer_change_layout_wizard(&$form_state, $step = 'choose', $layout = NULL) {
  ctools_include('common', 'panels');

  // Add defaults to the form state sent in.
  $form_state += array(
    'finish' => t('Save'),
    'allowed_layouts' => '',
    'no_redirect' => TRUE,
    'no buttons' => TRUE,
    'layout' => $layout,
  );

  $form_info = array(
    'id' => 'panelizer_change_layout_wizard',
    'finish text' => $form_state['finish'],
    'path' => $form_state['wizard path'],
    'show back' => TRUE,
    'order' => array(
      'choose' => t('Choose layout'),
    ),
    'forms' => array(
      'choose' => array(
        'form id' => 'panelizer_choose_layout_form',
      ),
      'move' => array(
        'form id' => 'panelizer_move_content_form',
      ),
    ),
  );

  if (!empty($form_state['display']->content)) {
    $form_info['order']['move'] = t('Move content');
  }

  ctools_include('common', 'panels');
  ctools_include('display-layout', 'panels');
  ctools_include('plugins', 'panels');
  ctools_include('wizard');
  $output = ctools_wizard_multistep_form($form_info, $step, $form_state);
  if (!empty($form_state['complete'])) {
    $form_state['display']->layout = $form_state['layout'];
  }

  return $output;
}

function panelizer_choose_layout_form($form, &$form_state) {
  // Change the #id of the form so the CSS applies properly.
  $form['#id'] = 'panels-choose-layout';
  $form = panels_choose_layout($form, $form_state);

  if (!empty($form['buttons']['return'])) {
    panelizer_add_revision_info_form($form, $form_state);
  }

  return $form;
}

function panelizer_choose_layout_form_validate(&$form, &$form_state) {
  if ($form_state['values']['layout'] == $form_state['display']->layout) {
    form_error($form['layout'], t('You must select a different layout if you wish to change layouts.'));
  }
}

function panelizer_choose_layout_form_submit(&$form, &$form_state) {
  $form_state['layout'] = $form_state['values']['layout'];
  $form_state['form_info']['path'] .= '/' . $form_state['values']['layout'];
}

function panelizer_move_content_form($form, &$form_state) {
  // Tell the Panels form not to display buttons.
  $form_state['no buttons'] = TRUE;

  // Change the #id of the form so the CSS applies properly.
  $form = panels_change_layout($form, $form_state);

  $form['buttons']['return']['#submit'][] = 'panels_change_layout_submit';
  panelizer_add_revision_info_form($form, $form_state);
  return $form;
}

function panelizer_edit_content_form($form, &$form_state) {
  ctools_include('ajax');
  ctools_include('plugins', 'panels');
  ctools_include('display-edit', 'panels');
  ctools_include('context');

  $cache = $form_state['display cache'];

  $form_state['renderer'] = panels_get_renderer_handler('editor', $cache->display);
  $form_state['renderer']->cache = $cache;

  $form_state['display'] = &$cache->display;
  $form_state['content_types'] = $cache->content_types;
  $form_state['display_title'] = TRUE;

  $form = panels_edit_display_form($form, $form_state);

  panelizer_add_revision_info_form($form, $form_state);
  $form['preview']['#weight'] = 100;

  return $form;
}
