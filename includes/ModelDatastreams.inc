<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class ModelDatastreams {

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
   * @param Array $display_method
   * @param TreeView $branch
   */
  private function renderDisplayMethod($display_method, $branch) {
    $default = $display_method['default'];
    $module = $display_method['module'];
    $file = $display_method['file'];
    $class = $display_method['class'];
    $method = $display_method['method'];
    $title = ($default == true ? '<b>' : '') . "$file<br/>$class->$method";
    $arguments = array($this->modelPid, $datastream, $module, $file, $class, $method);
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_dispmeth', $arguments);
    $icons .= create_icon(Image::Edit, AjaxAction::Button, 'icm_model_default_dispmeth', $arguments);
    $icons .= $default == true ? '</b>' : '';
    $branch->addChild($title, 'file', $icons);
  }

  /**
   *
   * @param Array $datastream
   * @param TreeView $branch
   */
  private function renderDisplayMethods($datastream, $branch) {
    $arguments = array($this->modelPid, $datastream);
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_dispmeth', $arguments);
    $branch = $branch->addChild('Display Methods', 'folder', $icon);
    $display_methods = $this->contentModel->getDisplayMethods($datastream);
    if ($display_methods === false || count($display_methods) <= 0)
      return;
    foreach ($display_methods as $display_method) {
      $this->renderDisplayMethod($display_method, $branch);
    }
  }

  /**
   *
   * @param Array $datastream
   * @param TreeView $branch
   */
  private function renderAddDatastreamMethod($datastream, $branch) {
    $add_methods = $this->contentModel->getAddDsMethod($datastream);
    $title = '<font color="green">Add Datastream Method</font>';
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_edit_adddsmeth', array($this->modelPid, $datastream));
    $branch->addChild($title, 'file', $icon);
  }

  /**
   *
   * @param Array $datastream
   * @param TreeView $branch
   */
  private function renderDatastream($datastream, $branch) {
    $arguments = array($this->modelPid, $datastream);
    $title = $datastream . ($this->contentModel->displayInFieldSet($datastream) ? '(Display in Fieldset) ' : '');
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_ds', $arguments);
    $icons .= create_icon(Image::View, AjaxAction::Button, 'icm_model_toggle_dsDisplay', $arguments);
    $branch = $branch->addChild($title, 'file', $icons);
    $this->renderDisplayMethods($datastream, $branch);
    $this->renderAddDatastreamMethod($datastream, $branch);
  }

  /**
   *
   * @param TreeView $model_tree
   */
  public function render($model_tree) {
    $datastreams = $this->contentModel->listDatastreams();
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_ds', array($this->modelPid));
    $branch = $model_tree->addChild('Datastreams', 'folder', $icon);
    if (count($datastreams) <= 0)
      return;
    foreach ($datastreams as $datastream) {
      $this->renderDatastream($datastream, $branch);
    }
  }

}