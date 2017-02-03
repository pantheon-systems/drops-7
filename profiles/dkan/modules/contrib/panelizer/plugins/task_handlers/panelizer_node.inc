<?php
/**
 * @file
 *
 * This is the task handler plugin to handle an entity view.
 * NOTE: This is named panelizer_node for historical reasons. It is too much
 * of a pain to change the name of a task handler. This panelizes any entity
 * not just a node.
 */

// Plugin definition
$plugin = array(
  // is a 'context' handler type, meaning it supports the API of the
  // context handlers provided by ctools context plugins.
  'handler type' => 'context',
  // Administrative fields.
  'title' => t('Panelizer'),
  'forms' => array(),
  'admin title' => 'panelizer_panelizer_task_title',
  'admin summary' => 'panelizer_panelizer_task_admin_summary',
  'operations' => array(),
  // Callback to render the data.
  'render' => 'panelizer_panelizer_task_render',
  // Callback to return addressable data
  'addressable callback' => 'panelizer_panelizer_task_get_addressable',

  'test' => 'panelizer_panelizer_task_test',
  'visible' => TRUE,

  // Provide custom panelizer specific contextual links.
  'contextual link' => 'panelizer_panelizer_task_contextual_link',

  'operations' => array(
    'settings' => array(
      'title' => t('General'),
      'description' => t('Change general settings for this variant.'),
      'form' => 'panelizer_panelizer_task_edit_settings',
    ),
    'criteria' => array(
      'title' => t('Selection rules'),
      'description' => t('Control the criteria used to decide whether or not this variant is used.'),
      'ajax' => FALSE,
      'form' => array(
        'order' => array(
          'form' => t('Selection rules'),
        ),
        'forms' => array(
          'form' => array(
            'include' => drupal_get_path('module', 'ctools') . '/includes/context-task-handler.inc',
            'form id' => 'ctools_context_handler_edit_criteria',
          ),
        ),
      ),
    ),
    'context' => array(
      'title' => t('Contexts'),
      'ajax' => FALSE,
      'description' => t('Add additional context objects to this variant that can be used by the content.'),
      'form' => array(
        'order' => array(
          'form' => t('Context'),
        ),
        'forms' => array(
          'form' => array(
            'include' => drupal_get_path('module', 'ctools') . '/includes/context-task-handler.inc',
            'form id' => 'ctools_context_handler_edit_context',
          ),
        ),
      ),
    ),
  ),
  'default conf' => array(
    'title' => t('Panelizer'),
    'contexts' => array(),
    'relationships' => array(),
    'context' => array(),
  ),

);

/**
 * Figure out the correct context to use for this panelizer.
 */
function _panelizer_panelizer_task_get_context($handler, $contexts) {
  if (isset($handler->conf['context']) && !empty($contexts[$handler->conf['context']])) {
    return $contexts[$handler->conf['context']];
  }

  // If one was not set up, we could be using old, saved data. Assume that
  // this means we just want the first context available.
  if (!empty($contexts)) {
    return reset($contexts);
  }

  // Fail!
  return stdClass();
}

/**
 * Provide appropriate contextual links for Panelizer controled entities.
 */
function panelizer_panelizer_task_contextual_link($handler, $plugin, $contexts, $args) {
  $links = array();

  $context = _panelizer_panelizer_task_get_context($handler, $contexts);
  if (empty($context->data)) {
    return;
  }

  $entity = &$context->data;

  if (empty($entity->panelizer['page_manager'])) {
    return FALSE;
  }

  $panelizer = $entity->panelizer['page_manager'];
  // One of these two will always be set.
  $entity_type = !empty($panelizer->entity_type) ? $panelizer->entity_type : $panelizer->panelizer_type;

  if ($entity_handler = panelizer_entity_plugin_get_handler($entity_type)) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($entity_type, $entity);

    // @todo there's code on the entity handler to do this path thing for us.
    $bits = explode('/', $entity_handler->plugin['entity path']);
    foreach ($bits as $count => $bit) {
      if (strpos($bit, '%') === 0) {
        $bits[$count] = $entity_id;
      }
    }

    $bits[] = 'panelizer';
    $bits[] = 'page_manager';

    $base_path = implode('/', $bits);
    if ($entity_handler->panelizer_access('settings', $entity, 'page_manager')) {
      $links['settings'] = array(
        'title' => t('Edit panelizer settings'),
        'href' => "$base_path/settings",
      );
    }
    if ($entity_handler->panelizer_access('context', $entity, 'page_manager')) {
      $links['context'] = array(
        'title' => t('Edit panelizer contexts'),
        'href' => "$base_path/context",
      );
    }
    if ($entity_handler->panelizer_access('layout', $entity, 'page_manager')) {
      $links['layout'] = array(
        'title' => t('Edit panelizer layout'),
        'href' => "$base_path/layout",
      );
    }
    if ($entity_handler->panelizer_access('content', $entity, 'page_manager')) {
      $links['content'] = array(
        'title' => t('Edit panelizer content'),
        'href' => "$base_path/content",
      );
    }
  }

  return $links;
}

