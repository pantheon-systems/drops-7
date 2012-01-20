<?php

/**
 * @file
 * Provides a new box-type that provides Views as boxes.
 */

/**
 * Views content listing box.
 */
class views_boxes_view extends boxes_box {
  /**
   * Implementation of boxes_box::options_defaults().
   */
  public function options_defaults() {
    return array(
      'view' => '',
      'settings' => array(),
      'view_object' => NULL,
      'view_more_text' => '',
      'view_more_link' => '',
      'expose_filters' => FALSE,
    );
  }
  /**
   * implements boxes_box::option_submit()
   * This needs a boxes patch
   */
  public function options_submit($form, &$form_state) {
    
    //$settings_form = $form['options']['settings'];
    //views_exposed_form_submit($settings_form, $form_state);
    foreach ($form['options']['settings'] as $key => $element) {
      if (isset($form_state['input'][$key])) {
        $this->options['settings'][$key] = $form_state['input'][$key];
      }
    }
  }


  /**
   * we use fancy ajax on our options form and this breaks with the context ui
   */
  function use_multistep_create() {
    return TRUE;
  }

  /**
   * Overrides boxes_box::cache_setting().
   *
   * This delegates each Views Box instance's cache setting to the value on the
   * View's Block display 'Block cache' value.
   */
  public function cache_setting() {

    $view_info = explode('--', $this->options['view']);
    // We are getting this from the cached views
    if ($view_info && count($view_info) == 2 && $view = views_get_view($view_info[0])) {
      foreach ($view->display as $id => $display) {
        if (($id == $view_info[1]) && (isset($display->display_options['block_caching']))) {
          return $display->display_options['block_caching'];
        }
      }
    }

    return DRUPAL_CACHE_CUSTOM;
  }

