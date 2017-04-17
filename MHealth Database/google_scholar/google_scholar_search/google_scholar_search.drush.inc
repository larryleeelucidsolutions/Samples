<?php
/**
  @file
  @brief Defines the Drush commands for the Google
    Scholar Search module.
*/

/**
  @brief Searches Google Scholar for articles
    containing the given terms and returns the
    results.
  @param $terms (string) the search terms.
  @param $start (int) the result offset.
  @param $num (int) the number of results requested.
  @return (array) the results or an error string.
*/
function drush_google_scholar_search ($terms, $start = 0, $num = 10) {
  module_load_include ('inc', 'google_scholar_search');
  $results = google_scholar_search ($terms, $start, $num);
  if (is_null ($results)) {
    return "An error occured while trying to execute the search.";
  }
  return print_r ($results, true);
}

/// Implements hook_drush_command.
function google_scholar_search_drush_command () {
  return array (
    'google-scholar-search' => array (
      'description'        => 'Searches Google Scholar for articles containing the given search terms.',
      'bootstrap'          => DRUSH_BOOTSTRAP_DRUPAL_FULL,
      'callback'           => 'drush_google_scholar_search',
      'callback arguments' => array (),
      'arguments'          => array (
        'terms' => 'The search terms.',
        'start' => 'The result offset.',
        'num'   => 'The number of results requested.'
      ),
      'examples'           => array (
        'google-scholar-search "cardiovascular infarcation" 10 15' => 'Executes a search for "cardiovascular infarcation" against Google Scholar, retrieves the results, and returns the 15 results after the first 10.'
      )
  ));
}
