<?php
/**
  @file
  @brief The Scopus Import CSV module defines an
    API that can be used to import Scopus records
    from CSV files. This file defines the key parse
    and import functions.
*/

/**
  @brief Parses the given CSV entry.
  @param $entry_string (string) the CSV entry string.
  @param $field_names (array) the names of each field.
  @param $replace (boolean) whether or not existing document nodes will be replaced.
  @param $field_delimiter (string) the field delimiter.
  @param $field_value_delimiter (string) the field value delimiter.
  @param $enclosure (string) the enclosure (quote) character.
  @param $escape (string) the escape character.
  @note the CSV string must define a set of necessary fields.
  @return (array) the CSV entry.
*/
function scopus_import_csv_parse ($entry_string, $field_names, $replace = false, $field_delimiter = ",", $field_value_delimiter = "|", $enclosure = '"', $escape = '\\') {
  module_load_include ('inc', 'scopus');

  watchdog (
    'scopus',
    '[scopus_import_csv_parse] entry string: @entry_string headers: @headers.',
    array (
      '@entry_string' => $entry_string,
      '@headers'      => print_r ($field_names, true)
  ));

  $entry = array ();

  // II.a. parse each row into fields.
  $fields = scopus_import_str_getcsv ($entry_string, $field_delimiter, $enclosure, $escape);
  $num_fields = count ($fields);
    
  if ($num_fields != count ($field_names)) {
    watchdog (
      'scopus_import_csv',
      '[scopus_import_csv_parse] Invalid csv string - invalid number of columns.',
      array (),
      WATCHDOG_ERROR
    );
    return;
  }

  // II.b. parse each field into values.
  for ($i = 0; $i < $num_fields; $i ++) {
    $field_values = scopus_import_str_getcsv ($fields [$i], $field_value_delimiter, $enclosure, $escape);
    $num_field_values = count ($field_values);
    if ($num_field_values > 1) {
      $y = $field_values;
    } else if ($num_field_values == 1) {
      $y = $field_values [0];
    } else {
      $y = '';
    }
    $entry [$field_names [$i]] = $y;
  }

  return $entry;
}

/**
  @brief Returns the CSV file headers.
  @return (array) the file headers.
  @note Scopus provides several different CSV
    export formats. The headers listed below are
    the headers that are listed in the 'Abstract
    Format'.
*/
function scopus_import_csv_headers () {
  return array (
    'authors',
    'title',
    'year',
    'source',
    'volume',
    'issue',
    'article_number',
    'page_start',
    'page_end',
    'page_count',
    'cited_by',
    'link',
    'affiliations',
    'authors_with_affiliations',
    'abstract',
    'author_keywords',
    'document_type',
    'database_source'
  );
}

/**
  @brief a crude replacement for str_getcsv in early versions of PHP.
  @see http://www.electrictoolbox.com/php-str-getcsv-function/
*/
function scopus_import_str_getcsv($input, $delimiter = ",", $enclosure = '"', $escape = "\\") {
  $ret = array();

  if (!function_exists('str_getcsv')) {
    $fp = fopen("php://memory", 'r+');
    fputs($fp, $input);
    rewind($fp);
    $ret = fgetcsv($fp, null, $delimiter, $enclosure); // $escape only got added in 5.3.0
    fclose($fp);
  } else {
    $ret = str_getcsv($input, $delimiter, $enclosure, $escape);
  }
  return $ret;
}

/**
  @brief Parses the given CSV file.
  @param $filename (string) the CSV file name.
  @param $offset (integer) the number of entries that should be skipped before parsing.
  @return (array) the CSV file entries.
  @note Entries are valid record arrays.
*/
function scopus_import_csv_parse_file ($filename, $offset = 1) {
  $handle = fopen ($filename, 'r');
  $entries = array ();
  $num = 0;
  $headers = scopus_import_csv_headers ();
  while (!feof ($handle)) {
    $entry_string = fgets ($handle);
    if ($num >= $offset) {
      $entry = scopus_import_csv_parse ($entry_string, $headers);
      if ($entry) {
        $entries [] = $entry;
      }
    }
    $num ++;
  }
  fclose ($handle);
  return $entries;
}

/**
  @brief Imports the Scopus records listed in the
    given CSV file.
  @param $filename (string) the CSV file name.
  @param $offset (integer) the number of entries
    that should be skipped before importing.
*/
function scopus_import_csv_file ($filename, $offset = 1) {
  module_load_include ('inc', 'scopus_import');

  return scopus_import_records (
    scopus_import_csv_parse_file ($filename, $offset));
}
