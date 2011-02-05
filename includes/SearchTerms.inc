<?php

// $Id$

/**
 * @file
 *
 */

/**
 *
 */
class SearchTerms {

  protected $collectionPolicy;
  protected $collectionPid;

  public function __construct(&$content_model, &$collection_policy) {
    $this->collectionPid = $collection_policy->pid;
    $this->collectionPolicy = $collection_policy;
  }

  /**
   *
   * @param TreeView $collection_tree
   */
  public function render(&$collection_tree) {
    $terms = $this->collectionPolicy->getSearchTerms(true);
    $icon = create_icon(Image::Add, AjaxAction::Form, 'icm_collection_add_term', array($this->collectionPid));
    $id = $this->collectionPid . '-terms';
    $branch = $collection_tree->addChild('Search Terms', 'folder', $icon, $id);
    if (count($terms) > 0) {
      foreach ($terms as $term) {
        $content = ($term['default'] ? '<b>' : '') . $term['value'] . ' (' . $term['field'] . ')' . ($term['default'] ? '</b>' : '');
        $arguments = array($this->collectionPid, $term['field']);
        $icons = create_icon(Image::Remove, AjaxAction::Button, 'icm_collection_remove_term', $arguments);
        $icons .= create_icon(Image::Edit, AjaxAction::Button, 'icm_collection_default_term', $arguments);
        $branch->addChild($content, 'file', $icons);
      }
    }
  }

}