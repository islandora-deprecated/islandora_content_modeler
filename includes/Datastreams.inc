<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class Datastreams {

  protected $contentModel;
  protected $collectionPid;

  public function __construct(&$content_model, &$collection_policy) {
    $this->contentModel = $content_model;
    $this->collectionPid = $collection_policy->pid;
  }

  /**
   *
   * @param TreeView $collection_tree 
   */
  public function render(&$collection_tree) {
    $data_streams = $this->contentModel->listDatastreams();
    if (count($data_streams) > 0) {
      $id = $this->collectionPid . '-data_streams';
      $branch = $collection_tree->addChild('Datastreams', 'folder', null, $id);
      foreach ($data_streams as $data_stream)
        $branch->addChild($data_stream);
    }
  }

}