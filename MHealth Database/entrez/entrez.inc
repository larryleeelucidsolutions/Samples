<?php
/**
  @file
  @brief Defines the functions that comprise the Entrez API.
*/

/// The E-utilities root URL.
define ('entrez_EUTILS_URL', 'http://eutils.ncbi.nlm.nih.gov/entrez/eutils');

/**
  @brief Sends an HTTP GET request to the given URL and
    returns the response.
  @param $url (string) the URL.
  @return (string) the response or false on failure.
*/
function _entrez_get ($url) {
  $curl = curl_init ($url);
  curl_setopt ($curl, CURLOPT_RETURNTRANSFER, 1);
  $response = curl_exec ($curl);

  if (!$response) {
    watchdog ('entrez', '[_entrez_get] Error: an error occured while trying to connect to @url. @message',
      array (
        '@url'     => $url,
        '@message' => curl_error ($curl)
      ), WATCHDOG_ERROR
    );
  } 
  curl_close ($curl);
  return $response;
}

/**
  @brief Executes a search query against an Entrez
    database and returns an XML document listing
    the results.
  @param $terms (string) the search terms.
  @param $db (string) the Entrez database name.
  @param $start (int) the start offset.
  @param $num (int) the maximum number of records
    to return.
  @return (string) the XML document or false on
    failure.
*/
function _entrez_search ($terms = '', $db = 'pubmed', $start = null, $num = null) {
  $url = entrez_EUTILS_URL . '/esearch.fcgi?term=' . rawurlencode ($terms) . '&db=' . rawurlencode ($db);
  if (isset ($start)) { $url .= "&retstart=$start"; }
  if (isset ($num))   { $url .= "&retmax=$num"; }

  return _entrez_get ($url);
}

/**
  @brief Parses an XML document that represents a
    Entrez search query response and returns the
    results.
  @param $xml (string) the XML document. 
  @return (array) the record ids.
*/
function _entrez_parse_search_response ($xml) {
  $dom = DOMDocument::loadXML ($xml);
  if (!$dom) {
    watchdog ('entrez', '[_entrez_parse_search_response] Error: an error occured while trying to parse an Entrez database search response.', array (), WATCHDOG_ERROR);
    return null;
  }

  $ids = array ();
  foreach ($dom->getElementsByTagName ('Id') as $elem) {
    $ids [] = $elem->nodeValue;
  }
  return $ids;
}

/**
  @brief Executes a search query against an Entrez
    database and returns the results.
  @param $terms (string) the search terms.
  @param $db (string) the Entrez database name.
  @param $start (int) the start offset.
  @param $num (int) the maximum number of records
    to return.
  @return (array) the record ids or null on failure.
*/
function entrez_search ($terms = '', $db = 'pubmed', $start = null, $num = null) {
  $xml = _entrez_search ($terms, $db, $start, $num);
  return $xml ? _entrez_parse_search_response ($xml) : null;
}

/**
  @brief Retrieves the Entrez database records that
    have the given ids.
  @param $ids (array) the record ids.
  @param $db (string) the Entrez database name.
  @return (string) an XML document that represents
    the response or false on failure.
*/
function _entrez_fetch ($ids, $db = 'pubmed') {
  return _entrez_get (entrez_EUTILS_URL . '/efetch.fcgi?id=' . implode (',', $ids) . "&db=$db&retmode=xml");
}

/**
  @brief Returns the element that has the given tag name.
  @param $dom (DOMElement) the root element.
  @param $name (string) the tag name.
  @return (DOMElement) the child element or null if
    none of the child elements have the given name.
*/
function _entrez_getElementByName ($dom, $name) {
  $elems = $dom->getElementsByTagName ($name);
  return $elems->length > 0 ? $elems->item (0) : null;
}

/**
  @brief Returns the node value of the element that
    has the given tag name.
  @param $dom (DOMElement) the root element.
  @param $name (string) the tag name.
  @return (string) the node value or null if none
    of the elements have the given name.
*/
function _entrez_getNodeValueByName ($dom, $name) {
  $elem = _entrez_getElementByName ($dom, $name);
  return $elem ? $elem->nodeValue : null;
}

