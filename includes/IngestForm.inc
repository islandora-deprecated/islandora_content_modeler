<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class IngestForm {

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
   * @return String
   */
  private function getTitle() {
    $attributes = $this->contentModel->getIngestFormAttributes();
    $dsid = $attributes['dsid'];
    $page = $attributes['page'];
    $hidden = $attributes['hide_file_chooser'] ? 'hidden' : 'visible';
    return "Ingest Form<br/>(dsid: <b>$dsid</b> page: <b>$page</b> file_chooser: <b>$hidden</b> )";
  }

  /**
   *
   * @return String
   */
  private function getTitleIcon() {
    $attributes = $this->contentModel->getIngestFormAttributes();
    $dsid = $attributes['dsid'];
    $page = $attributes['page'];
    $hidden = $attributes['hide_file_chooser'] ? 'true' : 'false';
    $arguments = array('icm_model_edit_ingestForm', $this->modelPid, $dsid, $page, $hidden);
    return create_icon(Image::Edit, AjaxAction::Form, 'icm_model_edit_ingestForm', $arguments);
  }

  /**
   *
   * @param DOMElement $element
   * @param Boolean $first
   * @param Boolean $last
   * @return String
   */
  private function getFormElementIcons($element, $first, $last) {
    $arguments = array($this->modelPid, $element['name']);
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'imc_model_remove_ingestFormElement', $arguments);
    if (!$first) {
      $icons .= create_icon(Image::Up, AjaxAction::Button, 'icm_model_inc_ingestFormElement', $arguments);
    }
    if (!$last) {
      $icons .= create_icon(Image::Down, AjaxAction::Button, 'icm_model_dec_ingestFormElement', $arguments);
    }
    $icons .= create_icon(Image::Edit, AjaxAction::Form, 'icm_model_edit_ingestFormElement', $arguments);
    return $icons;
  }

  /**
   *
   * @param DOMElement $element
   * @param Boolean $first
   * @param Boolean $last
   * @param TreeView $branch
   * @return String
   */
  private function renderFormElement($element, $first, $last, $branch) {
    $label = $element['label'];
    $name = $element['name'];
    $type = $element['type'];
    $title = "<b>$label</b> $name ($type)";
    $branch = $branch->addChild($title, 'file', $this->getFormElementIcons($element, $first, $last));
    $this->renderFormElementParameters($element, $branch);
    $this->renderAuthoritativeList($element, $branch);
  }

  /**
   *
   * @param DOMElement $element
   * @param TreeView $branch
   * @return String
   */
  private function renderFormElementParameters($element, $branch) {
    $params = $this->contentModel->getIngestFormElementParams($element['name']);
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_ingestElementParam', array($this->modelPid, $element['name']));
    $parameters = $branch->addChild('Parameters', 'folder', $icon);
    if (count($params) > 0) {
      foreach ($params as $key => $val) {
        $title = $key . ' = ' . $val;
        $icon = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_ingestElementParam', array($this->modelPid, $element['name'], $key));
        $parameters->addChild($title, 'file', $icon);
      }
    }
  }

  /**
   *
   * @param DOMElement $element
   * @param String $value
   * @param Boolean $first
   * @param Boolean $last
   * @return String
   */
  private function getAuthoritativeListIcons($element, $value, $first, $last) {
    $argument = array($this->modelPid, $element['name'], $value);
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_authListItem', $arguments);
    if (!$first) {
      $icons .= create_icon(Image::Up, AjaxAction::Button, 'icm_model_inc_authListItem', $arguments);
    }
    if (!$last) {
      $icons .= create_icon(Image::Down, AjaxAction::Button, 'icm_model_dec_authListItem', $arguments);
    }
    return $icons;
  }

  /**
   *
   * @param DOMElement $element
   * @param TreeView $branch
   * @return String
   */
  private function renderAuthoritativeList($element, $branch) {
    $render = $element['type'] == 'select' || $element['type'] == 'radio' || $element['type'] == 'other_select';
    if (!$render)
      return;
    $arguments = array($this->modelPid, $element['name']);
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_authListItem', $arguments);
    $branch = $branch->addChild('Authoritative List', 'folder', $icon);
    $end = count($element['authoritative_list']) - 1;
    $i = 0;
    foreach ($element['authoritative_list'] as $value => $label) {
      $first = $i == 0;
      $last = $i == $end;
      $title = "<b>$label</b> $value";
      $icons = $this->getAuthoritativeListIcons($element, $value, $first, $last);
      $branch->addChild($title, 'file', $icons);
      $i++;
    }
  }

  /**
   *
   * @param TreeView $branch
   */
  private function renderFormElements($branch) {
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_ingestFormElement', array($this->modelPid));
    $branch = $branch->addChild('Form Elements', 'folder', $icon);
    $ingest_form_elements = $this->contentModel->getIngestFormElements();
    if ($ingest_form_elements === false)
      return;
    $count = count($ingest_form_elements);
    foreach ($ingest_form_elements as $i => $element){
      $first = $i == 0;
      $last = $i == ($count - 1);
      $this->renderFormElement($element, $first, $last, $branch);
    }
  }

  /**
   *
   * @param TreeView $branch
   */
  private function renderFormBuilderMethod($branch) {
    $builder_method = $this->contentModel->getIngestFormBuilderMethod();
    $file = $builder_method['file'];
    $class = $builder_method['class'];
    $method = $builder_method['method'];
    $handler = $builder_method['handler'];
    $title = "$file<br/>Builder: $class->$method<br/>Handler: $class->$handler";
    $branch->addChild($title, 'file');
  }

  /**
   *
   * @param TreeView $model_tree
   */
  public function render($model_tree) {
    $branch = $model_tree->addChild($this->getTitle(), 'folder', $this->getTitleIcon());
    $this->renderFormBuilderMethod($branch);
    $this->renderFormElements($branch);
  }

}