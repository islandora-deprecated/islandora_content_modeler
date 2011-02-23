<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class ChildCollections {
  protected $collectionPid;
  protected $collectionModel;
  protected $collectionPolicy;

  public function __construct(&$content_model, &$collection_policy) {
    $this->collectionPid = $collection_policy->pid;
    $this->collectionPolicy = $collection_policy;
  }

  /**
   *
   * @return Boolean
   */
  private function shouldRenderChildCollections() {
    $content_models = $this->collectionPolicy->getContentModels();
    foreach ($content_models as $content_model) {
      if ($content_model->pid == variable_get('fedora_collection_model_pid', 'islandora:collectionCModel')) {
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
   *
   * @param TreeView $collection_tree 
   */
  public function render(&$collection_tree) {
    module_load_include('inc', 'fedora_repository', 'CollectionClass');
    if (!$this->shouldRenderChildCollections()) {
      return;
    }
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_collection_new', array($this->collectionPid));
    $id = $this->collectionPid . '-children';
    $branch = $collection_tree->addChild('Child Collections', 'folder', $icon, $id);

    $collection_helper = new CollectionClass();
    $items = new SimpleXMLElement($collection_helper->getRelatedItems($this->collectionPid, null, null));
    if (count($items->results->result) > 0) {
      foreach ($items->results->result as $result) {
        $child_pid = substr($result->object['uri'], strpos($result->object['uri'], '/') + 1);
        $model_pid = substr($result->content['uri'], strpos($result->content['uri'], '/') + 1);

        $child_collection_policy = CollectionPolicy::loadFromCollection($child_pid);
        if ($child_collection_policy !== false) {
          $content = '<a href="#" class="collection_child" id="' . $child_pid . '">' . $child_pid . '</a> (<a href="#" class="collection_model" id="' . $model_pid . '">' . $model_pid . '</a>)';
          $icon = create_icon(Image::Remove, AjaxAction::Form, 'icm_collection_purge', array($child_pid), 'Delete');
          $branch->addChild($content, 'file', $icon);
        }
      }
    }
  }

}