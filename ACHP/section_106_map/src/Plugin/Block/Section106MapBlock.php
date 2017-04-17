<?php
/**
 * Defines the Section 106 Map block which uses
 * the Leaflet library to present a map of Section
 * 106 Cases.
 *
 * @Block(
 *   id = "section_106_map_block",
 *   admin_label = @Translation("Section 106 Map block")
 * )
 */
namespace Drupal\section_106_map\Plugin\Block;
use Drupal\Core\Block\BlockBase;

class Section106MapBlock extends BlockBase {
  /**
   * {@inheritdoc}
   */
  public function build () {
    $config = \Drupal::config ('section_106_map.settings');
    return array (
      '#attached' => array (
        'library' => array ('section_106_map/section_106_map_library'),
        'drupalSettings' => array (
          'module_path' => base_path () . drupal_get_path ('module', 'section_106_map'),
          'section_106_map' => array (
            'mapbox_access_token'    => $config->get ('mapbox_access_token'),
            'filter_score_threshold' => $config->get ('filter_score_threshold'),
            'cases'                  => $this->getCases ()
        ))
      ),
      // disable caching so that setting updates will take immediate effect. 
      '#cache' => array ('max-age' => 0),
      '#theme' => 'section_106_map'
    );
  }

  /*
    Accepts no arguments and returns an array
    of associative arrays that represent Section
    106 Case nodes.
  */
  private function getCases () {
    $query = \Drupal::entityQuery ('node')
      ->condition ('type', 'section_106_case_profile')
      ->condition ('status', 1);

    $result = $query
      ->condition (
        $query->orConditionGroup ()
          ->condition ('field_case_status', '59') // Open cases 
          ->condition ('field_case_status', '133')) // Reopened cases
      ->execute ();

    $cases  = [];
    foreach (array_keys ($result) as $nid) {
      $node = \Drupal\node\Entity\Node::load ($nid);
      $cases [] = $this->createCase ($node->toArray ());
    }
    return $cases;
  }

  /*
    Accepts one argument: $node_array, an array
    that represents a Section 106 Case node; and
    returns an case record array that represents
    the case node.
  */
  private function createCase ($node_array) {
    $nid = $this->getFieldValue ($node_array, 'nid');
    $url = \Drupal\Core\Url::fromRoute ('entity.node.canonical', ['node' => $nid], array ());
    $alias_url = \Drupal::service('path.alias_manager')->getAliasByPath($url->toString ());
    // \Drupal::logger ('preserve_america_map')->notice ('[PreserveAmericaMap::createProfile] node array: <pre>' . print_r ($node_array, true) . '</pre>');
    return [
      'id'     => $nid,
      'url'    => $alias_url,
      'title'  => $this->getFieldValue ($node_array, 'field_case_name'),
      'body'   => $this->getFieldValue ($node_array, 'body'),
      'agency' => $this->getReferencedNodeTitle ($node_array, 'field_case_federal_agency'),
      'poc'    => array (
                    'name'  => $this->getFieldValue ($node_array, 'field_case_poc_name'),
                    'title' => $this->getFieldValue ($node_array, 'field_case_poc_title'),
                    'email' => $this->getFieldValue ($node_array, 'field_case_poc_email'),
                    'phone' => $this->getFieldValue ($node_array, 'field_case_poc_phone')
                  ),
      'states' => $this->getReferencedTermsNames ($node_array, 'field_case_location'),
      'status' => $this->getReferencedTermName ($node_array, 'field_case_status')
    ];
  }

  /*
    Accepts two arguments:

    * $node_array, an array that represents
      a node
    * and $fieldName, a string that denotes
      a field name

    and returns the title of the node referenced
    by the given field.
  */
  private function getReferencedNodeTitle ($node_array, $fieldName) {
    $nid  = $this->getFieldValue ($node_array, $fieldName, 'target_id');
    $node = is_null ($nid) ? null : \Drupal\node\Entity\Node::load ($nid);
    return is_null ($node) ? null : $node->getTitle ();
  }

  /*
    Accepts two arguments:

    * $node_array, an array that represents
      a node
    * and $fieldName, a string that denotes
      a field name

    and returns the name of the term referenced
    by the given field as a string or null
    (if the field is empty).
  */
  private function getReferencedTermName ($node_array, $fieldName) {
    $tid = $this->getFieldValue ($node_array, $fieldName, 'target_id');
    return is_null ($tid) ? null : $this->getTermName ($tid);
  }

  /*
    Accepts two arguments:

    * $node_array, an array that represents
      a node
    * and $fieldName, a string that denotes
      a field name

    and returns a string array listing the names
    of the terms referenced by the given field.
  */
  private function getReferencedTermsNames ($node_array, $fieldName) {
    $names = [];
    $tids = $this->getFieldValues ($node_array, $fieldName, 'target_id');
    foreach ($tids as $tid) {
      $name = $this->getTermName ($tid);
      if ($name) {
        $names [] = $name;
      }
    }
    return $names;
  }

  /*
    Accepts three arguments:

    * $node_array, an array that represents
      a node
    * $fieldName, a string that denotes a
      field name
    * and $fieldValueKey (default: "value")
      a string that specifies the key used to
      extract values from field arrays

    and returns either the field value as a
    string or null (if the field is empty).
  */
  private function getFieldValue ($node_array, $fieldName, $fieldValueKey = 'value') {
    return empty ($node_array [$fieldName]) ? null :
      $node_array [$fieldName][0][$fieldValueKey];
  }

  /*
    Accepts three arguments:

    * $node_array, an array that represents
      a node
    * $fieldName, a string that denotes a
      field name
    * and $fieldValueKey (default: "value")
      a string that specifies the key used to
      extract values from field arrays

    and returns an array listing the field's
    values.
  */
  private function getFieldValues ($node_array, $fieldName, $fieldValueKey = 'value') {
    $values = [];
    foreach ($node_array [$fieldName] as $field) {
      $values [] = $field [$fieldValueKey];
    }
    return $values;
  }

  /*
    Accepts one argument: $tid, a term ID;
    and returns the title of the referenced term.
  */
  private function getTermName ($tid) {
    $term = \Drupal::entityTypeManager ()->getStorage ('taxonomy_term')->load ($tid);
    return is_null ($term) ? null : $term->getName ();
  }
}
