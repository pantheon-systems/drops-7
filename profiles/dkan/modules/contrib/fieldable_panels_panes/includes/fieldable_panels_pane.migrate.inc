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
      $old_fieldable_panels_panes = entity_load_single('fieldable_panels_pane', $fieldable_panels_pane->fpid);
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

/**
 * Handling specific to a Drupal 7 source for Fieldable Panels Panes.
 */
class DrupalFieldablePanelsPanesMigration extends DrupalMigration {

  /**
   * The source and destination content types (bundles) we're dealing with.
   */
  protected $destinationType;

  /**
   * Default language to apply to the node and it's body field.
   *
   * @var string
   */
  protected $defaultLanguage = LANGUAGE_NONE;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $arguments) {
    $this->destinationType = $arguments['destination_type'];
    $this->sourceType = $arguments['source_type'];
    if (!empty($arguments['default_language'])) {
      $this->defaultLanguage = $arguments['default_language'];
    }
    $arguments['source_version'] = 7;
    parent::__construct($arguments);

    // Document known core fields.
    $this->sourceFields += array(
      'fpid' => t('The primary identifier for the entity.'),
      'vid' => t('The current version in use for this entity.'),
      'bundle' => t('The bundle of the entity.'),
      'title' => t('The title of the entity.'),
      'link' => t('Whether or not this entity title will link to another page.'),
      'path' => t('The path the title should link to.'),
      'reusable' => t('Whether or not this entity will appear in the Add Content dialog.'),
      'admin_title' => t('The title it will appear in the Add Content dialog as.'),
      'admin_description' => t('The description it will appear in the Add Content dialog with.'),
      'category' => t('The category it will appear in the Add Content dialog under.'),
      'view_access' => t('Access rules to describe if the user has view access to this entity.'),
      'edit_access' => t('Access rules to describe if the user has view access to this entity.'),
      'created' => t('The Unix timestamp when the entity was created.'),
      'changed' => t('The Unix timestamp when the entity was most recently saved.'),
      'uuid' => t('The Universally Unique Identifier.'),
      'language' => t('The languages.language of this entity.'),
      'uid' => t('The users.uid of the current revision.'),
      'log' => t('The log message of the current revision.'),
      'vuuid' => t('The Universally Unique Identifier of the current revision.'),
    );

    $this->sourceFields += $this->version->getSourceFields('fieldable_panels_pane', $this->sourceType);

    $this->source = new MigrateSourceSQL($this->query(), $this->sourceFields, NULL, $this->sourceOptions);

    $this->destination = new MigrateDestinationFieldablePanelsPanes($this->destinationType);

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'fpid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'Source entity ID',
          'alias' => 'fpp',
        ),
      ),
      MigrateDestinationNode::getKeySchema(),
      $this->mapConnection
    );

    if (!$this->newOnly) {
      $this->highwaterField = array(
        'name' => 'changed',
        'alias' => 'fpp',
        'type' => 'int',
      );
    }

    // Setup common mappings.
    $this->addSimpleMappings(array(
      'title',
      'link',
      'path',
      'reusable',
      'admin_title',
      'admin_description',
      'category',
      'view_access',
      'edit_access',
      'created',
      'changed',
      'uuid',
      'language',
    ));

    $this->addUnmigratedSources(array('vid'));
    $this->addUnmigratedDestinations(array());

    if (isset($arguments['default_uid'])) {
      $default_uid = $arguments['default_uid'];
    }
    else {
      $default_uid = 1;
    }
    /*
    if (isset($user_migration)) {
      $this->addFieldMapping('uid', 'uid')
        ->sourceMigration($user_migration)
        ->defaultValue($default_uid);
    }
    else {
      $this->addFieldMapping('uid', NULL, FALSE)
        ->defaultValue($default_uid);
    }
    */
  }

  /**
   * {@inheritdoc}
   */
  protected function query() {
    $query = Database::getConnection('default', $this->sourceConnection)
      ->select('fieldable_panels_panes', 'fpp')
      ->fields('fpp', array(
        'fpid',
        'vid',
        'title',
        'link',
        'path',
        'reusable',
        'admin_title',
        'admin_description',
        'category',
        'view_access',
        'edit_access',
        'created',
        'changed',
        'uuid',
        'language',
      ));
    $query->join('fieldable_panels_panes_revision', 'fppv',
      'fppv.fpid = fpp.fpid and fppv.vid = fpp.vid');
    $query->fields('fppv', array(
      'uid',
      'log',
      'vuuid',
    ));
    $query->condition('fpp.bundle', $this->sourceType);
    $query->orderBy($this->newOnly ? 'fpp.fpid' : 'fpp.changed');
    return $query;
  }

  /**
   * {@inheritdoc}
   */
  public function prepareRow($row) {
    if (parent::prepareRow($row) === FALSE) {
      return FALSE;
    }

    $this->version->getSourceValues($row, $row->fpid);

    return TRUE;
  }

  /**
   * Implementation of Migration::createStub().
   */
  protected function createStub($migration) {
    migrate_instrument_start('DrupalNodeMigration::createStub');
    $entity = new stdClass();
    $entity->title = t('Stub');
    $entity->type = $this->destination->getBundle();
    fieldable_panels_panes_save($entity);
    migrate_instrument_stop('DrupalFieldablePanelsPanesMigration::createStub');
    if (isset($entity->fpid)) {
      return array($entity->fpid);
    }
    else {
      return FALSE;
    }
  }

}
