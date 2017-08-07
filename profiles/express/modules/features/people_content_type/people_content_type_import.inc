<?php

/**
 * @file
 * Contains people.inc.
 *
 * More people!
 */

class PeopleMigration extends Migration {
  /**
   * People constructor.
   *
   * {@inheritdoc}
   */
  public function __construct() {
    parent::__construct();
    ini_set('auto_detect_line_endings', TRUE);
    // id,First,Last,Title,Address,Email,Phone,Job
    // Type,Department,Website,Biography,Interests,Extra,Photo.
    $columns = array(
      0 => array('id_csv', 'Id'),
      1 => array('first_csv', 'First Name'),
      2 => array('last_csv', 'Last Name'),
      3 => array('title_csv', 'Title'),
      4 => array('job_type_csv', 'Job Type'),
      5 => array('department_csv', 'Department'),
      6 => array('email_csv', 'Email'),
      7 => array('phone_csv', 'Phone'),
      8  => array('website_csv', 'Web Site'),
      9  => array('website_title_csv', 'Web Site Title'),
      10 => array('address_csv', 'Office Address'),
      11 => array('office_hours_csv', 'Office Hours'),
      12  => array('body_csv', 'body'),
      13  => array('photo_csv', 'Photo'),
      14  => array('status_csv', 'Status'),
    );

    // The Description of the import.
    // This desription is shown on the Migrate GUI.
    $this->description = t('Import of people content.');
    // The Source of the import.
    $files_dir = 'public://' . (variable_get('csvimportpeople', 'csvimportpeople'));
    $wrapper = file_stream_wrapper_get_instance_by_uri($files_dir);
    $path = $wrapper->getDirectoryPath() . "/" . file_uri_target($files_dir);
    $this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' .
                    $path . '/people.csv',
                    $columns, array('delimiter' => ',', 'header_rows' => 1));
    // The destination CCK (boundle).
    $this->destination = new MigrateDestinationNode('person');

    // Source and destination relation for rollbacks.
    $this->map = new MigrateSQLMap(
      $this->machineName,
      array(
        'id_csv' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'alias' => 'import')),
      MigrateDestinationNode::getKeySchema()
    );
    $format_arguments = MigrateTextFieldHandler::arguments(
      NULL,
      'wysiwyg',
      NULL);
    $this->addFieldMapping('uid')->defaultValue('1');
    $this->addFieldMapping('language')->defaultValue('und');
    $this->addFieldMapping('field_person_first_name', 'first_csv');
    $this->addFieldMapping('field_person_last_name', 'last_csv');
    $this->addFieldMapping('field_person_job_type', 'job_type_csv')->separator(',');
    $this->addFieldMapping('field_person_job_type:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_person_title', 'title_csv');
    $this->addFieldMapping('field_person_department', 'department_csv');
    $this->addFieldMapping('field_person_department:create_term')->defaultValue(TRUE);
    $this->addFieldMapping('field_person_email', 'email_csv');
    $this->addFieldMapping('field_person_phone', 'phone_csv');
    $this->addFieldMapping('field_person_website', 'website_csv');
    $this->addFieldMapping('field_person_website:title', 'website_title_csv');
    $this->addFieldMapping('field_person_website:attributes')->defaultValue('TARGET=_blank');
    $this->addFieldMapping('field_person_address', 'address_csv');
    $this->addFieldMapping('field_person_address:format')->defaultValue('plain_text');
    $this->addFieldMapping('field_person_office_hours', 'office_hours_csv');
    $this->addFieldMapping('field_person_office_hours:format')->defaultValue('plain_text');
    $this->addFieldMapping('body', 'body_csv');
    $this->addFieldMapping('body:format')->defaultValue('wysiwyg');
    $this->addFieldMapping('field_person_photo', 'photo_csv');
    $this->addFieldMapping('field_person_photo:source_dir')
      ->defaultValue("$path/mugshots");
    $this->addFieldMapping('field_person_photo:alt', 'last_csv');
    $this->addFieldMapping('field_person_photo:title', 'last_csv');
    $this->addFieldMapping('status', 'status_csv')->defaultValue('1');
  }
}
