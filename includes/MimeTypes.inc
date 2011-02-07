<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class MimeTypes {

  protected $contentModel;

  /**
   *
   * @param ContentModel $content_model
   */
  public function __construct($content_model) {
    $this->contentModel = $content_model;
  }

  /**
   *
   * @param TreeView $model_tree
   */
  public function render($model_tree) {
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_mime', array($this->contentModel->pid));
    $branch = $model_tree->addChild('Mimetypes', 'folder', $icon);
    $mime_types = $this->contentModel->getMimeTypes();
    foreach ($mime_types as $type) {
      $icon = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_mime', array($this->contentModel->pid, $type));
      $branch->addChild($type, 'file', $icon);
    }
  }

}