/**
 * Callback to provide administrative summary of the task handler.
 */
function panelizer_panelizer_task_admin_summary($handler, $task, $subtask, $page, $show_title = TRUE) {
  ctools_include('context');
  ctools_include('context-task-handler');

  $output = '';

  $output .= '<div class="clear-block">';
  if ($show_title) {
    // Get the operations
    $operations = page_manager_get_operations($page);

    // Get operations for just this handler.
    $operations = $operations['handlers']['children'][$handler->name]['children']['actions']['children'];
    $args = array('handlers', $handler->name, 'actions');
    $rendered_operations = page_manager_render_operations($page, $operations, array(), array('class' => array('actions')), 'actions', $args);

    $output .= '<div class="handler-title clear-block">';
    $output .= '<div class="actions handler-actions">' . $rendered_operations['actions'] . '</div>';
    $output .= '<span class="title-label">' . t('Panelizer') . '</span>';
    $output .= '</div>';
  }

  $plugin = page_manager_get_task_handler($handler->handler);

  $object = ctools_context_handler_get_task_object($task, $subtask, $handler);
  $context = ctools_context_load_contexts($object, TRUE);

  $access = ctools_access_group_summary(!empty($handler->conf['access']) ? $handler->conf['access'] : array(), $context);
  if ($access) {
    $access = t('This variant will be selected if the entity being viewed is panelized AND @conditions. This variant must be enabled and selected for panelizer to work!', array('@conditions' => $access));
  }
  else {
    $access = t('This variant will be selected if the entity being viewed is panelized. This variant must be enabled and selected for panelizer to work!');
  }

  $rows[] = array(
    array(
      'class' => array('page-summary-label'),
      'data' => t('Selection rule'),
    ),
    array(
      'class' => array('page-summary-data'),
      'data' => $access,
    ),
    array('class' => array('page-summary-operation'), ''),
  );

  $output .= theme('table', array('header' => array(), 'rows' => $rows, 'attributes' => array('class' => array('page-manager-handler-summary'))));
  $output .= '</div>';

  return $output;
}

/**
 * Render a entity that has been panelized.
 */
function panelizer_panelizer_task_render($handler, $base_contexts, $args, $test = TRUE) {
  // Get the context this is viewing; figure out what entity it is and load
  // the right plugin.
  ctools_include('context');
  $context = _panelizer_panelizer_task_get_context($handler, $base_contexts);
  if (!$context) {
    return FALSE;
  }

  if (empty($context->data->panelizer['page_manager'])) {
    return FALSE;
  }

  $panelizer = $context->data->panelizer['page_manager'];
  // One of these two will always be set.
  $entity_type = !empty($panelizer->entity_type) ? $panelizer->entity_type : $panelizer->panelizer_type;

  $address = implode('::', array('page_manager', $handler->task, $handler->subtask, $handler->name, implode('..', $args)));

  if ($entity_handler = panelizer_entity_plugin_get_handler($entity_type)) {
    $info = $entity_handler->render_entity($context->data, 'page_manager', NULL, $args, $address);

    // Remove and add body element classes
    $panel_body_css = &drupal_static('panel_body_css');
    if (!empty($info['classes_array']) && !isset($panel_body_css['body_classes_to_add'])) {
      $panel_body_css['body_classes_to_add'] = implode(' ', $info['classes_array']);
    }

    return $info;
  }
}

