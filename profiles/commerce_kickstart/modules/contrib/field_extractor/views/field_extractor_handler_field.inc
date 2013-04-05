<?php

/**
 * Field extractor views field.
 */
class field_extractor_handler_field extends views_handler_field_prerender_list {

  /**
   * Stores the entity type that contains the reference field.
   */
  public $entity_type;

  /**
   * Stores all entites which reference entities used for extracting.
   */
  public $entities;

  /**
   * Stores all entites used for extracting.
   */
  public $referenced_entities;

  /**
   * Stores the settings relevant to the reference field (entity type it is
   * referencing, allowed bundles, name of the column that stores the ids).
   */
  public $settings;

  public function init(&$view, &$options) {
    parent::init($view, $options);

    // Get the field, construct a fake instance array, get the settings.
    $field = field_info_field($this->definition['field_name']);
    $instance = array('settings' => array());
    $this->settings = field_extractor_settings($field, $instance);

    // Initialize the entity-type of the base table.
    $table_data = views_fetch_data($this->table);
    $this->entity_type = $table_data['table']['entity type'];
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['field_name'] = array(
      'default' => '',
    );
    $options['formatter'] = array(
      'default' => '',
    );
    $options['settings'] = array(
      'default' => array(),
    );

    return $options;
  }

  function options_form(&$form, &$form_state) {
    // Build a list of available fields.
    $fields = array();
    foreach (field_info_instances($this->settings['entity_type']) as $bundle => $instances) {
      foreach ($instances as $field_name => $field_instance) {
        $fields[$field_name] = $field_name;
      }
    }
    // If no field has been selected yet, or the current one is no longer
    // available, default to the first one.
    if (empty($this->options['field_name']) || !isset($fields[$this->options['field_name']])) {
      $field_names = array_keys($fields);
      $this->options['field_name'] = reset($field_names);
    }

    $form['field_name'] = array(
      '#type' => 'select',
      '#title' => t('Field'),
      '#options' => $fields,
      '#default_value' => $this->options['field_name'],
      '#ajax' => array(
        'path' => views_ui_build_form_url($form_state),
      ),
      '#submit' => array('views_ui_config_item_form_submit_temporary'),
      '#executes_submit_callback' => TRUE,
    );

    $selected_field = field_info_field($this->options['field_name']);
    $formatters = _field_extractor_formatters($selected_field['type']);
    // No recursion.
    unset($formatters['field_extractor']);
    // If no formatter has been selected yet, or doesn't fit the current field,
    // default to the first one.
    if (empty($this->options['formatter']) || !isset($formatters[$this->options['formatter']])) {
      $formatter_names = array_keys($formatters);
      $this->options['formatter'] = reset($formatter_names);
    }

    $form['formatter'] = array(
      '#type' => 'select',
      '#title' => t('Formatter'),
      '#options' => $formatters,
      '#default_value' => $this->options['formatter'],
      '#ajax' => array(
        'path' => views_ui_build_form_url($form_state),
      ),
      '#submit' => array('views_ui_config_item_form_submit_temporary'),
      '#executes_submit_callback' => TRUE,
    );

    $formatter = field_info_formatter_types($this->options['formatter']);
    $formatter_settings = $this->options['settings'] + field_info_formatter_settings($this->options['formatter']);
    // Provide an instance array for hook_field_formatter_settings_form().
    ctools_include('fields');
    $instance = ctools_fields_fake_field_instance($this->options['field_name'], '_dummy', $formatter, $formatter_settings);
    // Store the settings in a '_dummy' view mode.
    $instance['display']['_dummy'] = array(
      'type' => $this->options['formatter'],
      'settings' => $formatter_settings,
    );

    // Get the settings form.
    $settings_form = array('#value' => array());
    $function = $formatter['module'] . '_field_formatter_settings_form';
    if (function_exists($function)) {
      $settings_form = $function($selected_field, $instance, '_dummy', $element, $form_state);
    }
    $form['settings'] = $settings_form;

    parent::options_form($form, $form_state);
  }

  /**
   * The query method shouldn't do anything, since all information is taken
   * from entities loaded later in the process.
   */
  function query($use_groupby = FALSE) {}

  /**
   * Determine if this field is click sortable.
   */
  function click_sortable() {
    return FALSE;
  }

  /**
   * Load the entities for all rows that are about to be displayed.
   */
  function post_execute(&$values) {
    if (!empty($values)) {
      // Load the parent entities.
      list($this->entity_type, $this->entities) = $this->query->get_result_entities($values);

      // Gather a list of referenced ids, load the entities, then store them
      // in $this->referenced_entities grouped by referencing entity id.
      $referenced_ids = array();
      $referenced_ids_by_parent = array();
      foreach ($this->entities as $entity) {
        $items = field_get_items($this->entity_type, $entity, $this->definition['field_name']);
        if (!empty($items)) {
          list($entity_id) = entity_extract_ids($this->entity_type, $entity);
          foreach ($items as $item) {
            $referenced_ids[] = $item[$this->settings['column']];
            $referenced_ids_by_parent[$entity_id][] = $item[$this->settings['column']];
          }
        }
      }
      $referenced_entities = entity_load($this->settings['entity_type'], $referenced_ids);
      foreach ($referenced_ids_by_parent as $entity_id => $referenced_ids) {
        foreach ($referenced_ids as $id) {
          if (!empty($referenced_entities[$id])) {
            $this->referenced_entities[$entity_id][] = $referenced_entities[$id];
          }
        }
      }

      // Build the extracted field data for each row.
      foreach ($values as $row_id => &$value) {
        $entity = $this->entities[$row_id];
        list($entity_id) = entity_extract_ids($this->entity_type, $entity);
        if (!empty($this->referenced_entities[$entity_id])) {
          $element = field_extractor_build_element($this->referenced_entities[$entity_id], $this->settings['entity_type'], $this->options);
          // Only take the items from the renderable array.
          $items = array();
          foreach (element_children($element) as $key) {
            $items[$key] = $element[$key];
          }
        }
        else {
          $items = array();
        }

        $value->{'field_extractor_' . $this->options['id']} = $items;
      }
    }
  }

  function get_items($values) {
    return $values->{'field_extractor_' . $this->options['id']};
  }

  function render_item($count, $item) {
    $item['rendered'] = render($item);
    return $item['rendered'];
  }
}
