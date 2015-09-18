<?php

/**
 * @file
 * Provides views integration for fieldable panels panes.
 */

/**
 * Implements hook_views_data().
 */
function fieldable_panels_panes_views_data() {
  $data = array();

  // ------------------------------------------------------------------------
  // Panels panes table
  $data['fieldable_panels_panes'] = array(
    'table' => array(
      'base' => array(
        'field' => 'fpid',
        'title' => t('Fieldable pane entities'),
        'help' => t('A fieldable panel pane entity.'),
      ),
      'group' => t('Fieldable pane'),
      'entity type' => 'fieldable_panels_pane',
    ),
    'fpid' => array(
      'title' => t('Fieldable pane ID'),
      'help' => t('The internal ID of the pane.'),
      'relationship' => array(
        'title' => t('All revisions'),
        'base' => 'fieldable_panels_panes_revision',
        'field' => 'fpid',
        'label' => t('revision'),
      ),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'vid' => array(
      'title' => t('Current revision ID'),
      'help' => t('The internal ID of the pane.'),
      'relationship' => array(
        'title' => t('Current revision'),
        'base' => 'fieldable_panels_panes_revision',
        'field' => 'vid',
        'label' => t('revision'),
      ),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'bundle' => array(
      'title' => t('Bundle'),
      'help' => t('The bundle (type) of the entity.'),
      'field' => array(
        'handler' => 'fieldable_panels_panes_handler_field_bundle',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'fieldable_panels_panes_handler_filter_bundle',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'fieldable_panels_panes_handler_argument_bundle',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'title' => array(
      'title' => t('Title'),
      'help' => t('The content title of the entity.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'admin_title' => array(
      'title' => t('Administrative title'),
      'help' => t('The administrative title of the entity.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'admin_description' => array(
      'title' => t('Administrative description'),
      'help' => t('The administrative description of the entity.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'category' => array(
      'title' => t('Administrative category'),
      'help' => t('The administrative category of the entity.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'reusable' => array(
      'title' => t('Reusable'),
      'help' => t('Is this entity reusable and available to add to panels via the Add Content pane.'),
      'field' => array(
        'handler' => 'views_handler_field_boolean',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_boolean_operator',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'created' => array(
      'title' => t('Created date'),
      'help' => t('The date the entity was created.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
    ),
    'changed' => array(
      'title' => t('changed date'),
      'help' => t('The date the entity was changed.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
    ),
    'view_entity' => array(
      'field' => array(
        'title' => t('Administrative link'),
        'help' => t('Provide a simple link for administrators to view the content.'),
        'handler' => 'fieldable_panels_panes_handler_field_view_entity',
      ),
    ),
    'edit_entity' => array(
      'field' => array(
        'title' => t('Administrative edit link'),
        'help' => t('Provide a simple link for administrators to edit the content.'),
        'handler' => 'fieldable_panels_panes_handler_field_edit_entity',
      ),
    ),
    'delete_entity' => array(
      'field' => array(
        'title' => t('Administrative delete link'),
        'help' => t('Provide a simple link for administrators to delete the content.'),
        'handler' => 'fieldable_panels_panes_handler_field_delete_entity',
      ),
    ),
  );

  $data['fieldable_panels_panes_revision'] = array(
    'table' => array(
      'base' => array(
        'field' => 'vid',
        'title' => t('Fieldable pane entity revisions'),
        'help' => t('Revisions for fieldable panel pane entities.'),
      ),
      'group' => t('Fieldable pane (historical data)'),
      'entity type' => 'fieldable_panels_pane',
    ),
    'fpid' => array(
      'title' => t('Fieldable pane ID'),
      'help' => t('The pane entity this is a revision for.'),
      'relationship' => array(
        'title' => t('Fieldable pane entity'),
        'base' => 'fieldable_panels_panes',
        'field' => 'fpid',
        'label' => t('pane'),
      ),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'vid' => array(
      'title' => t('Revision ID'),
      'help' => t('The internal ID of the revision.'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'title' => array(
      'title' => t('Title'),
      'help' => t('The content title of the this revision of the entity.'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'timestamp' => array(
      'title' => t('Created date'),
      'help' => t('The date the entity was created.'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
    ),
    'log' => array(
      'title' => t('Log message'),
      'help' => t('The message provided for the revision.'),
      'field' => array(
        'handler' => 'views_handler_field_xss',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
        'allow empty' => TRUE,
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    ),
    'uid' => array(
      'title' => t('Author UID'),
      'help' => t('The user authoring the revision. If you need more fields from the author, add the Author relationship.'),
      'relationship' => array(
        'title' => t('Author'),
        'help' => t('Relate the revision to the user who created it.'),
        'handler' => 'views_handler_relationship',
        'base' => 'users',
        'field' => 'uid',
        'label' => t('author'),
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_user_name',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'field' => array(
        'handler' => 'views_handler_field_user',
      ),
    ),
    'view_revision' => array(
      'field' => array(
        'title' => t('Administrative link'),
        'help' => t('Provide a simple link for administrators to view the content.'),
        'handler' => 'fieldable_panels_panes_handler_field_view_revision',
      ),
    ),
    'edit_revision' => array(
      'field' => array(
        'title' => t('Administrative edit link'),
        'help' => t('Provide a simple link for administrators to edit the content.'),
        'handler' => 'fieldable_panels_panes_handler_field_edit_revision',
      ),
    ),
    'delete_revision' => array(
      'field' => array(
        'title' => t('Administrative delete link'),
        'help' => t('Provide a simple link for administrators to delete the content.'),
        'handler' => 'fieldable_panels_panes_handler_field_delete_revision',
      ),
    ),
    'is_current' => array(
      'field' => array(
        'title' => t('Is current'),
        'help' => t('Determine if this is the current revision for the entity.'),
        'handler' => 'fieldable_panels_panes_handler_field_is_current',
      ),
    ),
    'make_current' => array(
      'field' => array(
        'title' => t('Administrative set current revision link'),
        'help' => t('Provide a simple link for administrators to delete the content.'),
        'handler' => 'fieldable_panels_panes_handler_field_make_current',
      ),
    ),
  );

  return $data;
}
