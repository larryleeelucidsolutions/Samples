<?php
/**
 * @file
 * Contains \Drupal\section_106_map\Section106MapSettingsForm
 */
namespace Drupal\section_106_map;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

class Section106MapSettingsForm extends ConfigFormBase {
  /** 
   * {@inheritdoc}
   */
  public function getFormId () {
    return 'section_106_map_admin_settings';
  }

  /** 
   * {@inheritdoc}
   */
  protected function getEditableConfigNames () {
    return [
      'section_106_map.settings',
    ];
  }

  /** 
   * {@inheritdoc}
   */
  public function buildForm (array $form, FormStateInterface $form_state) {
    $config = $this->config ('section_106_map.settings');
    $form['mapbox_access_token'] = array (
      '#type' => 'textfield',
      '#title' => $this->t ('Mapbox Access Token'),
      '#default_value' => $config->get ('mapbox_access_token')
    );  
    $form['filter_score_threshold'] = array (
      '#type' => 'textfield',
      '#title' => $this->t ('Filter Score Threshold'),
      '#default_value' => $config->get ('filter_score_threshold')
    );  
    return parent::buildForm ($form, $form_state);
  }

  /** 
   * {@inheritdoc}
   */
  public function submitForm (array &$form, FormStateInterface $form_state) {
    $this->config ('section_106_map.settings')
      ->set ('mapbox_access_token',    $form_state->getValue ('mapbox_access_token'))
      ->set ('filter_score_threshold', $form_state->getValue ('filter_score_threshold'))
      ->save ();
    parent::submitForm ($form, $form_state);
  }
}
