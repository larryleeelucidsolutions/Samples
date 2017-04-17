<?php
/**
  @file
  @brief Defines functions that create the Scopus
    Article content type and the Scopus
    vocabularies.
*/

/// Specifies the Scopus Article content type name.
define ('scopus_TYPE', 'scopus_article');

/// Defines the Scopus Article content type.
function scopus_content_type () {
  $t = get_t ();
  return node_type_set_defaults (
    array (
      'type'        => scopus_TYPE,
      'name'        => $t ('Scopus Article'),
      'description' => $t ('Articles from the Scopus database.'),
      'help'        => $t ('Articles from the Scopus database.'),
      'has_title'   => true,
      'base'        => 'node_content',
      'custom'      => false,
      'modified'    => true,
      'locked'      => true,
      'module'      => 'scopus'
  ));
}

/// Returns an array listing the Scopus vocabularies.
function scopus_vocabularies () {
  return array (
    'scopus_author' => array (
      'name'         => 'Scopus Author',
      'machine_name' => 'scopus_author',
      'description'  => 'The name of an author of a Scopus article.'
    ),
    'scopus_source' => array (
      'name'         => 'Scopus Source',
      'machine_name' => 'scopus_source',
      'description'  => 'The name of a source for a Scopus article.'
    ),
    'scopus_year' => array (
      'name'         => 'Scopus Year',
      'machine_name' => 'scopus_year',
      'description'  => 'The year in which a Scopus article was published.'
    )
  );
}

/**
  @brief Creates a new vocabulary.
  @param $vocab (array) a vocabulary array.
*/
function _scopus_create_vocabulary ($vocab) {
  if (!taxonomy_vocabulary_machine_name_load ($vocab ['machine_name'])) {
    taxonomy_vocabulary_save ((object) $vocab);
  }
}

/// Creates the Scopus vocabularies.
function scopus_create_vocabularies () {
  foreach (scopus_vocabularies () as $vocab) {
    _scopus_create_vocabulary ($vocab);
  }
}

/// Deletes the Scopus vocabularies.
function scopus_delete_vocabularies () {
  foreach (array_keys (scopus_vocabularies ()) as $machine_name) {
    $vocab = taxonomy_vocabulary_machine_name_load ($machine_name);
    taxonomy_vocabulary_delete ($vocab->vid);
  }
}

/// Creates the Scopus Article content type.
function scopus_create_content_type () {
  module_load_include ('field.inc', 'scopus');

  $type = scopus_content_type ();
  node_type_save ($type);

  node_add_body_field ($type, 'Abstract');
  scopus_create_term_reference ($type, 'scopus_author',         'Author', 'scopus_author');
  scopus_create_term_reference ($type, 'scopus_year',           'Year',   'scopus_year');
  scopus_create_term_reference ($type, 'scopus_source',         'Source', 'scopus_source');
  scopus_create_textfield      ($type, 'scopus_volume',         'Volume');
  scopus_create_textfield      ($type, 'scopus_issue',          'Issue');
  scopus_create_textfield      ($type, 'scopus_article_number', 'Article No.');
  scopus_create_textfield      ($type, 'scopus_page_start',     'Page Start');
  scopus_create_textfield      ($type, 'scopus_page_end',       'Page End');
  scopus_create_textfield      ($type, 'scopus_page_count',     'Page Count');
  scopus_create_textfield      ($type, 'scopus_cited_by',       'No. Citations');
  scopus_create_textfield      ($type, 'scopus_link',           'Link');
  scopus_create_textfield      ($type, 'scopus_document_type',  'Document Type');
}

/**
  @brief Returns the node id of every Scopus
    Article node.
  @return (array) the node ids.
*/
function scopus_get_nodes () {
  $records = db_select   ('{node}', 'tbl')
             ->fields    ('tbl', array ('nid'))
             ->condition ('type', scopus_TYPE)
             ->execute   ();

  $nids = array ();
  foreach ($records as $record) {
    $nids [] = $record->nid;
  }
  return $nids;
}

/**
  @brief Returns the Scopus Article node that has
    the given title.
  @param $title (string) the title.
  @return (integer) the node id.
*/
function scopus_get_nodes_by_title ($title) {
  $records = db_select   ('{node}', 'tbl')
             ->fields    ('tbl', array ('nid'))
             ->condition ('type', scopus_TYPE)
             ->condition ('title', $title)
             ->execute   ();

  $nids = array ();
  foreach ($records as $record) {
    $nids [] = $record->nid;
  }
  return $nids;
}

/// Deletes every Scopus Article node.
function scopus_delete_nodes () {
  foreach (scopus_get_nodes () as $nid) {
    node_delete ($nid);
  }
}