/**
 * Determine if the panelizer task handler should fire.
 *
 * This returns true if the configured entity is panelized and has
 * a display.
 */
function panelizer_panelizer_task_test($handler, $base_contexts) {
  if (empty($base_contexts)) {
    return;
  }

  if (!ctools_context_handler_select($handler, $base_contexts)) {
    return;
  }

  $context = _panelizer_panelizer_task_get_context($handler, $base_contexts);
  if (empty($context->data)) {
    return;
  }

  $entity = &$context->data;
  // Fail if there isn't a panelizer object on the entity.
  if (empty($entity->panelizer['page_manager']->display)) {
    return;
  }

  $panelizer = $entity->panelizer['page_manager'];
  $entity_type = !empty($panelizer->entity_type) ? $panelizer->entity_type : $panelizer->panelizer_type;

  $panelizer_handler = panelizer_entity_plugin_get_handler($entity_type);
  if (!$panelizer_handler) {
    return;
  }

  list($entity_id, $revision_id, $bundle) = entity_extract_ids($entity_type, $entity);

  // Make sure the bundle + view mode is actually panelized!
  return $panelizer_handler->is_panelized($bundle . '.page_manager');
}

function panelizer_panelizer_task_get_addressable($task, $subtask_name, $handler, $address, $contexts, $arguments, $type) {
  ctools_include('plugins', 'panels');
  if (empty($contexts)) {
    return;
  }

  $context = _panelizer_panelizer_task_get_context($handler, $contexts);
  if (empty($context->data)) {
    return;
  }

  // Extract the entity from the context so we can load the panelizer.
  $entity = &$context->data;

  if (empty($entity->panelizer['page_manager']) || empty($entity->panelizer['page_manager']->display)) {
    return;
  }

  // Load the panelizer info.
  $panelizer = $entity->panelizer['page_manager'];
  // Load the display.
  $display = $panelizer->display;

  // One of these two will always be set.
  $entity_type = !empty($panelizer->entity_type) ? $panelizer->entity_type : $panelizer->panelizer_type;
  $handler = panelizer_entity_plugin_get_handler($entity_type);
  if (!$handler) {
    return;
  }

  list($entity_id, $revision_id, $bundle) = entity_extract_ids($entity_type, $entity);

  $display->context = $handler->get_contexts($panelizer, $entity);
  $display->args = $arguments;
  $display->css_id = $panelizer->css_id;

  $display->cache_key = implode(':', array('panelizer', $entity_type, $entity_id));

  $renderer = panels_get_renderer($panelizer->pipeline, $display);
  $renderer->prepare();

  $pid = array_shift($address);
  if (!empty($renderer->prepared['panes'][$pid])) {
    if ($type == 'content') {
      return $renderer->render_pane($renderer->prepared['panes'][$pid]);
    }
    elseif ($type == 'pane') {
      return $renderer->prepared['panes'][$pid];
    }
  }
}

/**
 * General settings for the panel
 */
function panelizer_panelizer_task_edit_settings($form, &$form_state) {
  ctools_include('context');
  ctools_include('context-task-handler');
  $conf = $form_state['handler']->conf;

  $form['title'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($conf['title']) ? $conf['title'] : t('Panelizer'),
    '#title' => t('Administrative title'),
    '#description' => t('Administrative title of this variant.'),
  );

  $contexts = ctools_context_handler_get_all_contexts($form_state['task'], $form_state['subtask'], $form_state['handler']);

  $required_context = new ctools_context_required(t('Panelized entity'), 'entity');
  $form['context'] = ctools_context_selector($contexts, $required_context, isset($conf['context']) ? $conf['context'] : '');

  return $form;
}

function panelizer_panelizer_task_edit_settings_submit($form, &$form_state) {
  $form_state['handler']->conf['title'] = $form_state['values']['title'];
  $form_state['handler']->conf['context'] = $form_state['values']['context'];
}

/**
 * Set up a title for the panel based upon the selection rules.
 */
function panelizer_panelizer_task_title($handler, $task, $subtask) {
  if (isset($handler->conf['title'])) {
    return check_plain($handler->conf['title']);
  }
  else {
    return t('Panelizer');
  }
}
