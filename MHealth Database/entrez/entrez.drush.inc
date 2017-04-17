<?php
/**
  @file
  @brief Defines the Drush commands for the Entrez module.
*/

/**
  @brief Executes a search query against an Entrez
    database and returns the result as an XML
    document.
  @param $terms (string) the search terms.
  @param $db (string) the Entrez database name.
  @param $start (int) the start offset.
  @param $num (int) the maximum number of records
    to return.
  @return (string) a string that represents the XML
    document.
*/
function drush_entrez_search ($terms, $db = 'pubmed', $start = null, $num = null) {
  module_load_include ('inc', 'entrez');
  $response = _entrez_search ($terms, $db, $start, $num);
  watchdog ('entrez', '[drush_entrez_search] response: <pre>' . htmlspecialchars ($response) . '</pre>'); 
  return $response;
}

/**
  @brief Retrieves the Entrez database records that
    have the given ids.
  @param $ids (string) a comma delimited list of
    record ids.
  @param $db (string) the Entrez database name.
  @return (array) the records or null on failure.
*/
function drush_entrez_fetch ($ids, $db = 'pubmed') {
  module_load_include ('inc', 'entrez');
  $response = entrez_fetch (explode (',', $ids), $db);
  watchdog ('entrez', '[drush_entrez_fetch] response: <pre>' . print_r ($response, true) . '</pre>'); 
  return $response;
}

/**
  @brief Retrieves the links associated with the
    given Entrez database records.
  @param $ids (string) a comma delimited list of
    record ids.
  @param $from (string) the Entrez database that
    the links reference from.
  @param $to (string) the Entrez database that the
    links reference to.
  @param $cmd (string) the Elink command mode.
  @param $retmode (string) the result format.
  @return (string) the response document or false
    on failure.
*/
function drush_entrez_links ($ids, $from = null, $to = null, $cmd = null, $retmode = 'xml') {
  module_load_include ('inc', 'entrez');
  $response = _entrez_links (explode (',', $ids), $from, $to, $cmd, $retmode);
  watchdog ('entrez', '[drush_entrez_links] response: <pre>' . htmlspecialchars ($response) . '</pre>');
  return $response;
}

/// Implements hook_drush_command.
function entrez_drush_command () {
  return array (
    'entrez-search' => array (
      'description'        => 'Executes a search query against an Entrez database and returns the result as an XML document.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_entrez_search',
      'callback arguments' => array (),
      'arguments'          => array (
        'terms' => 'The search terms.',
        'db'    => 'The Entrez database name. Default "pubmed".',
        'start' => 'The start index. Default "0".',
        'num'   => 'The maximum number of search results. Default "unlimited".'
      ),
      'examples'           => array (
        'entrez-search "cardiovascular infarcation" "pubmed" 0 1' => 'Executes a search for "cardiovascular infarcation" against the PubMed database and retrieves the first matching record.'
      )
    ),
    'entrez-fetch' => array (
      'description'        => 'Retrieves the Entrez database records that have the given ids.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_entrez_fetch',
      'callback arguments' => array (),
      'arguments'          => array (
        'ids' => 'A comma delimited list of record ids.',
        'db'  => 'The Entrez database name. Default "pubmed".'
      ),
      'examples'           => array (
        'entrez-fetch "23479719,23479672" "pubmed"' => 'Retrieves two records from PubMed.'
      )
    ),
    'entrez-links' => array (
      'description'        => 'Retrieves the links associated with the given Entrez database records.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_entrez_links',
      'callback arguments' => array (),
      'arguments'          => array (
        'ids'  => 'A comma delimited list of record ids.',
        'from' => 'The Entrez database that the links reference from.',
        'to'   => 'The Entrez database that the links reference to.'
      ),
      'examples'           => array (
        'entrez-links "23479719" "pubmed" "" "prlinks"' => 'Returns the fulltext links associated with record 23479719.'
      )
    )
  );
}