  /**
   * Implementation of boxes_box::options_form().
   *
   * We are presenting a form element for the view to use
   * and then a field set of elements based on that views exposed form and arguments
   */
  public function options_form(&$form_state) {

    drupal_add_css(drupal_get_path('module', 'views_boxes') . '/plugins/views_boxes_view.css');
    drupal_add_js(drupal_get_path('module', 'views_boxes') . '/plugins/views_boxes_view.js');
    $form = array();
    $view_options = array();

    // Find all view displays that are blocks
    foreach (views_get_enabled_views() as $view) {
      $name = $view->name;
      foreach ($view->display as $display_name => $display) {
        // Only show block displays
        if ($display->display_plugin == 'block')  {
          $view_options[$view->human_name]["$name--$display_name"] = "{$view->human_name} - {$display->display_title}";
        }
      }
    }
    
    // form element for the view, change this set off an ajax call on the settings
    $form['view'] = array(
      '#type' => 'select',
      '#title' => t('View'),
      '#description' => t('Select the View and Display to use.'),
      '#options' => $view_options,
      '#default_value' => $this->options['view'],
      '#ajax' => array(
        'callback' => 'views_boxes_view_settings_callback',
        'wrapper' => 'view_settings',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );

    // wrapper for exposed form and arguments
    $form['settings'] = array(
      '#type' => 'fieldset',
      '#title' => 'Settings',
      '#prefix' => '<div id="view_settings">',
      '#suffix' => '</div>'
    ); 

    // EXPOSED FILTERS
    $view_string = !empty($form_state['values']['view']) ? $form_state['values']['view'] : $this->options['view'];
    list($view_name, $display_name)  = (preg_match('/--/', $view_string)) ? explode('--', $view_string) : array('', '');
    if ($view_name) {

      // Insure that we use values that have been set in the form
      // on ajax calls
      $view = views_get_view($view_name, TRUE);
      $view->set_display($display_name);
      $view->set_exposed_input($this->options['settings']);
      $view->execute();

      // Get the exposed form and use it to build the settings_form
      $exposed_form = $view->display_handler->get_plugin('exposed_form');
      $settings_form_state = array();
      list($settings_form, $settings_form_state) = $this->render_exposed_form($exposed_form, $settings_form_state);
      $form_state['settings_form_state'] = $settings_form_state;
      $temp_form_state = $form_state;
      $this->options_submit(array("options" => array("settings" => $settings_form)), $temp_form_state);

      // Iterate over each item and make sure it has a title and add back
      // our default values
      foreach ($settings_form as $id => $element) {
        if (isset($view->filter[$id])) {
          $handler = $view->filter[$id];
          $title = $handler->options['ui_name'] ? $handler->options['ui_name'] : $id;
          $title = $handler->options['expose']['label'] ? $handler->options['expose']['label'] : $title;
          $settings_form[$id]['#title'] = $title;
        }
        if (isset($this->options['settings'][$id])) {
          // if we have an options array we want to make sure our default value 
          // is in the options before we set it as the value
          $default_value = $this->options['settings'][$id];
          if (isset($settings_form[$id]['#options'])) {
            if (is_array($default_value)) {
              foreach ($default_value as $key => $value) {
                if (!in_array($value, array_keys($settings_form[$id]['#options']))) {
                  unset($default_value[$key]);
                }
              }
              if (empty($default_value)) {
                unset($default_value);
              }
              else {
                if (!($settings_form[$id]['#multiple'])) {
                  $default_value = $default_value[0];
                  unset($default_value);
                }
              }
            }
            else {
                if (!in_array($default_value, array_keys($settings_form[$id]['#options']))) {
                  unset($default_value);
                }
            }
          }
          if (isset($default_value)) {
            $settings_form[$id]['#default_value'] = $default_value;
          }
        }
        /*
        // we want to reorder the options to the same order as our stored values
        // incase the order matters
        if (is_array($this->options['settings'][$id])) {
          $options = array();
          foreach ($this->options['settings'][$id] as $key => $on) {
            $options[$key] = $settings_form[$id]['#options'][$key];
          }
          $settings_form[$id]['#options'] = $options + $settings_form[$id]['#options'];
        }
        // Make checkboxes instead of multi select for the sort options
        // This really should be part of the filter settings
        if ($settings_form[$id]['#type'] =='select' && $settings_form[$id]['#multiple'] == TRUE  && $id == 'sort') {
          $settings_form[$id]['#type'] ='checkboxes';
        }
        */
      }

      // ARGUMENTS
      // foreach argument let's make a element in the settings_form
      foreach ($view->argument as $key => $arg) {

        // We are doing some custom handling on the id argument type
        // basically we are hiding the field that holds all of the ids
        // and making autocomplete fields for node selection.
        // We then update the original field with JS.
        $re = "/\[(.*)\]/";
        $entity_type = FALSE;

        // Check for the following format of the admin title
        // [entity type]
        // if we have it remove it from the name and set the entity type
        if (preg_match($re, $arg->options['ui_name'], $matches)) {
          $entity_type = $matches[1];
          $arg->options['ui_name'] = preg_replace($re, '', $arg->options['ui_name']);
        }
        
        // See if we're going to filters on specific bundles
        $re = "/\{bundles:(.*)\}/";
        if (preg_match($re, $arg->options['ui_name'], $matches)) {
          $bundles = $matches[1];
          $arg->options['ui_name'] = preg_replace($re, '', $arg->options['ui_name']);
        }
        
        if ((get_class($arg) == 'views_handler_argument_null') ||
             ($arg->field == 'tid') ||
             ($arg->field == 'nid') ||
             $entity_type) {
          // we need to get info about our entity 
          // for now we are only looking at nodes and taxonomy_terms
          if (isset($form_state['values'][$key])) {
            $this->options['settings'][$key] = $form_state['values'][$key];
          }

          $entity_type_array = array(
            'nid' => 'node',
            'tid' => 'taxonomy_term',
          );
          $entity_type = $entity_type ? $entity_type : (isset($entity_type_array[$arg->field]) ? $entity_type_array[$arg->field] : 'node');
          $entity_info = entity_get_info($entity_type);
          // Grab the name of the label and id field so that we can build the 
          // autocomplete field values later
          $id_field = $entity_info['entity keys']['id'];
          $label_field = $entity_info['entity keys']['label'];

          // Build the field group to put all of our widgets in
          $title = $arg->options['ui_name'] ? $arg->options['ui_name'] : $key;
          $settings_form["{$key}_group"] = array(
            '#title' => $title,
            '#type' => "fieldset",
            '#prefix' => "<div id = '$key-group' class='id-group'>",
            '#suffix' => "</div>",
          );

          // Build the actual value field (this is what gets sent to the view)
          $settings_form[$key] = array(
            '#type' => 'textfield',
            '#default_value' => $this->options['settings'][$key],
            '#attributes' => array('class' => array('id-value'), 'group' => "$key-group", 'style' => 'display:none'),
          );

          // Wrap the sortable items
          $settings_form["{$key}_group"]["{$key}_container_start"] = array(
            '#markup' => "<div class='id-sortable' key='$key' id='$key-wrapper'>",
            '#weight' => -1,
          );

          // Find the current values and build auto complete fields that are populated
          $ids = explode("+", $this->options['settings'][$key]);
          $entities = entity_load($entity_type, $ids);
          $count = 0;
          
          // Restrict autocomplete to specific entity or bundle(s)
          if (!isset($bundles)) {
            $autocomplete_path = "entity-autocomplete/{$entity_type}";
          }
          else {
            $autocomplete_path = "entity-autocomplete/bundle/{$entity_type}/" . $bundles;
          }
          
          foreach ($entities as $index => $entity) {
            $id = $index;

            // Adding a rand to the key so the value does not get cached
            $settings_form["{$key}_group"][$key . "_$id-" . rand(0, 10000)] = array(
              '#type' => 'textfield',
              '#default_value' => "{$entity->{$label_field}} [id:$id]",
              '#autocomplete_path' => $autocomplete_path,
              '#prefix' => '<div class = "id-item"><div class= handle></div>',
              '#attributes' => array('class' => array('id-field'), 'key' => $key),
              '#suffix' => '</div>',
              '#weight' => $count,
            );
            $count ++;
          }

          // if the use asked for a new field or we have no fields add a empty field
          if ($form_state['clicked_button']['#key'] == $key || ($count == 0)) {
            $settings_form["{$key}_group"][$key . "new-" . rand(0, 10000)] = array(
              '#type' => 'textfield',
              '#default_value' => "",
              '#autocomplete_path' => $autocomplete_path,
              '#prefix' => '<div class = "id-item"><div class= handle></div>',
              '#attributes' => array('class' => array('id-field'), 'key' => $key),
              '#suffix' => '</div>',
              '#weight' => 1000,
            );
          }
          // End the sortable group
          $settings_form["{$key}_group"]["{$key}_container_end"] = array(
            '#markup' => '</div>',
            '#weight' => 1001,
          );
          // Add a button to add a new item
          $settings_form["{$key}_group"]["{$key}_add"] = array(
            '#type' => 'button',
            '#value' => "Add New $title",
            '#key' => $key,
            '#weight' => 1002,
              '#attributes' => array('key' => $key),
            '#ajax' => array(
              'callback' => 'views_boxes_view_id_callback',
              'wrapper' => "{$key}-group",
              'method' => 'replace',
              'effect' => 'fade',
            ),

          );
        }
        else {
          $settings_form[$key] = array(
            '#type' => 'textfield',
            '#default_value' => $this->options['settings'][$key],
            '#title' => $arg->options['ui_name'] ? $arg->options['ui_name'] : $key,
          );
        }
      }
      // if there are no settings we want to keep the field set
      // but make it clear that it is empty
      if (empty($settings_form)) {
        $form['settings']['empty'] = array(
          '#markup' => t('This view has no settings'),
        );
      }
      else {
        $form['settings'] = $form['settings'] + $settings_form;
      }

    }
    else {
        // Leave the setting fieldset but make it clear that we do not yet have a view
        $form['settings']['empty'] = array(
          '#markup' => t('No view selected'),
        );
    }
    $form['view_more_text'] = array(
      '#title' => t('More link text'),
      '#description' => t('Optional text for a custom link that follows the listing.'),
      '#type' => 'textfield',
      '#default_value' => $this->options['view_more_text'],
    );
    $form['view_more_link'] = array(
      '#title' => t('More link URL'),
      '#description' => t('Optional URL for a custom link that follows the listing. Need to be a valid URL.'),
      '#type' => 'textfield',
      '#default_value' => $this->options['view_more_link'],
    );
    $form['expose_filters'] = array(
      '#title' => t('Expose filters in render'),
      '#description' => t('By default in Views Boxes, exposed filters are meant to be used for editing the box settings for the view and are not rendered in the output. Checking this box will show them in the rendered view.'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['expose_filters'],
    );
    return $form;
  }

  /**
   * Implementation of boxes_box::render().
   */
  public function render() {
    $content = '';
    // Only build a view if we have one
    if (
      ($view_info = explode('--', $this->options['view'])) &&
      ($view = views_get_view($view_info[0], TRUE))
    ) {
      $display_name = $view_info[1];

      $view->set_exposed_input($this->options['settings']);
      $view->exposed_data = $this->options['settings'];
      $view->set_display($display_name);
      $view->load($display_name);

      // Set arguments
      $args = array();
      $view->display_handler->get_handlers('argument');
      
      // Run though the arguments and set values if we have them, set wildcard otherwise
      foreach ($view->display_handler->handlers['argument'] as $key => $arg) {
        if (!empty($this->options['settings'][$key])) {
          $args[] = $this->options['settings'][$key];
        }
        else {
          $args[] = $arg->options['exception']['value'];
        }
      }
      if (!empty($args)) {
        $view->set_arguments($args);
      }

      // we need to execute before we remove the exposed widgets
      // but this is not an issue because render will not re-execute
      $view->pre_execute($args);
      $view->execute($display_name);
      $view->post_execute();

      // We want to remove the filters by default
      if (!isset($this->options['expose_filters']) || !$this->options['expose_filters']) {
        unset($view->exposed_widgets);
      }

      $content = $view->render($display_name);

      // Let's use the box title first,
      // if it is not set, let's use the views
      // if it is set to <none>, let's not have a title
      $title = $this->title;
      if (!$title) {
        $options = $view->display_handler->options;
        if ($options['defaults']['title']) {
          $title = isset($view->display['default']->display_options['title']) ? check_plain($view->display['default']->display_options['title']) : NULL;
        }
        else {
          $title = isset($view->display_handler->options['title']) ? check_plain($view->display_handler->options['title']) : NULL;
        }
      }
      elseif ($title == '<none>') {
        $title = NULL;
      }
    }
    else {
       $content = t("No view selected");
       $title = $this->title;
    }
    // More link
    if ($this->options['view_more_text']) {
      if (valid_url($this->options['view_more_link'])) {
        $content .= '<div class="box-more-link">' . 
          l($this->options['view_more_text'], $this->options['view_more_link']) . '</div>';
      }
    }
    
    $box = array(
      'delta' => $this->delta, // Crucial.
      'title' => $title,
      'subject' => $title,
      'content' => $content,
      'is_empty' => FALSE,
      'view_more_text' => $this->options['view_more_text'],
      'view_more_link' => $this->options['view_more_link'],
    );
    
    if ((isset($view)) && (count($view->result) == 0) && !boxes_access_edit()) {
      $box['is_empty'] = TRUE;
    }
    
    if ($display_name) {
      $box['current_display'] = $display_name;
    }
    if ($view->name) {
      $box['current_view'] = $view->name;
    }  
    return $box;
  }

  /*
   * This is mostly a copy of exposed_form::render_exposed_form()
   *
   * But we do not do the render step and instead pass on the form and form_state
   */
  function render_exposed_form($that, &$form_state, $block = FALSE) {
    // Deal with any exposed filters we may have, before building.
      $e = $this->options['settings']['exposed_input'];
      //$_SESSION['views'][$that->view->name][] 
      //$that->view->exposed_data = $e;
      //$that->view->set_exposed_input($e);
    $form_state = $form_state + array(
      'view' => &$that->view,
      'display' => &$that->display,
      'method' => 'get',
      'rerender' => TRUE,
      'no_redirect' => TRUE,
      'always_process' => TRUE,
      'input' => $e
    );

    // Some types of displays (eg. attachments) may wish to use the exposed
    // filters of their parent displays instead of showing an additional
    // exposed filter form for the attachment as well as that for the parent.
    if (!$that->view->display_handler->displays_exposed() || (!$block && $that->view->display_handler->get_option('exposed_block'))) {
      unset($form_state['rerender']);
    }

    if (!empty($that->ajax)) {
      $form_state['ajax'] = TRUE;
    }

    $form_state['exposed_form_plugin'] = $that;
    $form = drupal_build_form('views_exposed_form', $form_state);
    foreach ($form as $key => $item) {
      if (preg_match("/(^#|^form|submit)/", $key)) {
        unset($form[$key]);
      }
    }
    $output = array($form, $form_state);
    if (!empty($form_state['js settings'])) {
      $that->view->js_settings = $form_state['js settings'];
    }

    if (!$that->view->display_handler->displays_exposed() || (!$block && $that->view->display_handler->get_option('exposed_block'))) {
      return "";
    }
    else {
      return $output;
    }

  }
}
drupal_add_js(drupal_get_path('module', 'views_boxes') . '/plugins/views_boxes_view.js');
drupal_add_css(drupal_get_path('module', 'views_boxes') . '/plugins/views_boxes_view.css');
