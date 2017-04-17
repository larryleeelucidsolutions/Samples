<?php
/**
  @file
  @brief Defines the mhealthdb_sync Drush command.
*/

/**
  @brief Iterates over all of the sources and
    creates/updates/removes MHealthDB record nodes
    as needed to keep everything in sync.
*/
function drush_mhealthdb_sync_records () {
  return mhealthdb_sync_records ();
}

/// Implements hook_drush_command ().
function mhealthdb_drush_command () {
  return array (
    'mhealthdb-sync-records' => array (
      'description' => 'Iterates over all of the sources and creates/updates/removes MHealthDB record nodes as needed to keep them in sync with Pubmed and Google Scholar sources.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_mhealthdb_sync_records',
      'callback arguments' => array (),
      'arguments'          => array (),
      'examples'           => array (
        'mhealthdb-sync-records' => 'Syncs the MHealthDB records with the source records'
  )));
}
