<?php

// $Id$

/**
 * @file
 *
 * Repersents the Models Tree in the Content Modeler. It allows the user
 * to select and modify a selection of Content Models.
 */
module_load_include('inc', 'fedora_repository', 'ContentModel');
module_load_include('inc', 'fedora_repository', 'CollectionPolicy');
module_load_include('inc', 'islandora_content_modeler', 'includes/Icon');

/**
 * Faking enumeration for conveience.
 */
class ModelsTreeBranches {
  const MimeTypes = 'MimeTypes';
  const Datastreams = 'ModelDatastreams';
  const IngestRules = 'IngestRules';
  const IngestForm = 'IngestForm';
  const EditMetadataMethod = 'EditMetadataMethod';
  const Services = 'Services';

  /**
   * For the sake of conveience.
   * @var Array
   */
  static $Values = array(
    self::MimeTypes => self::MimeTypes,
    self::Datastreams => self::Datastreams,
    self::IngestRules => self::IngestRules,
    self::IngestForm => self::IngestForm,
    self::EditMetadataMethod => self::EditMetadataMethod,
    self::Services => self::Services,
  );

}

/**
 *
 */
class ModelsTree {

  protected $branches = array();
  protected $contentModel;

  public function __construct() {
    $this->contentModel = $this->getContentModel();
    if ($this->contentModel === false) {
      echo t('Error: Unable to load content model');
      exit();
    }
    $this->registerBranches();
  }

  /**
   *
   */
  public static function renderListOfInstalledContentModels() {
    module_load_include('inc', 'islandora_content_modeler', 'treeview');
    module_load_include('inc', 'fedora_repository', 'CollectionClass');

    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_model_new');
    $modelTree = new treeview('Installed Content Models', 'folder', $icon, 'models');

    $collectionHelper = new CollectionClass();
    $items = new SimpleXMLElement($collectionHelper->getRelatedItems(variable_get('fedora_content_model_collection_pid', 'islandora:ContentModelCollection'), null, null));
    $content_models_installed = false;
    if (count($items->results->result) > 0) {
      foreach ($items->results->result as $result) {
        $child_pid = substr($result->object['uri'], strpos($result->object['uri'], '/') + 1);
        $content_model = ContentModel::loadFromModel($child_pid);
        if ($content_model !== false) {
          $title = "<a href='#' class='list_model' id='$child_pid'>$child_pid</a>";
          $icon = create_icon(Image::Remove, AjaxAction::Form, 'icm_model_purge', array($child_pid));
          $modelTree->addChild($title, 'file', $icon);
          $content_models_installed = true;
        }
      }
    }
    if (!$content_models_installed) {
      $modelTree->addChild(t('No installed content models found.'));
    }
    echo $modelTree->buildTree('filetree');
  }

  /**
   * 
   */
  public static function renderContentModel() {
    $models_tree = new ModelsTree();
    $models_tree->render();
  }

  /**
   *
   * @return ContentModel 
   */
  private function getContentModel() {
    $load_from_model = isset($_GET['model_pid']);
    $model_pid = $load_from_model ? trim($_GET['model_pid']) : variable_get('fedora_repository_pid', 'islandora:top');
    if ($load_from_model) {
      return ContentModel::loadFromModel($model_pid);
    }
    else {
      return ContentModel::loadFromObject($model_pid);
    }
  }

  /**
   *
   */
  private function registerBranches() {
    foreach (ModelsTreeBranches::$Values as $branch) {
      module_load_include('inc', 'islandora_content_modeler', "includes/$branch");
      $this->branches[$branch] = new $branch($this->contentModel, $this->collectionPolicy);
    }
  }

  /**
   *
   * @return string
   */
  private function getTitle() {
    $title = $this->contentModel->pid;
    $title .= ( $this->contentModel->validate() ? '' : '!');
    return $title;
  }

  /**
   * ...
   *
   * @global String $base_url
   * @return String
   *   ...
   */
  private function getTitleIcons() {
    $icons = ' ' . create_icon(Image::Info, AjaxAction::Form, 'icm_model_rollback', array($this->contentModel->pid));
    $icons .= create_icon(Image::View, AjaxAction::Form, 'icm_display_rawXml', array('model', $this->contentModel->pid));
    return $icons;
  }

  /**
   * 
   */
  private function render() {
    module_load_include('inc', 'islandora_content_modeler', 'treeview');
    $model_tree = new treeview($this->getTitle(), 'folder', $this->getTitleIcons(), 'models');
    if (!$this->contentModel->validate()) {
      $errors = $model_tree->addChild('XML Errors', 'folder');
      foreach (ContentModel::$errors as $error) {
        $errors->addChild('<b><font color="red">' . $error . '</font></b>');
      }
      echo $model_tree->buildTree('filetree');
      exit();
    }
    foreach ($this->branches as $branch) {
      $branch->render($model_tree);
    }
    echo $model_tree->buildTree('filetree');
  }
}