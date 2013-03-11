<?php


/**
 * Get the sql dump file. 
 *
 *  Returns a list of sql commands, one command per line.
 *  That makes it easier to import without loading the whole file into memory.
 *  The files are a little harder to read, but human-readability is not a priority
 */
function backup_migrate_backup_db_to_file_mysql($file, $settings) {
  $lines = 0;
  $exclude = !empty($settings->exclude_tables) ? $settings->exclude_tables : array();
  $nodata = !empty($settings->nodata_tables) ? $settings->nodata_tables : array();
  if ($file->open(TRUE)) {
    $file->write(_backup_migrate_get_sql_file_header_mysql());
    $alltables = _backup_migrate_get_tables_mysql();
    foreach ($alltables as $table) {
      if (_backup_migrate_check_timeout()) {
        return FALSE;
      }
      if ($table->name && !isset($exclude[$table->name])) {
        $file->write(_backup_migrate_get_table_structure_sql_mysql($table));
        $lines++;
        if (!in_array($table->name, $nodata)) {
          $lines += _backup_migrate_dump_table_data_sql_to_file($file, $table);
        }
      }
    }
    $file->write(_backup_migrate_get_sql_file_footer_mysql());
    $file->close();
    return $lines;
  }
  else {
    return FALSE;
  }
}

/**
 * Restore the db from a valid backup file.
 */
function backup_migrate_restore_db_from_file_mysql($file, $settings) {
  $num = 0;
  if ($file->open()) {
    // Read one line at a time and run the query.
    while ($line = $file->read()) {
      if (_backup_migrate_check_timeout()) {
        return FALSE;
      }

      $line = trim($line);
      if ($line) {
        // Use the helper instead of the api function to avoid substitution of '{' etc.
        Database::getConnection()->prepare($line)->execute();
        $num++;
      }
    }
    // Close the file with fclose/gzclose.
    $file->close();
  }
  else {
    drupal_set_message(t("Unable to open file %file to restore database", array("%file" => $file->filepath())), 'error');
    $num = FALSE;
  }
  return $num;
}

/**
 * Get the sql for the structure of the given table.
 */
function _backup_migrate_get_table_structure_sql_mysql($table) {
  $out = "";

  $result = db_query("SHOW CREATE TABLE `". $table->name ."`");
  if ($create = db_fetch_array($result)) {
    $out .= "DROP TABLE IF EXISTS `". $table->name ."`;\n";
    $out .= strtr($create['create table'], array("\n" => " ", '"' => '`'));
    if ($table->auto_increment) {
      $out .= " AUTO_INCREMENT=". $table->auto_increment;
    }
    $out .= ";\n";
  }
  return $out;
}

/**
 *  Get the sql to insert the data for a given table
 */
function _backup_migrate_dump_table_data_sql_to_file($file, $table) {
  $lines = 0;
  $data = db_query("SELECT * FROM `". $table->name ."`");
  while ($row = db_fetch_array($data)) {
    $items = array();
    foreach ($row as $key => $value) {
      $items[] = is_null($value) ? "null" : "'". mysql_escape_string($value) ."'";
    }
    if ($items) {
      $file->write("INSERT INTO `". $table->name ."` VALUES (". implode(",", $items) .");\n");
      $lines++;
    }
  }
  return $lines;
}

/**
 * The header for the top of the sql dump file. These commands set the connection
 *  character encoding to help prevent encoding conversion issues.
 */
function _backup_migrate_get_sql_file_header_mysql() {
  return "
/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE=NO_AUTO_VALUE_ON_ZERO */;

SET NAMES utf8;
";
}

/**
 * The footer of the sql dump file.
 */
function _backup_migrate_get_sql_file_footer_mysql() {
  return "

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
";
}

/**
 * Get a list of tables in the db.
 */
function _backup_migrate_get_tables_mysql() {
  $out = array();
  // get auto_increment values and names of all tables
  $tables = db_query("show table status");
  foreach ($tables as $table) {
    $out[$table->name] = $table;
  }
  return $out;
}


/**
 * Get the list of table names.
 */
function _backup_migrate_get_table_names_mysql() {
  $out = array();
  foreach (_backup_migrate_get_tables_mysql() as $key => $table) {
    $out[$key] = $table->name;
  }
  return $out;
}