/**
  @brief Parses an XML element that represents a
    PubMed record and returns the result.
  @param $dom (DOMElement) the XML element.
  @return (array) the record or null on failure. 
*/
function _entrez_parse_pubmed_record ($dom) {
  $emsg = '[_entrez_parse_pubmed_record] Error: an error occured while trying to parse a PubMed record.';

  $record = array ();

  foreach ($dom->getElementsByTagName ('ArticleId') as $articleIdElem) {
    if ($articleIdElem->getAttribute ('IdType') === 'pubmed') {
      $record ['id'] = $articleIdElem->nodeValue;
    } else if ($articleIdElem->getAttribute ('IdType') === 'doi') {
      $record ['doi'] = $articleIdElem->nodeValue;
    }
  }
  if (!isset ($record ['id'])) {
    watchdog ('entrez', $emsg . ' The PubMed id value is missing. (Should be given in an ArticleId element).', array (), WATCHDOG_ERROR);
    return null;
  }

  $record ['title'] = _entrez_getNodeValueByName ($dom, 'ArticleTitle');
  if (!isset ($record ['title'])) {
    watchdog ('entrez', $emsg . ' The ArticleTitle element is missing (or empty).', array (), WATCHDOG_ERROR);
    return null;
  }

  $record ['abstract'] = _entrez_getNodeValueByName ($dom, 'AbstractText');

  $record ['terms'] = array ();
  foreach ($dom->getElementsByTagName ('MeshHeading') as $meshHeadingElem) {
    $descriptorNameElem = _entrez_getElementByName ($meshHeadingElem, 'DescriptorName');
    if (!$descriptorNameElem) {
      watchdog ('entrez', $emsg . ' One of the MeshHeading elements is missing a DescriptorName element.', array (), WATCHDOG_ERROR);
      return null;
    }

    $qualifiers = array ();
    foreach ($meshHeadingElem->getElementsByTagName ('QualifierName') as $qualifierNameElem) {
      $qualifiers [] = $qualifierNameElem->nodeValue;
    }

    $record ['terms'][] = array ('descriptor' => $descriptorNameElem->nodeValue,
                                 'qualifiers' => $qualifiers);
  }

  $record ['language'] = _entrez_getNodeValueByName ($dom, 'Language');

  foreach ($dom->getElementsByTagName ('DateCreated') as $dateElem) {
    $record ['publication_date'] = array (
      'year'  => _entrez_getNodeValueByName ($dateElem, 'Year'),
      'month' => _entrez_getNodeValueByName ($dateElem, 'Month'),
      'day'   => _entrez_getNodeValueByName ($dateElem, 'Day')
    );
  }

  $record ['authors'] = array ();
  foreach ($dom->getElementsByTagName ('Author') as $authorElem) {
    $record ['authors'][] = array (
      'last_name' => _entrez_getNodeValueByName ($authorElem, 'LastName'),
      'fore_name' => _entrez_getNodeValueByName ($authorElem, 'ForeName'),
      'initials'  => _entrez_getNodeValueByName ($authorElem, 'Initials')
    );
  }

  $journalElem = _entrez_getElementByName ($dom, 'Journal');
  if ($journalElem) {
    $dateElem = _entrez_getElementByName ($journalElem, 'PubDate');
    $record ['journal'] = array (
      'title'  => _entrez_getNodeValueByName ($journalElem, 'Title'),
      'volume' => _entrez_getNodeValueByName ($journalElem, 'Volume'),
      'date'   => array (
        'year'  => _entrez_getNodeValueByName ($dateElem, 'Year'),
        'month' => _entrez_getNodeValueByName ($dateElem, 'Month')
    ));
  }

  return $record; 
}

