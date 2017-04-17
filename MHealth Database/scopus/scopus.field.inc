<?php
/**
  @file
  @brief Defines a set of functions that simplify
    the definition of content type fields.
*/

/**
  @brief Creates a textarea field.
  @param $type (object) the content type that the field will be added to.
  @param $name (string) the field name.
  @param $label (string) the field label.
*/
function scopus_create_textarea ($type, $name, $label) {
  $field = field_info_field($name);
  $instance = field_info_instance('node', $name, $type->type);
  if (empty($field)) {
    $field = array(
      'field_name' => $name,
      'type' => 'text_long',
      'entity_types' => array('node'),
    );
    $field = field_create_field($field);
  }
  if (empty($instance)) {
    $instance = array(
      'field_name' => $name,
      'entity_type' => 'node',
      'bundle' => $type->type,
      'label' => $label,
      'widget' => array('type' => 'text_textarea'),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'text_default',
        ),
        'teaser' => array(
          'label' => 'hidden',
          'type' => 'text_default',
        ),
      ),
    );
    $instance = field_create_instance($instance);
  }
  return $instance;
}

/**
  @brief Creates a text field.
  @param $type (object) the content type that the field will be added to.
  @param $name (string) the field name.
  @param $label (string) the field label.
*/
function scopus_create_textfield ($type, $name, $label) {
  $field = field_info_field($name);
  $instance = field_info_instance('node', $name, $type->type);
  if (empty($field)) {
    $field = array(
      'field_name' => $name,
      'type' => 'text',
      'entity_types' => array('node'),
    );
    $field = field_create_field($field);
  }
  if (empty($instance)) {
    $instance = array(
      'field_name' => $name,
      'entity_type' => 'node',
      'bundle' => $type->type,
      'label' => $label,
      'widget' => array('type' => 'text_textfield'),
      'display' => array(
        'default' => array(
          'label' => 'hidden',
          'type' => 'text_default',
        ),
        'teaser' => array(
          'label' => 'hidden',
          'type' => 'text_default',
        ),
      ),
    );
    $instance = field_create_instance($instance);
  }
  return $instance;
}

/**
  @brief Creates a term reference field.
  @param $type (object) the content type.
  @param $name (string) the field name.
  @param $label (string) the field label.
  @param $machineName (string) the vocabulary
    machine name.
*/
function scopus_create_term_reference ($type, $name, $label, $machineName) {
  $field = field_info_field ($name);
  $instance = field_info_instance ('node', $name, $type->type);
  if (empty ($field)) {
    field_create_field (array (
      'field_name' => $name,
      'type' => 'taxonomy_term_reference',
      'entity_types' => array ('node'),
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'settings' => array (
        'allowed_values' => array (
          array ('vocabulary' => $machineName,
                 'parent' => 0)
        ),
      )
    ));
  }
  if (empty ($instance)) {
    field_create_instance (array (
      'field_name' => $name,
      'entity_type' => 'node',
      'bundle' => $type->type,
      'label' => $label,
      'widget' => array (
        'type' => 'options_select',
      ),
      'settings' => array (
        'text_processing' => true
      ),
      'display' => array (
        'default' => array (
          'label' => 'above',
          'type' => 'text',
          'default_formatter' => 'text_plain'
        ),
        'teaser' => array (
          'label' => 'above',
          'type' => 'text',
          'default_formatter' => 'text_plain'
        )
      )
    ));
  }

  return $instance;
}

/**
  @brief Creats an entity reference field.
  @param $type (object) the content type.
  @param $name (string) the field name.
  @param $label (string) the field label.
  @param $target_types (array) the target types.
*/
function scopus_create_entity_reference ($type, $name, $label, $target_types) {
  $field = field_info_field ($name);
  $instance = field_info_instance ('node', $name, $type->type);
  if (empty ($field)) {
    field_create_field (array (
      'field_name'   => $name,
      'type'         => 'entityreference',
      'entity_types' => array ('node'),
      'cardinality'  => FIELD_CARDINALITY_UNLIMITED,
      'settings'     => array (
        'handler_settings' => array (
          'target_bundles' => $target_types,
          'sort' => array ('type' => 'none')
    ))));
  }
  if (empty ($instance)) {
    field_create_instance (array (
      'field_name'   => $name,
      'entity_type'  => 'node',
      'bundle'       => $type->type,
      'label'        => $label,
      'widget'       => array ('type' => 'entityreference_autocomplete'),
      'display'      => array (
        'default' => array (
          'label' => 'Subject',
          'type'  => 'entityreference_entity_view'
      ))
    ));
  }
  return $instance;
}

/**
  @brief Creates a date field and adds it to the
    PubMed content type.
  @param $type (object) the content type.
  @param $name (string) the field name.
  @param $label (string) the field label.
*/
function scopus_create_date_field ($type, $name, $label) {
  $field = field_info_field ($name);
  $instance = field_info_instance ('node', $name, $type->type);
  if (empty ($field)) {
    field_create_field (array (
      'field_name' => $name,
      'type' => 'date',
      'entity_types' => array ('node')
    ));
  }
  if (empty ($instance)) {
    field_create_instance (array (
      'field_name' => $name,
      'entity_type' => 'node',
      'bundle' => $type->type,
      'label' => $label,
      'widget' => array (
        'type' => 'date_select',
      ),
      'settings' => array (
        'text_processing' => true
      ),
      'display' => array (
        'default' => array (
          'label' => 'above',
          'type' => 'date',
          'default_formatter' => 'date_default'
        ),
        'teaser' => array (
          'label' => 'above',
          'type' => 'date',
          'default_formatter' => 'date_default'
        )
      )
    ));
  }

  return $instance;
}

/**
  @brief Adds a taxonomy term value to a term reference field.
  @param $name (string) the term name.
  @param $machine_name (string) the vocabulary machine name.
  @return (array) the field value.
  @note If the term name does not exist, creates a
    new term with the given name.
*/
function scopus_create_term_reference_value ($name, $machine_name) {
  $vocab = taxonomy_vocabulary_machine_name_load ($machine_name);
  $terms = taxonomy_get_term_by_name ($name, $machine_name);

  $term = null;
  if (count ($terms) < 1) {
    $term = (object) array (
      'vid' => $vocab->vid,
      'name' => $name
    );
    taxonomy_term_save ($term);
  } else {
    $term = array_shift ($terms);
  }

  return array ('tid' => $term->tid);
}

/**
  @brief Adds an entity id to an entity reference field.
  @param $id (integer) the entity id.
  @return (array) the field value.
*/
function scopus_create_entity_reference_value ($id) {
  return array ('target_id' => $id);
}

/**
  @brief Returns a date field value.
  @param $year (int) the year.
  @param $month (int) the month.
  @param $day (int) the day.
  @return (array) the field value.
*/
function scopus_create_date_field_value ($year, $month, $day) {
  return array (
    'value' => "$year-$month-$day" . 'T05:00:00',
    'date_type' => 'date'
  );
}

/**
  @brief Returns a text field value.
  @param $value (string) the value.
  @return (array) the field value.
*/
function scopus_create_text_field_value ($value) {
  return array (
    'value'  => $value,
    'format' => 'plain_text'
  );
}
