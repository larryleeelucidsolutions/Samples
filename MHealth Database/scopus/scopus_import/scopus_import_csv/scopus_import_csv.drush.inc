<?php
/**
  @file
  @brief Defines the Drush commands for the Scopus
    Import CSV module.
*/

/// Imports the Scopus records listed in the given CSV file.
function drush_scopus_import_csv_file ($filename, $offset = 1) {
  module_load_include ('inc', 'scopus_import_csv');
  scopus_import_csv_file ($filename, $offset);
}

/// Implements hook_drush_command.
function scopus_import_csv_drush_command () {
  return array (
    'scopus-import-csv-file' => array (
      'description'        => 'Imports the Scopus records listed in the given CSV file. Note: the file must use the "Abstract Format" export format.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_scopus_import_csv_file',
      'callback arguments' => array (),
      'arguments'          => array (
        'filename' => 'The CSV file path.',
        'offset'   => 'The number of entries to skip.'
      ),
      'examples'            => array (
        'scopus-import-csv-file "/home/llee/scopus.csv" 2' => 'Imports records from /home/llee/scopus.csv and skips the first 2.'
      )
    )
  );
}
