<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class EditMetadataMethod {

  protected $contentModel;
  protected $modelPid;
  protected $metadataMethod;

  /**
   *
   * @param ContentModel $content_model
   */
  public function __construct($content_model) {
    $this->contentModel = $content_model;
    $this->modelPid = $content_model->pid;
    $this->metadataMethod = $content_model->getEditMetadataMethod();
  }

  /**
   *
   * @return String
   */
  private function getMetadataMethodTitle() {
    $dsid = $this->metadataMethod['dsid'];
    $file = $this->metadataMethod['file'];
    $class = $this->metadataMethod['class'];
    $method = $this->metadataMethod['method'];
    $handler = $this->metadataMethod['handler'];
    return "DSID: $dsid<br/>$file<br/>Builder: $class->$method<br/>Handler: $class->$handler";
  }

  /**
   *
   * @param TreeView $model_tree 
   */
  public function render($model_tree) {
    $arguments = array($this->modelPid);
    $icon = $this->metadataMethod ? '' : create_icon(Image::Add, AjaxAction::Form, 'icm_model_update_editMetadataMethod', $arguments);
    $branch = $model_tree->addChild('Edit Metadata Method', 'folder', $icon);
    if ($this->metadataMethod === false)
      return;
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_editMetadataMethod', $arguments);
    $icons .= create_icon(Image::Edit, AjaxAction::Form, 'icm_model_update_editMetadataMethod', $arguments);
    $branch->addChild($this->getMetadataMethodTitle(), 'file', $icons);
  }

}