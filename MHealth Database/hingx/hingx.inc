<?php
/**
  @file
  @brief Defines the functions that comprise the HingX API.
*/

/// The HingX root URL.
define ('hingx_URL', 'http://hingx.org/Api');

/**
  @brief Sends an HTTP Get request to the given URL.
  @param $url (string) the URL.
  @return (string) the response or false on failure.
*/
function _hingx_get ($url) {
  watchdog ('hingx', "[_hingx_get] url: $url");

  $curl = curl_init ($url);
  curl_setopt ($curl, CURLOPT_RETURNTRANSFER, 1);
  $response = curl_exec ($curl);

  if (!$response) {
    watchdog ('hingx', '[_hingx_get] Error: an error occured while trying to connect to @url. @message',
      array (
        '@url' => $url,
        '@message' => curl_error ($curl)
      ), WATCHDOG_ERROR
    );
  }
  curl_close ($curl);
  return $response;
}

/**
  @brief Executes a search query against the HingX
    resource database and returns a JSON string
    representing the response.
  @param $terms (string) the search terms.
  @param $page (integer) the page index.
  @param $num (integer) the maximum number of
    search results per page.
  @return (string) the response or false on failure.
*/
function _hingx_search ($terms = '', $page = 1, $num = 100) {
  watchdog ('hingx', '[_hingx_search] terms: @terms page: @page num: @num', array ('@terms' => $terms, '@page' => $page, '@num' => $num));
  return _hingx_get (hingx_URL . '/Resource/Find?searchString=' . rawurlencode ($terms) . "&page=$page&maxItemsPerPage=$num");
}

/**
  @brief Executes a search query against the HingX
    resource database and returns the results.
  @param $terms (string) the search terms.
  @param $num (integer) the maximum number of search results.
  @return (string) the resource ids or null on failure.
*/
function hingx_search ($terms = '', $num = 100) {
  watchdog ('hingx', '[hingx_search] terms: @terms num: @num.', array ('@terms' => $terms, '@num' => $num));

  $json = _hingx_search ($terms, 1, $num);
  watchdog ('hingx', '[hingx_search] json: @json', array ('@json' => $json));
  if (!$json) { return null; }

  $response = json_decode ($json);
  watchdog ('hingx', '[hingx_search] response: @response', array ('@response' => print_r ($response, true)));

  $ids = array ();
  $resources = $response->Data;
  for ($i = 0; $i < min ($num, count ($resources)); $i ++) {
    $resource = $resources [$i];
    $ids[] = $resource->ResourceID;
  }

  watchdog ('hingx', '[hingx_search] ids: @ids', array ('@ids' => print_r ($ids, true)));
  return $ids;
}

/**
  @brief Retrieves the HingX resource that has the
    given id and returns a JSON string representing
    the response.
  @param $id (integer) the resource id.
  @return (string) the response or false on failure.
*/
function _hingx_retrieve ($id) {
  return _hingx_get (hingx_URL . "/Resource/GetDetails?resourceIDs=$id");
}

/**
  @brief Retrieves the HingX resource that has the given id.
  @param $id (integer) the resource id.
  @return (object) the resource or null on failure.
*/
function hingx_retrieve ($id) {
  $json = _hingx_retrieve ($id);
  if (!$json) { return null; }

  $response = json_decode ($json);
  if (!is_array ($response)) { return null; }

  $response = array_shift ($response);
  return $response->Data;
}
