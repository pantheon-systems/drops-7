<?php
/**
 * @file
 * Support for fieldable_panels_pane destinations.
 *
 * Based on MigrateDestinationNode in migrate module
 * (migrate/plugins/destinations/node.inc).
 */
// @todo Make sure this works with updates, explicit destination keys.

/**
 * Destination class implementing migration into fieldable panels panes.
 */
class MigrateDestinationFieldablePanelsPanes extends MigrateDestinationEntity {

  static public function getKeySchema() {
    return array(
      'fpid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'description' => 'The primary identifier for the entity.',
      ),
    );
  }

  /**
   * Return an options array for fieldable panels pane destinations.
   *
   * @param string $language
   *   Default language for fieldable panels panes created via this destination
   *   class.
   * @param string $text_format
   *   Default text format for fieldable panels panes created via this
   *   destination class.
   */
  static public function options($language, $text_format) {
    return compact('language', 'text_format');
  }

  /**
   * Basic initialization.
   *
   * @param string $bundle
   *   A.k.a. the fieldable panels pane type (the default fieldable_panels_pane,
   *   etc.) of the fieldable panels pane.
   * @param array $options
   *   Options applied to fieldable panels panes.
   */
  public function __construct($bundle, array $options = array()) {
    parent::__construct('fieldable_panels_pane', $bundle, $options);
  }

  /**
   * Returns a list of fields available to be mapped for the fieldable panels
   * pane type (bundle).
   *
   * @param Migration $migration
   *   Optionally, the migration containing this destination.
   * @return array
   *   Keys: machine names of the fields (to be passed to addFieldMapping)
   *   Values: Human-friendly descriptions of the fields.
   */
  public function fields($migration = NULL) {
    $fields = array();
    // First the core (fieldable_panels_panes table) properties.
    $fields['fpid'] = t('The primary identifier for the entity.');
    $fields['vid'] = t('The current version in use for this entity.');
    $fields['bundle'] = t('The bundle of the entity.');
    $fields['title'] = t('The title of the entity.');
    $fields['link'] = t('Whether or not this entity title will link to another page.');
    $fields['path'] = t('The path the title should link to.');
    $fields['reusable'] = t('Whether or not this entity will appear in the Add Content dialog.');
    $fields['admin_title'] = t('The title it will appear in the Add Content dialog as.');
    $fields['admin_description'] = t('The description it will appear in the Add Content dialog with.');
    $fields['category'] = t('The category it will appear in the Add Content dialog under.');
    $fields['view_access'] = t('Access rules to describe if the user has view access to this entity.');
    $fields['edit_access'] = t('Access rules to describe if the user has view access to this entity.');
    $fields['created'] = t('The Unix timestamp when the entity was created.');
    $fields['changed'] = t('The Unix timestamp when the entity was most recently saved.');
    $fields['uuid'] = t('The Universally Unique Identifier.');
    $fields['language'] = t('The languages.language of this entity.');

    // Then add in anything provided by handlers.
    $fields += migrate_handler_invoke_all('Entity', 'fields', $this->entityType, $this->bundle, $migration);
    $fields += migrate_handler_invoke_all('FieldablePanelsPanes', 'fields', $this->entityType, $this->bundle, $migration);

    return $fields;
  }

  /**
   * Delete a batch of fieldable panels panes at once.
   *
   * @param $fpids
   *   Array of fieldable panels pane IDs to be deleted.
   */
  public function bulkRollback(array $fpids) {
    migrate_instrument_start('entity_delete_multiple');
    $this->prepareRollback($fpids);
    entity_delete_multiple('fieldable_panels_pane', $fpids);
    $this->completeRollback($fpids);
    migrate_instrument_stop('entity_delete_multiple');
  }

  /**
   * Import a single fieldable panels pane.
   *
   * @param $fieldable_panels_pane
   *   Fieldable panels pane object to build. Prefilled with any fields mapped
   *   in the Migration.
   * @param $row
   *   Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *   Array of key fields (fpid only in this case) of the fieldable panels pane
   *   that was saved if successful. FALSE on failure.
   */
  public function import(stdClass $fieldable_panels_pane, stdClass $row) {
    // Updating previously-migrated content?
    $migration = Migration::currentMigration();
    if (isset($row->migrate_map_destid1)) {
      // Make sure is_new is off.
      $fieldable_panels_pane->is_new = FALSE;
      if (isset($fieldable_panels_pane->fpid)) {
        if ($fieldable_panels_pane->fpid != $row->migrate_map_destid1) {
          throw new MigrateException(t("Incoming fpid !fpid and map destination fpid !destid1 don't match",
              array('!fpid' => $fieldable_panels_pane->fpid, '!destid1' => $row->migrate_map_destid1)));
        }
      }
      else {
        $fieldable_panels_pane->fpid = $row->migrate_map_destid1;
      }
      // Get the existing vid, tnid so updates don't generate notices.
      $values = db_select('fieldable_panels_panes', 'fpp')
        ->fields('fpp', array('vid'))
        ->condition('fpid', $fieldable_panels_pane->fpid)
        ->execute()
        ->fetchAssoc();
      if (empty($values)) {
        throw new MigrateException(t("Incoming fieldable panels pane ID !fpid no longer exists",
          array('!fpid' => $fieldable_panels_pane->fpid)));
      }
      $fieldable_panels_pane->vid = $values['vid'];
    }
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      if (!isset($fieldable_panels_pane->fpid)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but no destination fpid provided'));
      }
      $old_fieldable_panels_panes = entity_load_single('fieldable_panels_panes', $fieldable_panels_pane->fpid);
      if (empty($old_fieldable_panels_panes)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but fieldable panels pane !fpid does not exist',
          array('!fpid' => $fieldable_panels_pane->fpid)));
      }
      if (!isset($fieldable_panels_pane->created)) {
        $fieldable_panels_pane->created = $old_fieldable_panels_panes->created;
      }
      if (!isset($fieldable_panels_pane->vid)) {
        $fieldable_panels_pane->vid = $old_fieldable_panels_panes->vid;
      }
    }

    if (!isset($fieldable_panels_pane->bundle)) {
      // Default the type to our designated destination bundle (by doing this
      // conditionally, we permit some flexibility in terms of implementing
      // migrations which can affect more than one type).
      $fieldable_panels_pane->bundle = $this->bundle;
    }

    // Set some required properties.
    if ($migration->getSystemOfRecord() == Migration::SOURCE) {
      if (empty($fieldable_panels_pane->language)) {
        $fieldable_panels_pane->language = $this->language;
      }
      if (empty($fieldable_panels_pane->created)) {
        $fieldable_panels_pane->created = REQUEST_TIME;
      }
      if (empty($fieldable_panels_pane->updated)) {
        $fieldable_panels_pane->updated = $fieldable_panels_pane->created;
      }
    }

    // Invoke migration prepare handlers.
    $this->prepare($fieldable_panels_pane, $row);

    if (!isset($fieldable_panels_pane->revision)) {
      // Saves disk space and writes. Can be overridden.
      $fieldable_panels_pane->revision = 0;
    }

    // Trying to update an existing fieldable panels panes.
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      // Incoming data overrides existing data, so only copy non-existent
      // fields.
      if (!empty($old_fieldable_panels_panes)) {
        foreach ($old_fieldable_panels_panes as $field => $value) {
          // An explicit NULL in the source data means to wipe to old value
          // (i.e. don't copy it over from $old_fieldable_panels_panes).
          if (property_exists($fieldable_panels_pane, $field) && $fieldable_panels_pane->$field === NULL) {
            // Ignore this field.
          }
          elseif (!isset($fieldable_panels_pane->$field)) {
            $fieldable_panels_pane->$field = $old_fieldable_panels_panes->$field;
          }
        }
      }
    }

    if (isset($fieldable_panels_pane->fpid)
        && !(isset($fieldable_panels_pane->is_new)
        && $fieldable_panels_pane->is_new)) {
      $updating = TRUE;
    }
    else {
      $updating = FALSE;
    }

    migrate_instrument_start('fieldable_panels_panes_save');
    entity_save('fieldable_panels_pane', $fieldable_panels_pane);
    migrate_instrument_stop('fieldable_panels_panes_save');

    if (isset($fieldable_panels_pane->fpid)) {
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }

      // Unfortunately, http://drupal.org/node/722688 was not accepted, so fix
      // the changed timestamp.
      if (isset($changed)) {
        db_update('fieldable_panels_panes')
            ->fields(array('changed' => $changed))
            ->condition('fpid', $fieldable_panels_pane->fpid)
            ->execute();
        $fieldable_panels_pane->changed = $changed;
      }

      // Potentially fix timestamp in fieldable_panels_panes_revision.
      $query = db_update('fieldable_panels_panes_revision')
          ->condition('vid', $fieldable_panels_pane->vid);
      if (isset($changed)) {
        $fields['timestamp'] = $changed;
      }
      if (!empty($fields)) {
        // We actually have something to update.
        $query->fields($fields);
        $query->execute();
        if (isset($changed)) {
          $fieldable_panels_pane->timestamp = $changed;
        }
      }
      $return = array($fieldable_panels_pane->fpid);
    }
    else {
      $return = FALSE;
    }

    $this->complete($fieldable_panels_pane, $row);
    return $return;
  }
}
