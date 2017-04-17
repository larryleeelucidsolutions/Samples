<?php
/**
  @file
  @brief This file defines the Google Scholar
    Search API. This API can be used to search
    Google Scholar for citations.
*/

/// The Google Scholar root URL.
define ('google_scholar_search_URL', 'http://scholar.google.com/scholar');

/// The default cookie file path.
define ('google_scholar_search_cookie_file', './google_scholar_cookie.txt');

/**
  @brief Sends an HTTP GET request to the given URL and returns the response.
  @param $url (string) the URL.
  @param $cookie_file (string) a path to a cookie file.
  @return (string) the response or false on failure.
*/
function _google_scholar_search_get ($url, $cookie_file = null) {
  $curl = curl_init ($url);
  curl_setopt ($curl, CURLOPT_RETURNTRANSFER, 1);
  if (!$cookie_file) {
    curl_setopt ($curl, CURLOPT_COOKIEJAR, $cookie_file);
    curl_setopt ($curl, CURLOPT_COOKIEFILE, $cookie_file);
  }
  $response = curl_exec ($curl);
  if (!$response) {
    watchdog (
      'google_scholar_search',
      '[_google_scholar_search_get] Error: An error occured while trying to send a GET request to @url. @msg',
      array (
        '@url' => $url,
        '@msg' => curl_error ($curl)
      ),
      WATCHDOG_ERROR
    );
  }
  curl_close ($curl);
  return $response;
}

/**
  @brief Searches Google Scholar for articles that
    contain one or more of the given terms in their
    titles.
  @param $terms (string) the search terms.
  @param $start (int) the result offset.
  @param $num (int) the number of results requested.
  @return (string) the HTML response or false on
    failure.
*/
function _google_scholar_search ($terms, $start = 0, $num = 10) {
  $url = url (google_scholar_search_URL, array ('query' => array ('q' => 'allintitle:' . $terms, 'start' => $start, 'num' => $num))); 
  return _google_scholar_search_get ($url, google_scholar_search_cookie_file);
}

/**
  @brief Accepts a DOM node and returns its child
    elements that have the given class name.
  @param $node (DOMNode) a DOM node.
  @param $tag (string) the tag name.
  @param $class (string) the class name.
  @return (array) the child nodes.
*/
function _google_scholar_search_getElementsByClassName ($node, $tag, $class) {
  $nodes = array ();
  foreach ($node->getElementsByTagName ($tag) as $child) {
    if ($child->getAttribute ('class') === $class) {
      $nodes[] = $child;
    }
  }
  return $nodes;
}

/**
  @brief Parses an HTML search response and returns
    the search results.
  @param $response (string) the search response.
  @return (array) the search results or null on error.
*/
function _google_scholar_search_parse_response ($response) {
  $dom = @DOMDocument::loadHTML (utf8_encode ($response));
  if (!$dom) {
    watchdog (
      'google_scholar_search',
      '[_google_scholar_search_parse_response] Error: An error occured while trying to parse search results from Google Scholar. The response probably represented an error on Google Scholar.',
      WATCHDOG_ERROR
    );
    return null;
  }
  $results = array ();
  $result_elems = _google_scholar_search_getElementsByClassName ($dom, 'div', 'gs_r');
  foreach ($result_elems as $result_elem) {
    $result = array ();

    $title_elems = _google_scholar_search_getElementsByClassName ($result_elem, 'h3', 'gs_rt');
    if (empty ($title_elems)) { continue; }
    $title_elem = $title_elems [0];

    $title_link_elems = $title_elem->getElementsByTagName ('a');
    if ($title_link_elems->length == 0) { continue; }
    $title_link_elem  = $title_link_elems->item (0);

    $result ['title'] = $title_link_elem->textContent;
    $result ['url'] = $title_link_elem->getAttribute ('href');

    $author_elems = _google_scholar_search_getElementsByClassName ($result_elem, 'div', 'gs_a');
    if (empty ($author_elems)) { continue; }
    $author_elem = $author_elems [0];

    $ellipse      = '\xe2\x80\xa6';
    $author_name  = '\w+(\s\w+)+';
    $author_names = $author_name . '(,\s' . $author_name . ')*';

    if (preg_match ('/(' . $author_names . ')(' . $ellipse . ')?\s-\s((.*),\s)(\d{4})\s-\s(.*)/', $author_elem->textContent, $matches)) {

      $result ['authors'] = array_map ('trim', explode (',', $matches [1]));
      $result ['journal'] = $matches [7];
      $result ['year']    = $matches [8];
    } else {
      watchdog (
        'google_scholar_search',
        '[_google_scholar_search_parse_response] failed match: <pre>@elem</pre>',
        array (
          '@elem' => print_r ($author_elem->textContent, true)
      ));
      continue;
    }

    $snippet_elems = _google_scholar_search_getElementsByClassName ($result_elem, 'div', 'gs_rs');
    if (empty ($snippet_elems)) { continue; }
    $snippet_elem = $snippet_elems [0];

    $result ['snippet'] = $snippet_elem->textContent;

    $results[] = $result;
  }
  return $results;
}

/**
  @brief Executes a search against Google Scholar
    and returns the results.
  @param $terms (string) the search terms.
  @param $start (int) the result offset.
  @param $num (int) the number of results requested.
  @return (array) the search results or null on error.
*/
function google_scholar_search ($terms, $start = 0, $num = 10) {
  $response = _google_scholar_search ($terms, $start, $num);
  if (!$response) { return null; }
  return _google_scholar_search_parse_response ($response);
}
