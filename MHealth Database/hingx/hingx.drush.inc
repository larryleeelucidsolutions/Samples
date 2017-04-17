<?php
/**
  @file
  @brief Defines the Drush commands for the HingX module.
*/

/**
  @brief Executes a search query against the HingX
    resource database.
  @param $terms (string) the search terms.
  @param $num (integer) the maximum number of search results.
  @return (string)
*/
function drush_hingx_search ($terms = '', $num = 100) {
  module_load_include ('inc', 'hingx');
  $response = hingx_search ($terms, $num);
  watchdog ('hingx', '[drush_hingx_search] response: <pre>' . print_r ($response, true) . '</pre>');
  return print_r ($response, true);
}

/**
  @brief Retrieves the HingX resource that has the given id.
  @param $id (integer) the resource id.
  @return (string) the response.
*/
function drush_hingx_retrieve ($id) {
  module_load_include ('inc', 'hingx');
  $response = hingx_retrieve ($id);
  watchdog ('hingx', '[drush_hingx_retrieve] response <pre>' . print_r ($response, true) . '</pre>');
  return $response;
}

/// Implements hook_drush_command.
function hingx_drush_command () {
  return array (
    'hingx-search' => array (
      'description'        => 'Executes a search query against the HingX resource database.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_hingx_search',
      'callback arguments' => array (),
      'arguments'          => array (
        'terms' => 'The search terms.',
        'num'   => 'The maximum number of search results.'
      ),
      'examples'           => array (
        'hingx-search "mhealth"' => 'Retrieves a list of HingX resources related to mhealth.'
      )
    ),
    'hingx-retrieve' => array (
      'description'        => 'Retrieves the HingX resource that has the given id.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_hingx_retrieve',
      'callback arguments' => array (),
      'arguments'          => array (
        'id' => 'The resource id.'
      ),
      'examples'           => array (
        'hingx-retrieve "1225"' => 'Retrieves resource 1225 from the HingX resource database.'
      )
    )
  );
}
