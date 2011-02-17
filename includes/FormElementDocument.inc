<?php

// $Id$

/**
 * @file
 */

/**
 * This class repersents a Form Element from the content
 * model.
 */
class FormElementDocument extends DOMDocument {

  protected $values;

  /**
   * Creates a DOMDocument that repersents a Form Element from the content
   * model.
   *
   * This document repersents a Form Element from the content model. It populates its values from
   * submitted form values.
   *
   * @param String $type
   *   The type of form element this document repersents (textfield, fieldset, etc).
   * @param Array $values
   *   An array of submitted values, repersenting the form elements
   *   properties.
   */
  public function __construct($type, $values) {
    parent::__construct();
    $this->formatOutput = TRUE;
    $this->values = $values;
    $root = $this->appendChild(new DOMElement($type)); // Root Node
    $this->setCommonElementProperties($root, $values);
    $func = 'set' . ucwords($type) . 'ElementProperties';
    if (method_exists($this, $func)) {
      call_user_func_array(array($this, $func), array(&$root, &$values));
    }
  }

  /**
   * Append all properties from the form that are common to all form
   * element types.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setCommonElementProperties(&$root, &$values) {
    $title = $root->appendChild(new DOMElement('title'));
    $title->appendChild(new DOMText($values['title']));
    $description = $root->appendChild(new DOMElement('description'));
    $description->appendChild(new DOMText($values['description']));
    $xpath = $root->appendChild(new DOMElement('xpath'));
    $xpath->appendChild(new DOMText($values['xpath']));
    $xpath->setAttribute('full_path', $values['full_path'] ? 'true' : 'false');
  }

  /**
   * Append all properties specific to textfield's.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setTextfieldElementProperties(&$root, &$values) {
    $required = $root->appendChild(new DOMElement('required'));
    $required->appendChild(new DOMText($values['required'] ? 'true' : 'false'));
    $default_value = $root->appendChild(new DOMElement('default_value'));
    $default_value->appendChild(new DOMText($values['default_value']));
  }

  /**
   * Append all properties specific to textarea's.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setTextareaElementProperties(&$root, &$values) {
    $required = $root->appendChild(new DOMElement('required'));
    $required->appendChild(new DOMText($values['required'] ? 'true' : 'false'));
    $default_value = $root->appendChild(new DOMElement('default_value'));
    $default_value->appendChild(new DOMText($values['default_value']));
  }

  /**
   * Append all properties specific to select boxes.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setSelectElementProperties(&$root, &$values) {
    $required = $root->appendChild(new DOMElement('required'));
    $required->appendChild(new DOMText($values['required'] ? 'true' : 'false'));
    $options = $root->appendChild(new DOMElement('options'));
    $options->setAttribute('ignoreFirstOption', $values['ignoreFirstOption'] ? 'true' : 'false');
    $option_list = explode(';', $values['options']);
    foreach ($option_list as $value) {
      $option = $options->appendChild(new DOMElement('option'));
      $option->appendChild(new DOMText($value));
    }
  }

  /**
   * Append all properties specific to list's.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setListElementProperties(&$root, &$values) {
    $required = $root->appendChild(new DOMElement('required'));
    $required->appendChild(new DOMText($values['required'] ? 'true' : 'false'));
    $default_value = $root->appendChild(new DOMElement('default_value'));
    $default_value->appendChild(new DOMText($values['default_value']));
  }

  /**
   * Append all properties specific to fieldset's.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setFieldsetElementProperties(&$root, &$values) {
    $collapsible = $root->appendChild(new DOMElement('collapsible'));
    $collapsible->appendChild(new DOMText($values['collapsible'] ? 'true' : 'false'));
    $collapsed = $root->appendChild(new DOMElement('collapsed'));
    $collapsed->appendChild(new DOMText($values['collapsed'] ? 'true' : 'false'));
    $root->appendChild(new DOMElement('content'));
  }

  /**
   * Append all properties specific to tabpanel's.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setTabpanelElementProperties(&$root, &$values) {
    $root->appendChild(new DOMElement('content'));
  }

  /**
   * Append all properties specific to checkbox's.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setCheckboxElementProperties(&$root, &$values) {
    $return_value = $root->appendChild(new DOMElement('return_value'));
    $return_value->appendChild(new DOMText($values['return_value']));
  }

}