/**
  @brief Parses an XML document that represents an
    Entrez fetch response and returns the results.
  @param $dom (DOMDocument) the XML document.
  @return (array) the records.
*/
function _entrez_parse_pubmed_fetch_response ($dom) {
  $records = array ();
  foreach ($dom->getElementsByTagName ('PubmedArticle') as $elem) {
    $records [] = _entrez_parse_pubmed_record ($elem);
  }
  return $records;
}

/**
  @brief Retrieves the Entrez database records that
    have the given ids.
  @param $ids (array) the record ids.
  @param $db (string) the Entrez database name.
  @return (array) the records or null on failure.
*/
function entrez_fetch ($ids, $db = 'pubmed') {
  $emsg = '[entrez_fetch] Error: an error occured while trying to fetch @db database records.';
  if (empty ($ids)) {
    return array ();
  }

  $xml = _entrez_fetch ($ids, $db);
  if (!$xml) {
    watchdog ('entrez', $emsg, array ('@db' => $db), WATCHDOG_ERROR);
    return null;
  }
  watchdog ('entrez', '[entrez_fetch] xml: <pre>' . htmlspecialchars ($xml) . '</pre>');

  $dom = DOMDocument::loadXML ($xml);
  if (!$dom) {
    watchdog ('entrez', $emsg . ' An error occured while trying to parse the XML response.', array ('@db' => $db), WATCHDOG_ERROR);
    return null;
  }

  switch ($db) {
    case 'pubmed':
      return _entrez_parse_pubmed_fetch_response ($dom);
    default:
      watchdog ('entrez', $emsg . ' Unrecognized database name.', array ('@db' => $db), WATCHDOG_ERROR
      );
  }
  return null;
}

/**
  @brief Retrieves the links associated with the
    given Entrez database records.
  @param $ids (array) the record ids.
  @param $from (string) the Entrez database that
    the links reference from.
  @param $to (string) the Entrez database that the
    links reference to.
  @param $cmd (string) the Elink command mode.
  @return (string) the response document or false
    on failure.
*/
function _entrez_links ($ids, $from = null, $to = null, $cmd = null) {
  $url = entrez_EUTILS_URL . '/elink.fcgi?id=' . implode (',', $ids) . '&retmode=xml';
  if ($from) { $url .= "&dbfrom=$from"; }
  if ($to)   { $url .= "&db=$to"; }
  if ($cmd)  { $url .= "&cmd=$cmd"; }

  return _entrez_get ($url);
}

/**
  @brief Retrieves the links associated with the
    given Entrez database records.
  @param $ids (array) the record ids.
  @param $from (string) the Entrez database that
    the links reference from.
  @param $to (string) the Entrez database that the
    links reference to.
  @param $cmd (string) the Elink command mode.
  @return (array) the links or null on failure.
*/
function entrez_links ($ids, $from = null, $to = null, $cmd = null) {
  $xml = _entrez_links ($ids, $from, $to, $cmd);
  if (!$xml) {
    watchdog (
      'entrez',
      '[entrez_links] Error: an error occured while trying to retrieve links for the following entrez records <pre>@ids</pre>',
      array ('@ids' => print_r ($ids, true)),
      WATCHDOG_ERROR
    );
    return null;
  }
     
  $dom = DOMDocument::loadXML ($xml);
  if (!$dom) {
    watchdog (
      'entrez',
      '[entrez_links] Error: an error occured while trying to retrieve links for the following entrez records <pre>@ids</pre>',
      array ('@ids' => print_r ($ids, true)),
      WATCHDOG_ERROR
    );
    return null;
  }

  $linkSets = array ();
  foreach ($dom->getElementsByTagName ('IdUrlSet') as $setElem) {
    $linkSets [] = array (
      'id' => _entrez_getNodeValueByName ($setElem, 'Id'),
      'links' => array ()
    );

    foreach ($setElem->getElementsByTagName ('ObjUrl') as $elem) {
      $linkSet ['links'][]  = array (
        'url'      => _entrez_getNodeValueByName ($elem, 'Url'),
        'category' => _entrez_getNodeValueByName ($elem, 'Category')
      );
    }
  }
  return $linkSets;
}
