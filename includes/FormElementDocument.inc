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
    $this->setFormBuilderElementProperties($root, $values['form_builder']);
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
    if ($values['description'] != '') {
      $description = $root->appendChild(new DOMElement('description'));
      $description->appendChild(new DOMText($values['description']));
    }
  }

  /**
   * Appends form builder properties to the element.
   *
   * @param DOMElement $root
   *   Root element of the dom document.
   * @param Array $values
   *   An array of submitted values, repersenting the form element's
   *   properties.
   */
  private function setFormBuilderElementProperties(&$root, &$values) {
    $create_element = $values['element'] != '';
    $create_attribute = $values['attribute'] != '';
    $create_xml = $values['xml'] != '';
    if (!$create_element && !$create_attribute && !$create_xml) {
      return;
    }
    $form_builder = $root->appendChild(new DOMElement('form_builder'));
    $xpath = $form_builder->appendChild(new DOMElement('xpath'));
    $path = $xpath->appendChild(new DOMElement('path'));
    $path->appendChild(new DOMText($values['path']));
    $path->setAttribute('full', $values['full'] ? 'true' : 'false');
    if ($values['full']) {
      $parent_path = $xpath->appendChild(new DOMElement('parent_path'));
      $parent_path->appendChild(new DOMText($values['parent_path']));
    }
    if ($create_element) {
      $xml = $xpath->appendChild(new DOMElement('element'));
      $xml->appendChild(new DOMText($values['element']));
    }
    else if ($create_attribute) {
      $xml = $xpath->appendChild(new DOMElement('attribute'));
      $xml->appendChild(new DOMText($values['attribute']));
    }
    else if ($create_xml) {
      $xml = $xpath->appendChild(new DOMElement('xml'));
      $xml->appendChild(new DOMText($values['xml']));
    }
    $value = ($values['require_value']) ? 'true' : 'false';
    $required = $form_builder->appendChild(new DOMElement('require_value'));
    $required->appendChild(new DOMText($value));
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
    if ($values['required']) {
      $required = $root->appendChild(new DOMElement('required'));
      $required->appendChild(new DOMText($values['required'] ? 'true' : 'false'));
    }
    if ($values['default_value']) {
      $default_value = $root->appendChild(new DOMElement('default_value'));
      $default_value->appendChild(new DOMText($values['default_value']));
    }
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
    if ($values['required']) {
      $required = $root->appendChild(new DOMElement('required'));
      $required->appendChild(new DOMText($values['required'] ? 'true' : 'false'));
    }
    if ($values['default_value']) {
      $default_value = $root->appendChild(new DOMElement('default_value'));
      $default_value->appendChild(new DOMText($values['default_value']));
    }
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
  private function setSetElementProperties(&$root, &$values) {
    if ($values['required']) {
      $value = ($values['required']) ? 'true' : 'false';
      $required = $root->appendChild(new DOMElement('required'));
      $required->appendChild(new DOMText($value));
    }
    if ($values['default_value'] != '') {
      $default_value = $root->appendChild(new DOMElement('default_value'));
      $default_value->appendChild(new DOMText($values['default_value']));
    }
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
    if ($values['collapsible']) {
      $value = ($values['collapsible']) ? 'true' : 'false';
      $collapsible = $root->appendChild(new DOMElement('collapsible'));
      $collapsible->appendChild(new DOMText($value));
    }
    if ($values['collapsed']) {
      $value = ($values['collapsed']) ? 'true' : 'false';
      $collapsed = $root->appendChild(new DOMElement('collapsed'));
      $collapsed->appendChild(new DOMText($value));
    }
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
    if ($values['collapsible']) {
      $value = ($values['collapsible']) ? 'true' : 'false';
      $collapsible = $root->appendChild(new DOMElement('collapsible'));
      $collapsible->appendChild(new DOMText($value));
    }
    if ($values['collapsed']) {
      $value = ($values['collapsed']) ? 'true' : 'false';
      $collapsed = $root->appendChild(new DOMElement('collapsed'));
      $collapsed->appendChild(new DOMText($value));
    }
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
    if ($values['return_value'] != '') {
      $return_value = $root->appendChild(new DOMElement('return_value'));
      $return_value->appendChild(new DOMText($values['return_value']));
    }
  }

}
