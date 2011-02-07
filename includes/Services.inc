<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class Services {

  protected $contentModel;
  protected $modelPid;

  /**
   *
   * @param ContentModel $content_model
   */
  public function __construct($content_model) {
    $this->contentModel = $content_model;
    $this->modelPid = $content_model->pid;
  }

  /**
   *
   * @param TreeView $model_tree
   */
  public function render($model_tree) {
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_service_add', array($this->modelPid));
    $model_tree->addChild('Services', 'folder', $icon);
  }

}