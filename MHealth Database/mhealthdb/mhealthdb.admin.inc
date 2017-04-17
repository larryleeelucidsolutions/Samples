<?php

/**
 * Page callback that displays a simple report of the number of published and unpublished nodes
 * per mhealthdb-related content type (pubmed, mhealth_record, mhealthdb_contributed, and google_scholar_record).
 */
function mhealthdb_reports_page() {
  $rows = array();
  $header = array(
    array('data' => t('Content Type')),
    array('data' => t('# Published Nodes')),
    array('data' => t('# Unpublished Nodes')),
  );

  $types = array('pubmed', 'mhealthdb_record', 'mhealthdb_contributed', 'google_scholar_record');
  sort($types);
  $statuses = array(0, 1);
  foreach ($types as $type) {
    $result = db_query("SELECT (SELECT COUNT(nid) FROM node n WHERE type = :type AND status = :status1) AS Published, (SELECT COUNT(nid) FROM node WHERE type = :type AND status = :status2) AS Unpublished", array(':type' => $type, ':status1' => 1, ':status2' => 0));
    foreach ($result as $data) {
      $rows[] = array('data' =>
        array(
          // Cells
          t($type),
          t($data->Published),
          t($data->Unpublished),
        ),
      );
    }
  }

  $build['mhealthdb_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#attributes' => array('id' => 'admin-mhealthdb'),
    '#empty' => t('No log messages available.'),
    '#prefix' => '<div style="padding-bottom: 1em">This is a summary of node counts, both published and unpublished. To view actual content, go to <a href="/admin/content">administer content</a>.</div>',
  );

  return $build;
}
