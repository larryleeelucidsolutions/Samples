<?php
/**
  @file
  @brief Defines functions that are used to create
    the MHealth DB search interface.
*/

/**
  @brief accepts a string array, trims the
    elements, and removes any empty strings.
  @param $xs (array) the array.
  @return (array) the filtered array.
*/
function _mhealthdb_search_remove_empty_elements ($xs) {
  $ys = array ();
  foreach ((array) $xs as $x) {
    $trimmed_x = trim ($x);
    if (!empty ($trimmed_x)) {
      $ys [] = $trimmed_x;
    }
  }
  return $ys;
}

/**
  @brief accepts a multidimensional array and
    flattens it.
  @param $xs (array) the array.
  @return (array) the flattened result.
*/
function _mhealthdb_search_flatten ($xs) {
  $result = array ();
  foreach ((array) $xs as $x) {
    if (is_array ($x)) {
      $result = array_merge ($result, _mhealthdb_search_flatten ($x));
    } else {
      $result [] = $x;
    }
  }
  return $result;
}

/*
  @brief returns the possible completions for the
    given input.
  @param $vid (integer) the vocabulary id.
  @param $input (string) the partial input.
  @return (array) the completions.
*/
function _mhealthdb_search_taxonomy_autocomplete ($vid, $input = '') {
  $completions = array ();

  if ($input != '') {
    $term_matches = array ();
    $terms = _mhealthdb_search_flatten (taxonomy_get_tree ($vid));

    $term_names = array ();
    foreach ((array) $terms as $term) {
      $term_names [] = $term->name;
    }

    sort ($term_names);

    $input_terms = array_map ('trim', explode (',', $input));
    $input_term = array_pop ($input_terms);

    foreach ((array) $term_names as $term_name) {
      if (stristr ($term_name, $input_term) !== false) {
        $term_matches [$term_name] = $term_name;
      }
    }

    $filtered_input_terms = _mhealthdb_search_remove_empty_elements ($input_terms);

    foreach ((array) $term_matches as $term_match) {
      $completion_terms = $filtered_input_terms;
      $completion_terms [] = $term_match;
      $completion = implode (',', $completion_terms);
      $completions [$completion] = $term_match;
    }
  }

  return $completions;
}

/**
  @brief Returns the set of possible completions
    for the given string.
  @details The set of possible completions
    correspond to terms in the MeSH vocabulary.
  @param $input (string) the partial string.
  @return (array) the possible completions.
*/
function _mhealthdb_search_mesh_autocomplete ($input = '') {
  $mesh_vocab = taxonomy_vocabulary_machine_name_load ('mesh');
  drupal_json_output (_mhealthdb_search_taxonomy_autocomplete ($mesh_vocab->vid, $input));
}

/**
  @brief Retrieves the search state array.
  @return (array) the search state array.
*/
function _mhealthdb_search_state () {
  return $_SESSION ['mhealthdb_search']['state'];
}

/**
  @brief Saves the given search state array.
  @param $state (array) the search state array.
*/
function _mhealthdb_search_save_state ($state) {
  $_SESSION ['mhealthdb_search']['state'] = $state;
}

/**
  @brief Defines the default group state array.
  @return (array) the default group array.
*/
function _mhealthdb_search_default_group_state () {
  return array (
    'op' => ' AND ',
    'terms' => array (array ('op' => ' AND ', 'term' => ''))
  ); 
}

/**
  @brief Defines the default search state array.
  @return (array) the default state array.
*/
function _mhealthdb_search_default_search_state () {
  $x = _mhealthdb_search_default_group_state ();
  return array (
    'tag'  => rand (), // tags behave like revision ids. They are used to ensure that browsers do not auto fill field values.
    'all'  => $x,
    'mesh' => $x,
    'id'   => $x,
    'year' => array ()
  );
}

/**
  @brief Resets the search state variable and
    returns the default advanced search form.
  @details Implements hook_form ().
*/
function _mhealthdb_search_reset_form ($form, $form_state) {
  module_load_include ('module', 'mhealthdb_search');
  $_SESSION ['mhealthdb_search']['state'] = _mhealthdb_search_default_search_state ();
  return drupal_get_form ('mhealthdb_search_advanced_search_form');
}

/**
  @brief Creates an operator field.
  @param $default (string) the default operator value.
  @return (array) the form element array.
*/
function _mhealthdb_search_create_op_field ($default) {
  return array (
    '#default_value' => $default,
    '#options'       => array (' AND ' => 'AND', ' OR ' => 'OR'),
    '#prefix'        => '<div class="mhealthdb_search_field_operator_button">',
    '#suffix'        => '</div>',
    '#type'          => 'select'
  );
}

/**
  @brief Creates a group operator field.
  @param $default (string) the default operator value.
  @return (array) the form element array.
*/
function _mhealthdb_search_create_group_op_field ($default) {
  $field = _mhealthdb_search_create_op_field ($default);
  $field ['#prefix'] = '<div class="mhealthdb_search_group_operator_button">';
  $field ['#suffix'] = '</div>';
  return $field;
}

