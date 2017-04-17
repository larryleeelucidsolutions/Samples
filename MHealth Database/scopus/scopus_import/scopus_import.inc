<?php
/**
  @file
  @brief The Scopus module defines an API for
    importing Scopus articles. This file defines
    the scopus_import_record function which
    creates/updates Scopus Article nodes using a
    generic Scopus record array.
*/

/**
  @brief Creates/Updates a Scopus Article node that
    represents the given Scopus Article record.
  @param $record (array) the record.
  @return (boolean) true iff no errors were
    detected while importing.
*/
function scopus_import_record ($record) {
  module_load_include ('field.inc', 'scopus');
  module_load_include ('inc', 'scopus');

  $nids = scopus_get_nodes_by_title ($record ['title']);
  if (empty ($nids)) {
    watchdog ('scopus', '[scopus_import] creating a new record.');

    $node = new stdClass ();
    $node->type = scopus_TYPE;
    node_object_prepare ($node);
  } else {
    watchdog ('scopus', '[scopus_import] updating an existing record.');
    $node = node_load (array_shift ($nids));
  }

  $node->title = $record ['title'];

  $node->scopus_author = array ('und' => array ());
  foreach ((array) $record ['authors'] as $author) {
    foreach ((array) explode (',', $author) as $author_name) {
      $node->scopus_author ['und'][] = scopus_create_term_reference_value ($author_name, 'scopus_author');
    }
  }

  $node->scopus_year = array (
    'und' => array (
      scopus_create_term_reference_value ($record ['year'], 'scopus_year')
  ));
  
  $node->scopus_source = array (
    'und' => array (
      scopus_create_term_reference_value ($record ['source'], 'scopus_source')
  ));

  $node->body                  = array ('und' => array (scopus_create_text_field_value ($record ['abstract'])));
  $node->scopus_volume         = array ('und' => array (scopus_create_text_field_value ($record ['volume'])));
  $node->scopus_issue          = array ('und' => array (scopus_create_text_field_value ($record ['issue'])));
  $node->scopus_article_number = array ('und' => array (scopus_create_text_field_value ($record ['article_number'])));
  $node->scopus_page_start     = array ('und' => array (scopus_create_text_field_value ($record ['page_start'])));
  $node->scopus_page_end       = array ('und' => array (scopus_create_text_field_value ($record ['page_end'])));
  $node->scopus_page_count     = array ('und' => array (scopus_create_text_field_value ($record ['page_count'])));
  $node->scopus_cited_by       = array ('und' => array (scopus_create_text_field_value ($record ['cited_by'])));
  $node->scopus_link           = array ('und' => array (scopus_create_text_field_value ($record ['link'])));
  $node->scopus_document_type  = array ('und' => array (scopus_create_text_field_value ($record ['document_type'])));

  node_save ($node);
  watchdog (
    'scopus',
    '[scopus_import] record: <pre>@record</pre> node: <pre>@node</pre>.',
    array (
      '@record' => print_r ($record, true),
      '@node'   => print_r ($node, true)
  ));
  return true;
}

/**
  @brief Updates/Creates Scopus Article nodes that
    represent the given Scopus Article records.
  @param $records (array) the records.
  @return (boolean) true iff no errors were
    detected while importing.
*/
function scopus_import_records ($records) {
  foreach ((array) $records as $record) {
    if (!scopus_import_record ($record)) {
      watchdog (
        'scopus',
        '[scopus_import_records] Error: an error occured while trying to import a Scopus Article (@title).',
        array ('@title' => $record ['title']),
        WATCHDOG_ERROR
      );
      return false;
    }
  } 
  return true;
}
