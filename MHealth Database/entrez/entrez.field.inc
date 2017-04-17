<?php
/**
  @file
  @brief Defines a set of functions that simplify
    the definition of content type fields.
*/

/**
  @brief Adds a taxonomy term value to a term reference field.
  @param $name (string) the term name.
  @param $machine_name (string) the vocabulary machine name.
  @return (array) the field value.
  @note If the term name does not exist, creates a
    new term with the given name.
*/
function entrez_create_term_reference_value ($name, $machine_name) {
  $vocab = taxonomy_vocabulary_machine_name_load ($machine_name);
  $terms = taxonomy_get_term_by_name ($name, $machine_name);
  $desc='';

  $term = null;
  if (count ($terms) < 1) {
    if (strlen($name)>255) {
     $spacePos=strrpos(substr($name,0,255),' '); 
     $trimmed=substr($name,0,$spacePos);
     $desc=$name;
     $name=$trimmed;
    }
    $term = (object) array (
      'vid' => $vocab->vid,
      'name' => $name
    );
    if (strlen($desc)>0) {
      $term->description=$desc;
    }
    taxonomy_term_save ($term);
  } else {
    $term = array_shift ($terms);
  }

  return array ('tid' => $term->tid);
}