/**
  @brief Creates a text field.
  @param $title (string) the title.
  @param $default (string) the default value.
  @param $path (string) the autocomplete path.
  @return (array) the form element array.
*/
function _mhealthdb_search_create_text_field ($title = '', $default = '', $path = '', $id = '') {
  $field = array (
    '#default_value' => $default,
    '#title'         => $title,
    '#type'          => 'textfield',
    '#maxlength'     => '1000'
  );
  if ($id) {
    $field ['#id'] = $id;
  }
  if ($path) {
    $field ['#autocomplete_path'] = $path;
  }
/*
  watchdog (
    'mhealthdb_search',
    '[_mhealthdb_search_create_text_field] field: <pre>@field</pre>',
    array ('@field' => print_r ($field, true))
  );
*/
  return $field;
}

/**
  @brief Creates a set of group fields.
  @param $form (array) the form element array.
  @param $id (string) the group id.
  @param $title (string) the fieldset title.
  @param $add_field (string) the add field function.
  @param $path (string) the AJAX handler path.
  @return (array) the form element array.
*/
function _mhealthdb_search_create_fields ($form, $id, $title, $add_field, $path = '') {
  $state = _mhealthdb_search_state ();
  $tag = $state ['tag'];

  $form [$id] = array ();

  $form [$id]['#title'] = $title;
  $form [$id]['op_' . $id] = _mhealthdb_search_create_group_op_field ($state [$id]['op']);

  foreach ($state [$id]['terms'] as $i => $field) {
    $key = $id . '_field_' . $i . '_' . $tag;

    $first = true;
    if (count ($form [$id]) > 2) {
      $form [$id]['op_' . $key] = _mhealthdb_search_create_op_field ($field ['op']);
      $first = false;
    }
    $form [$id][$key] = _mhealthdb_search_create_text_field ($title, $field ['term'], $path);
    if ($first) {
      $form [$id][$key]['#prefix'] = '<div class="mhealthdb_search_first_term_field">';
      $form [$id][$key]['#suffix'] = '</div>';
    } else if ($i == count ($state [$id]['terms']) - 1) {
      $form [$id][$key]['#prefix'] = '<div class="mhealthdb_search_last_term_field">';
      $form [$id][$key]['#suffix'] = '</div>';
    }
  }
  $form [$id]['#type'] = 'fieldset';

  $form ['add_' . $id . '_field'] = array (
    '#ajax' => array (
      'callback' => $add_field,
      'wrapper' => 'mhealthdb_search_form'
    ),
    '#prefix' => '<div class="mhealthdb_search_add_field_button">',
    '#suffix' => '</div>',
    '#type'  => 'button',
    '#value' => 'add ' . $title . ' field'
  );

  return $form;
}

/**
  @brief Returns a text field for the given group.
  @param $id (string) the group id.
  @return (array) the form element array.
*/
function _mhealthdb_search_add_field ($id) {
  module_load_include ('module', 'mhealthdb_search');
  $_SESSION ['mhealthdb_search']['state'][$id]['terms'][] = array ('op' => ' AND ', 'term' => '');
  return drupal_get_form ('mhealthdb_search_advanced_search_form');
}

/**
  @brief Returns a text field for the All group.
  @return (array) the form element array.
*/
function _mhealthdb_search_add_all_field () {
  return _mhealthdb_search_add_field ('all');
}

/**
  @brief Returns a text field for the MeSH group.
  @return (array) the form element array.
*/
function _mhealthdb_search_add_mesh_field () {
  return _mhealthdb_search_add_field ('mesh');
}

/**
  @brief Returns a text field for the Id group.
  @return (array) the form element array.
*/
function _mhealthdb_search_add_id_field () {
  return _mhealthdb_search_add_field ('id');
}
/**
  @brief Determines the state of the given group from a set of form values.
  @param $form_values (array) the search form values.
  @param $id (string) the group id.
  @return (array) the group state.
*/
function _mhealthdb_search_get_group_state ($form_values, $id) {
  // watchdog ('mhealthdb_search', '[_mhealthdb_search_get_group_state] id: @id.', array ('@id' => $id));

  $state = array ('op' => $form_values ['op_' . $id]);

  foreach ($form_values as $key => $value) {
    if (strpos ($key, $id . '_field_') === 0) {
      $trimmed_value = trim ($value);

      if (!empty ($trimmed_value)) {
        $op_key = 'op_' . $key;
        $state ['terms'][] = array (
          'op'   => array_key_exists ($op_key, $form_values) ? $form_values [$op_key] : ' AND ',
          'term' => $trimmed_value
        );
      }
    }
/*
    watchdog (
      'mhealthdb_search',
      '[_mhealthdb_search_get_group_state] id: @id key: @key value: @value state: <pre>@state</pre>.',
      array (
        '@id'    => $id,
        '@key'   => $key,
        '@value' => $value,
        '@state' => print_r ($state, true)
    ));
*/
  }
  if (empty ($state ['terms'])) {
    $state = _mhealthdb_search_default_group_state ();
  }
  return $state;
}

