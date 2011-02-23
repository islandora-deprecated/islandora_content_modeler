<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class IngestRules {

  protected $contentModel;
  protected $modelPid;

  /**
   *
   * @param ContentModel $content_model
   */
  public function __construct($content_model) {
    $this->contentModel = $content_model;
    $this->modelPid = $this->contentModel->pid;
  }

  /**
   *
   * @param String $id
   * @param Array $ingest_rule
   * @param TreeView $branch
   */
  private function renderIngestRule($id, $ingest_rule, $branch) {
    $icon = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_ingestRule', array($this->modelPid, $id), 'Remove ingest rule');
    $branch = $branch->addChild('Rule ' . $id, 'folder', $icon);
    $this->renderAppliesToFolder($id, $ingest_rule, $branch);
    $this->renderIngestMethods($id, $ingest_rule, $branch);
  }

  /**
   *
   * @param String $id
   * @param Array $ingest_rule
   * @param TreeView $branch
   */
  private function renderAppliesToFolder($id, $ingest_rule, $branch) {
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_appliesTo', array($this->modelPid, $id));
    $applies_to = $branch->addChild('Applies To', 'folder', $icon);
    foreach ($ingest_rule['applies_to'] as $type) {
      $icon = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_appliesTo', array($this->modelPid, $id, $type));
      $applies_to->addChild($type, 'file', $icon);
    }
  }

  /**
   *
   * @param String $id
   * @param Array $ingest_rule
   * @param TreeView $branch
   */
  private function renderIngestMethodParameters($id, $ingest_method, $branch) {
    $arguments = array($this->modelPid, $id, $ingest_method['module'], $ingest_method['file'], $ingest_method['class'], $ingest_method['method']);
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_ingestMethodParam', $arguments);
    $parameters = $branch->addChild('Parameters', 'folder', $icon);
    if (count($ingest_method['parameters']) > 0) {
      foreach ($ingest_method['parameters'] as $key => $val) {
        $title = $key . ' = ' . $val;
        $arguments = array($this->modelPid, $id, $ingest_method['module'], $ingest_method['file'], $ingest_method['class'], $ingest_method['method'], $key);
        $icon = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_ingestMethodParam', $arguments);
        $parameters->addChild($title, 'file', $icon);
      }
    }
  }

  /**
   *
   * @param String $id
   * @param Array $ingest_rule
   * @param TreeView $branch
   */
  private function renderIngestMethod($id, $ingest_method, $branch) {
    $module = $ingest_method['module'];
    $file = $ingest_method['file'];
    $class = $ingest_method['class'];
    $method = $ingest_method['method'];
    $title = "$file<br/>$class->$method";
    $icon = create_icon(Image::Remove, AjaxAction::Button, 'icm_model_remove_ingestMethod', array($this->modelPid, $id, $module, $file, $class, $method));
    $branch = $branch->addChild($title, 'file', $icon);
    $this->renderIngestMethodParameters($id, $ingest_method, $branch);
  }

  /**
   *
   * @param String $id
   * @param Array $ingest_rule
   * @param TreeView $branch
   */
  private function renderIngestMethods($id, $ingest_rule, $branch) {
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_ingestMethod', array($this->modelPid, $id));
    $ingest_methods = $branch->addChild('Ingest Methods', 'folder', $icon);
    foreach ($ingest_rule['ingest_methods'] as $ingest_method) {
      $this->renderIngestMethod($id, $ingest_method, $ingest_methods);
    }
  }

  /**
   *
   * @param TreeView $model_tree
   */
  public function render($model_tree) {
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_add_ingestRule', array($this->modelPid));
    $branch = $model_tree->addChild('Ingest Rules', 'folder', $icon);
    $ingest_rules = $this->contentModel->getIngestRules();
    foreach ($ingest_rules as $id => $ingest_rule) {
      $this->renderIngestRule($id, $ingest_rule, $branch);
    }
  }

}