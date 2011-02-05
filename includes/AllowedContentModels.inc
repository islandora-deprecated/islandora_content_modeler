<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class AllowedContentModels {

  protected $collectionPolicy;

  public function __construct(&$content_model, &$collection_policy) {
    $this->collectionPid = $collection_policy->pid;
    $this->collectionPolicy = $collection_policy;
  }

  /**
   * 
   * @param TreeView $collection_tree
   */
  public function render(&$collection_tree) {
    module_load_include('inc', 'islandora_content_modeler', 'includes/Icon');
    $content_models = $this->collectionPolicy->getContentModels();
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_collection_add_cmodel', array($this->collectionPid));
    $id = $this->collectionPid . '-cmodels';
    $branch = $collection_tree->addChild('Allowed Content Models', 'folder', $icon, $id);
    foreach ($content_models as $content_model) {
      $content = $content_model->name . ' (<a href="#" class="collection_model" id="' . $content_model->pid . '">' . $content_model->pid . '</a>)';
      $icon = create_icon(Image::Remove, AjaxAction::Button, 'icm_collection_remove_cmodel', array($this->collectionPid, $content_model->pid));
      $branch->addChild($content, 'file', $icon);
    }
  }

}