/**
  @brief Parses a set of search form values to determine the current search state.
  @param $form_values (array) the search form values.
  @return (array) the search state.
*/
function _mhealthdb_search_get_state ($form_values) {
  $state = _mhealthdb_search_state ();
  $tag = $state ['tag'];
  return array (
    'all'  => _mhealthdb_search_get_group_state ($form_values, 'all'),
    'mesh' => _mhealthdb_search_get_group_state ($form_values, 'mesh'),
    'id'   => _mhealthdb_search_get_group_state ($form_values, 'id'),
    'year' => empty ($form_values ["year_from_field_$tag"]) ||
              empty ($form_values ["year_to_field_$tag"]) ?
              array () :
              array ('from' => $form_values ["year_from_field_$tag"],
                     'to'   => $form_values ["year_to_field_$tag"])
  );
}

/**
  @brief Adds the given term to the given query expression.
  @param $query (string) the query expression.
  @param $op (string) the boolean operator.
  @param $term (string) the term.
  @return (string) the modified query expression.
*/
function _mhealthdb_search_add_term_to_query ($query, $op, $term) {
  return empty ($term) ? $query : (empty ($query) ? $term : "$query $op $term");
}

/**
  @brief Adds the given term to the given query
    expression and encloses the term within
    parenthesis.
  @param $query (string) the query expression.
  @param $op (string) the boolean operator.
  @param $term (string) the term.
  @return (string) the modified query expression.
*/
function _mhealthdb_search_add_term_to_query_with_paren ($query, $op, $term) {
  return empty ($term) ? $query : _mhealthdb_search_add_term_to_query ($query, $op, "( $term )");
}

/**
  @brief Adds the group that has the given id to the query.
  @param $query (string) the query expression.
  @param $state (array) the search state.
  @param $id (string) the group id.
  @param $prefix (string) the field prefix.
  @return (string) the modified query. 
*/
function _mhealthdb_search_add_group_to_query ($query, $state, $id, $prefix = '') {
  $x = '';
  foreach ($state [$id]['terms'] as $field) {
    $y = '';
    foreach (explode (',', $field ['term']) as $term) {
      if (!empty ($term)) {
        $y = _mhealthdb_search_add_term_to_query ($y, 'OR', "$term");
      }
    }
    if (!empty ($y) && !empty ($prefix)) {
      $y = $prefix . ':(' . $y . ')';
    }
    $x = _mhealthdb_search_add_term_to_query_with_paren ($x, $field ['op'], $y);
  }
  return _mhealthdb_search_add_term_to_query_with_paren ($query, $state [$id]['op'], $x);
}

/**
  @brief Adds the group that has the given id to the query.
  @param $query (string) the query expression.
  @param $state (array) the search state.
  @param $id (string) the group id.
  @param $prefixes (array) the set of field prefixes.
  @param $machine_name (string) the vocabulary name.
  @return (string) the modified query. 
*/
function _mhealthdb_search_add_term_references_to_query ($query, $state, $id, $prefixes, $machine_name) {
  $x = '';
  foreach ($state [$id]['terms'] as $field) {
    $tids = array ();
    foreach (explode (',', $field ['term']) as $subterm) {
      foreach (taxonomy_get_term_by_name (trim ($subterm), $machine_name) as $term) {
        $tids [] = $term->tid;
      }
    }

    if (!empty ($tids)) {
      $y = '';
      foreach ($tids as $tid) {
        foreach ($prefixes as $prefix) {
          $y = _mhealthdb_search_add_term_to_query ($y, 'OR', $prefix . ':' . $tid);
        }
      }
      $x = _mhealthdb_search_add_term_to_query_with_paren ($x, $field ['op'], $y);
    }
  }
  return _mhealthdb_search_add_term_to_query_with_paren ($query, $state [$id]['op'], $x);
}

/**
  @brief Returns a query string that represents the given search state.
  @param $state (array) the search state.
  @return (string) the query string.
*/
function _mhealthdb_search_state_to_query ($state) {
  $query = '';
  $query = _mhealthdb_search_add_group_to_query ($query, $state, 'all');
  $query = _mhealthdb_search_add_term_references_to_query ($query, $state, 'mesh', array ('im_mesh'), 'mesh');
  $query = _mhealthdb_search_add_group_to_query ($query, $state, 'id', 'is_field_document_id');
  if (
    $state ['year']['from'] &&
    $state ['year']['to']
  ) {
    $query = _mhealthdb_search_add_term_to_query_with_paren ($query, 'AND',
      'is_field_document_year:[ ' . 
                $state ['year']['from'] . ' TO ' .
                $state ['year']['to'] . ' ]'
    );
  }
  return $query;
}
