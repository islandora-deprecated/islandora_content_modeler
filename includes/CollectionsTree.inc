<?php

// $Id$

/**
 * @file
 *
 * Repersents the Collections Tree in the Content Modeler. It allows the user
 * to modify a collection.
 */
module_load_include('inc', 'fedora_repository', 'ContentModel');
module_load_include('inc', 'fedora_repository', 'CollectionPolicy');
module_load_include('inc', 'islandora_content_modeler', 'includes/Icon');

/**
 * Faking enumeration for conveience.
 */
class CollectionsTreeBranches {
  const AllowedContentModels = 'AllowedContentModels';
  const Datastreams = 'Datastreams';
  const SearchTerms = 'SearchTerms';
  const ChildCollections = 'ChildCollections';
  /**
   * For the sake of conveience.
   * @var Array
   */
  static $Values = array(
    self::AllowedContentModels => self::AllowedContentModels,
    self::Datastreams => self::Datastreams,
    self::SearchTerms => self::SearchTerms,
    self::ChildCollections => self::ChildCollections,
  );

}

/**
 * Models a fedora collection as a tree, for the purpose of editing/adding/removing
 * attributes of the fedora collection.
 */
class CollectionsTree {

  protected $branches = array();
  protected $collectionPid;
  protected $contentModel;
  protected $collectionPolicy;
  /**
   * Repersents the title...
   * @var String
   */
  protected $title;

  /**
   * 
   */
  public function __construct() {
    $this->collectionPid = $this->getCollectionPid();
    $this->contentModel = ContentModel::loadFromObject($this->collectionPid);
    $this->collectionPolicy = CollectionPolicy::loadFromCollection($this->collectionPid);
    if ($this->contentModel === false && $this->collectionPolicy === false) {
      echo t('Error: Missing content model and/or collection policy datastream(s) for  ' . $this->collectionPid);
      exit();
    }
    $this->registerBranches();
  }

  /**
   * Gets the collection pid that this object repersents.
   * 
   * @return String
   *   The pid of collection that this object repersents. If the pid was 
   *   sent to the via a get request it will be used. Otherwise default to 
   *   fedora_repository_pid, as set by the Digital repository module.
   */
  private function getCollectionPid() {
    return (!isset($_GET['collection_pid']) || strtolower($_GET['collection_pid']) == 'false') ?
        variable_get('fedora_repository_pid', 'islandora:top') :
        trim($_GET['collection_pid']);
  }

  /**
   *
   */
  private function registerBranches() {
    foreach (CollectionsTreeBranches::$Values as $branch) {
      module_load_include('inc', 'islandora_content_modeler', "includes/$branch");
      $this->branches[$branch] = new $branch($this->contentModel, $this->collectionPolicy);
    }
  }

  /**
   *
   * @return string 
   */
  private function getTitle() {
    $title = $this->collectionPid;
    $title .= ( $this->collectionPolicy->validate() ? '' : '!');
    $title .= ' (' . $this->contentModel->pid . ($this->contentModel->validate() ? '' : '!') . ')';
    return $title;
  }

  /**
   * ...
   *
   * @global String $base_url
   * @return String
   *   ...
   */
  private function getTitleIcon() {
    global $base_url;
    $module_root = $base_url . '/' . drupal_get_path('module', 'islandora_content_modeler');
    $icon = ' '. create_icon(Image::Edit, AjaxAction::Form, 'icm_collection_edit', array($this->collectionPolicy->pid));
    $icon .= create_icon(Image::Info, AjaxAction::Form, 'icm_collection_rollback', array($this->collectionPolicy->pid));
    $icon .= create_icon(Image::View, AjaxAction::Form, 'icm_display_rawXml', array($this->collectionPolicy->pid));
    return $icon;
  }

  /**
   *
   * @param TreeView $collection_tree
   * @return Boolean
   */
  private function contentModelAndCollectionPolicyIsValid(&$collection_tree) {
    return $this->contentModel->validate() && $this->collectionPolicy->validate();
  }

  /**
   * Adds error/warnings from ...
   *
   * @param TreeView $collection_tree
   */
  private function addContentModelAndCollectionPolicyErrorsToTree(&$collection_tree) {
    $errors = $collection_tree->addChild('XML Errors', 'folder');
    foreach (ContentModel::$errors as $error) {
      $errors->addChild('<b><font color="red">' . $error . '</font></b>');
    }
    foreach (CollectionPolicy::$errors as $err) {
      $errors->addChild('<b><font color="red">' . $error . '</font></b>');
    }
  }

  /**
   * 
   */
  public function renderBranches() {
    module_load_include('inc', 'islandora_content_modeler', 'treeview');

    // Add Root Element.
    $collection_tree = new treeview($this->getTitle(), 'folder', $this->getTitleIcon(), 'collections');

    // Error Occured report and abort.
    if (!$this->contentModelAndCollectionPolicyIsValid()) {
      $this->addContentModelAndCollectionPolicyErrorsToTree($collection_tree);
      echo $collection_tree->buildTree('filetree');
      exit();
    }

    // Render Branches
    foreach ($this->branches as $branch) {
      $branch->render($collection_tree);
    }

    // Render to page.
    echo $collection_tree->buildTree('filetree');
  }

}