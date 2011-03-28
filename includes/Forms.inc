<?php

// $Id$

/**
 * @file
 *
 * Form hooks and blah blah.
 */
module_load_include('inc', 'fedora_repository', 'ContentModel');

/**
 * Reperesents the Forms branch in the content model.
 */
class Forms {

  /**
   * The content model this branch belongs to.
   * 
   * @var ContentModel
   */
  protected $contentModel;

  /**
   * Create a Forms object.
   *
   * @param ContentModel $content_model
   */
  public function __construct($content_model) {
    $this->contentModel = $content_model;
  }

  /**
   *
   * @param String $model_pid
   * @return Forms
   */
  public static function createFromPID($model_pid) {
    $content_model = ContentModel::loadFromModel($model_pid);
    return new Forms($content_model);
  }

  /**
   *
   * @return <type> 
   */
  public static function getElementProperties() {
    if (!isset($_POST['form_build_id']))
      return;
    $form_state = array('storage' => NULL, 'submitted' => FALSE, 'post' => $_POST);
    $form_build_id = $_POST['form_build_id'];
    if (!$form = form_get_cache($form_build_id, $form_state))
      exit();
    $forms = self::createFromPID($form_state['post']['model_pid']);
    $form['form']['element_properties'] = $forms->getElementPropertiesForm($form_state['post']['type']);
    $form_id = $form['#parameters'][0];
    $form['#post'] = $_POST;
    $form['#programmed'] = FALSE;
    form_set_cache($form_build_id, $form, $form_state);
    $form = form_builder($form_id, $form, $form_state);
    unset($form['form']['element_properties']['#prefix'], $form['form']['element_properties']['#suffix']); // Prevent dbl wrapping.
    drupal_json(array(
      'status' => TRUE,
      'data' => theme('status_messages') . drupal_render($form['form']['element_properties']),
    ));
  }

  /**
   *
   * @param array $params
   * @return array
   */
  public static function expandParameters($params) {
    if (is_array($params)) {
      list($model_pid, $form_name) = $params;
      $path = implode(' ', array_splice($params, 2));
      return array($model_pid, $form_name, $path);
    }
    return array();
  }

  /**
   * Removes a form from the content model.
   *
   * @param array $params
   *   Ajax button callback parameters.
   */
  public static function removeForm($params) {
    list($model_pid, $form_name) = self::expandParameters($params);
    $forms = self::createFromPID($model_pid);
    if ($forms->_removeForm($form_name)) {
      echo 'success:' . t('Removed form <b>%name</b> from model <b>%model_pid</b>.',
          array('%name' => $form_name, '%model_pid' => $model_pid));
    }
    else {
      echo t('Error: Unable to remove form <b>%name</b> from model <b>%model_pid</b>.',
          array('%name' => $name, '%model_pid' => $model_pid));
    }
    exit();
  }

  /**
   * Removes a form element from a form.
   *
   * @param array $params
   *   Ajax button callback parameters.
   */
  public static function removeFormElement($params) {
    list($model_pid, $form_name, $path) = self::expandParameters($params);
    $forms = self::createFromPID($model_pid);
    if ($forms->_removeFormElement($form_name, $path)) {
      echo 'success:' . t('Removed form element from model <b>%model_pid</b>.',
          array('%name' => $form_name, '%model_pid' => $model_pid));
    }
    else {
      echo t('Error: Unable to remove form element from model <b>%model_pid</b>.',
          array('%name' => $form_name, '%model_pid' => $model_pid));
    }
    exit();
  }

  /**
   * Moves the element higher up in its list of siblings.
   *
   * @param array $params
   *   Ajax button callback parameters.
   */
  public static function incrementFormElement($params) {
    list($model_pid, $form_name, $path) = self::expandParameters($params);
    $forms = self::createFromPID($model_pid);
    if ($forms->_incrementFormElement($form_name, $path)) {
      echo 'success:' . t('Successful.');
    }
    else {
      echo t('Error:');
    }
    exit();
  }

  /**
   * Moves the element higher up in its list of siblings.
   *
   * @param array $params
   *   Ajax button callback parameters.
   */
  public static function decrementFormElement($params) {
    list($model_pid, $form_name, $path) = self::expandParameters($params);
    $forms = self::createFromPID($model_pid);
    if ($forms->_decrementFormElement($form_name, $path)) {
      echo 'success:' . t('Successful');
    }
    else {
      echo t('Error:');
    }
    exit();
  }

  /**
   *
   * @param string $name
   * @return DOMElement
   */
  public function getFormElement($name) {
    return $this->contentModel->getForm($name);
  }

  /**
   *
   * @param String $form_name
   * @return HTML
   */
  function getFormIcons($form_name, $form) {
    $params = array($this->contentModel->pid, $form_name);
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'Forms::removeForm', $params);
    $icons .= create_icon(Image::Add, AjaxAction::Form, 'icm_add_form_element', $params);
    if (!$this->isFirstChildElement($form)) {
      $icons .= create_icon(Image::Up, AjaxAction::Button, 'Forms::incrementFormElement', $params);
    }
    if (!$this->isLastChildElement($form)) {
      $icons .= create_icon(Image::Down, AjaxAction::Button, 'Forms::decrementFormElement', $params);
    }
    $icons .= create_icon(Image::Edit, AjaxAction::Form, 'icm_edit_form', $params);
    return $icons;
  }

  /**
   *
   * @param String $form_name
   * @param DOMElement $form_element
   * @return HTML
   */
  function getFormElementIcons($form_name, $form_element) {
    $path_from_form_node = array_reverse($this->getPathToFormElement($form_element));
    $params = array($this->contentModel->pid, $form_name, implode(' ', $path_from_form_node));
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'Forms::removeFormElement', $params);
    $result = $form_element->getElementsByTagName('content');
    if ($result && $result->length) {
      $content = $result->item(0);
      $path_to_content = array_reverse($this->getPathToFormElement($content));
      $add_params = array($this->contentModel->pid, $form_name, implode(' ', $path_to_content));
      $icons .= create_icon(Image::Add, AjaxAction::Form, 'icm_add_form_element', $add_params);
    }
    if (!$this->isFirstChildElement($form_element)) {
      $icons .= create_icon(Image::Up, AjaxAction::Button, 'Forms::incrementFormElement', $params);
    }
    if (!$this->isLastChildElement($form_element)) {
      $icons .= create_icon(Image::Down, AjaxAction::Button, 'Forms::decrementFormElement', $params);
    }
    $icons .= create_icon(Image::Edit, AjaxAction::Form, 'icm_edit_form_element', $params);
    return $icons;
  }

  /**
   *
   * @param TreeView $model_tree
   */
  public function render($model_tree) {
    $icons = create_icon(Image::Add, AjaxAction::Form, 'icm_add_form', array($this->contentModel->pid));
    $forms_tree_node = $model_tree->addChild('Forms', 'folder', $icons);
    $this->renderEachForm($forms_tree_node);
  }

  /**
   *
   * @param TreeView $model_tree
   */
  private function renderEachForm($model_tree) {
    $names = $this->contentModel->getFormNames();
    foreach ($names as $name) {
      $form = $this->contentModel->getForm($name);
      $icons = $this->getFormIcons($name, $form);
      $form_tree_node = $model_tree->addChild("<b>Form: $name</b> ", 'folder', $icons);
      $this->renderEachFormElement($name, $form, $form_tree_node);
    }
  }

  /**
   *
   * @param String $form_name
   * @param DOMElement $form_element
   * @param TreeView $model_tree
   */
  private function renderEachFormElement($form_name, $form_element, $model_tree) {
    $this->forEachChildElementDo($form_element, 'renderFormElement', array(&$form_name, &$model_tree));
  }

  /**
   *
   * @param DOMElement $child
   * @param String $form_name
   * @param TreeView $model_tree
   */
  private function renderFormElement($child, &$form_name, &$model_tree) {
    $name = $child->tagName;
    $title = $child->getElementsByTagName('title')->item(0)->textContent;
    $icons = $this->getFormElementIcons($form_name, $child);
    $tree_node = $model_tree->addChild("<b>$name, ($title)</b> ", 'folder', $icons);
    $content = $child->getElementsByTagName('content');
    if ($content && $content->length > 0) {
      $this->renderEachFormElement($form_name, $content->item(0), $tree_node);
    }
  }

  /**
   *
   * @param DOMElement $element
   * @param String $function
   * @param Array $params
   */
  private function forEachChildElementDo($element, $function, $params) {
    if (!method_exists($this, $function))
      return FALSE;
    $children = $element->childNodes;
    array_unshift($params, $element); // Dummy placeholder for each child.
    for ($i = 0; $i < $children->length; $i++) {
      $child = $children->item($i);
      if (get_class($child) != 'DOMElement') {
        continue;
      }
      $params[0] = &$child;
      call_user_func_array(array($this, $function), $params);
    }
  }

  /**
   *
   * @param DOMElement $child
   * @param DOMElement $form_element
   * @param int $count
   * @param Boolean $found
   */
  private function findChildIndex(&$child, &$form_element, &$count, &$found) {
    if (!$found) {
      if ($child->isSameNode($form_element)) {
        $found = TRUE;
      }
      else {
        $count++;
      }
    }
  }

  /**
   *
   * @param DOMElement $child
   * @param Array $path
   * @param int $count
   * @param DOMElement $element
   */
  private function findFormElement($child, &$index, &$count, &$element) {
    if ($index == $count) {
      $element = $child;
    }
    $count++;
  }

  /**
   *
   * @param String $form_name
   * @param Array $path
   * @return DOMElement
   */
  private function findFormElementFromPath($form_name, $path = NULL) {
    $form = $this->getFormElement($form_name);
    $form_element = $form;
    if (isset($path)) {
      $path = explode(' ', $path);
      while (true) {
        $index = array_shift($path);
        if (isset($index) && $index != '') {
          $count = 0;
          $child_element = ''; // Place holder
          $this->forEachChildElementDo($form_element, 'findFormElement', array(&$index, &$count, &$child_element));
          $form_element = $child_element;
        }
        else {
          break;
        }
      }
    }
    return $form_element;
  }

  /**
   *
   * @param DOMElement $form_element
   * @return int
   */
  private function getIndexOfChildInParent($form_element) {
    $count = 0;
    $found = FALSE;
    $this->forEachChildElementDo($form_element->parentNode, 'findChildIndex', array(&$form_element, &$count, &$found));
    return $count;
  }

  /**
   *
   * @param DOMElement $form_element
   * @return Array
   */
  private function getPathToFormElement($form_element) {
    $path[] = $this->getIndexOfChildInParent($form_element);
    if ($form_element->parentNode->tagName != 'form') {
      $path = array_merge($path, $this->getPathToFormElement($form_element->parentNode));
    }
    return $path;
  }

  /**
   *
   * @param DOMElement $form_element
   * @return Boolean
   */
  private function isFirstChildElement($form_element) {
    $prev = $form_element;
    while ($prev = $prev->previousSibling) {
      if (get_class($prev) == 'DOMElement') {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   *
   * @param DOMElement $form_element
   * @return Boolean
   */
  private function isLastChildElement($form_element) {
    $next = $form_element;
    while ($next = $next->nextSibling) {
      if (get_class($next) == 'DOMElement') {
        return FALSE;
      }
    }
    return TRUE;
  }

  /**
   * Removes a form from the content model.
   *
   * @param DOMElement $element
   * @return TRUE if successful, FALSE otherwise
   */
  public function addForm($element) {
    $result = $this->contentModel->addForm($element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   *
   * @param String $type
   * @param Array $element_properties_form_values
   * @param String $form_name
   * @param Array $path
   * @return TRUE if successful, FALSE otherwise.
   */
  public function addFormElement($type, $element_properties_form_values, $form_name, $path) {
    module_load_include('inc', 'islandora_content_modeler', 'includes/FormElementDocument');
    $form_element_document = new FormElementDocument($type, $element_properties_form_values);
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->addElementToForm($form_element_document->documentElement, $form_element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   * Removes a form from the content model.
   *
   * @param string $form_name
   *
   * @return TRUE if successful, FALSE otherwise
   */
  private function _removeForm($form_name) {
    $result = $this->contentModel->removeForm($form_name);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   *
   * @param String $form_name
   * @param Array $path
   * @return TRUE if successful FALSE otherwise.
   */
  private function _removeFormElement($form_name, $path) {
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $form_element->parentNode->removeChild($form_element);
    return $this->contentModel->saveToFedora();
  }

  /**
   * Moves an element up the list of its siblings.
   *
   * @param string $form_name
   *   Name of the form the element belongs to.
   * @param string $path
   *   Path to the form element.
   *
   * @return boolean
   *   TRUE if successful, FALSE otherwise.
   */
  private function _incrementFormElement($form_name, $path) {
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->incFormElement($form_element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   * Moves an element down the list of its siblings.
   *
   * @param string $form_name
   *   Name of the form the element belongs to.
   * @param string $path
   *   Path to the form element.
   *
   * @return boolean
   *   TRUE if successful, FALSE otherwise.
   */
  private function _decrementFormElement($form_name, $path) {
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->decFormElement($form_element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   *
   * @param <type> $type
   * @param <type> $element_properties_form_values
   * @param <type> $form_name
   * @param <type> $path
   */
  public function editFormElement($type, $element_properties_form_values, $form_name, $path) {
    module_load_include('inc', 'islandora_content_modeler', 'includes/FormElementDocument');
    $form_element_document = new FormElementDocument($type, $element_properties_form_values);
    $xml = $form_element_document->saveXML();
    $xml .= ' ';
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->editFormElement($form_element_document->documentElement, $form_element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   *
   * @param String $form_name
   * @param String $path
   * @param Boolean $edit
   * @return Array
   */
  public function getFormElementForm($form_name, $path, $edit) {
    $form = array(
      'form' => array(
        '#type' => 'fieldset',
        '#title' => '',
        'type' => array(
          '#type' => 'select',
          '#title' => t('Choose element type'),
          '#options' => array(
            'fieldset' => 'fieldset',
            'tabpanel' => 'tabpanel',
            'textfield' => 'textfield',
            'textarea' => 'textarea',
            'set' => 'set',
            'select' => 'select',
            'checkbox' => 'checkbox',
          ),
          '#ahah' => array(
            'path' => 'admin/content/modeler/ajax/element/properties',
            'event' => 'change',
            'wrapper' => AHAH_WRAPPER,
            'method' => 'replace',
            'effect' => 'fade',
          ),
        ),
        'element_properties' => array(),
        'submit_parameters' => array(
          '#type' => 'hidden',
          '#value' => $params,
        ),
        'model_pid' => array(
          '#type' => 'hidden',
          '#value' => $this->contentModel->pid,
        ),
        'name' => array(
          '#type' => 'hidden',
          '#value' => $form_name,
        ),
        'path' => array(
          '#type' => 'hidden',
          '#value' => $path,
        ),
        'submit' => array(
          '#type' => 'submit',
          '#value' => t('Save'),
        ),
        'cancel' => array(
          '#type' => 'button',
          '#value' => t('Cancel'),
          '#id' => 'cancel',
        )
      )
    );
    if ($edit) {
      $form_element = $this->findFormElementFromPath($form_name, $path);
      $type = $form_element->tagName;
      $form['form']['type']['#default_value'] = $type;
      $form['form']['element_properties'] = $this->getElementPropertiesForm($type, $form_element);
    }
    else {
      $types = array_keys($form['form']['type']['#options']);
      $form['form']['element_properties'] = $this->getElementPropertiesForm($types[0]);
    }
    return $form;
  }

  /**
   * Get the value of an attribute/element in the content models form.
   *
   * @param string $path
   *   XPath to the desired element or attribute.
   * @param DOMNode $node
   *   The context node in which to run the XPath query.
   *
   * @return string
   *   Value of the found attribute, or the text content of a found element,
   *   NULL if the element/attribute is not found.
   */
  private function getValue($path, $node = NULL) {
    if (!$node) {
      return NULL;
    }
    $xpath = new DOMXPath($node->ownerDocument);
    $xpath->registerNamespace('cm', 'http://www.islandora.ca');
    $result = $xpath->query($path, $node);
    if ($result->length == 1) {
      $target = $result->item(0);
      if (get_class($target) == 'DOMElement') {
        return $target->textContent;
      }
      else {
        return $target->value;
      }
    }
    return NULL;
  }

  /**
   * Gets options as a set.
   * 
   * @param string $path
   * @param DOMNode $node
   * 
   * @return array 
   */
  private function getOptions($path, $node = NULL) {
    if (!$node) {
      return NULL;
    }
    $xpath = new DOMXPath($node->ownerDocument);
    $xpath->registerNamespace('cm', 'http://www.islandora.ca');
    $result = $xpath->query($path, $node);
    for ($i = 0; $i < $result->length; $i++) {
      $option = $result->item($i);
      $options[] = $option->textContent;
    }
    return isset($options) ? implode(';', $options) : '';
  }

  /**
   * Tests if a element/attribute exists.
   *
   * @param string $path
   *   Path to the element.
   * @param DOMNode $node
   *   The context node in which to run the XPath query.
   *
   * @return boolean
   *   TRUE if the element/attribute exists FALSE otherwise.
   */
  private function exists($path, $node = NULL) {
    if (!$node) {
      return FALSE;
    }
    $xpath = new DOMXPath($node->ownerDocument);
    $xpath->registerNamespace('cm', 'http://www.islandora.ca');
    $result = $xpath->query($path, $node);
    if ($result->length == 1) {
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Creates a drupal form for editing/creating form elements
   * in the content model.
   *
   * @param string $type
   *   The type of form element, (textfield, fieldset, etc.)
   * @param DOMElement $node
   *   The DOMElement that defines this form element.
   *
   * @return array
   *   A drupal form.
   */
  function getElementPropertiesForm($type, $node = NULL) {
    $form = $this->getDefaultPropertiesForm($node);
    $func = 'get' . ucwords($type) . 'ElementProperties';
    if (method_exists($this, $func)) {
      $form = array_merge($form, call_user_func(array($this, $func), $node));
    }
    $form = array_merge($form, $this->getFormBuilderProperties($node));
    return array(
      '#type' => 'markup',
      '#prefix' => '<div id="' . AHAH_WRAPPER . '">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
      $form
    );
  }

  /**
   * Gets default form element properties.
   *
   * @param DOMElement $node
   *   Drupal Form element.
   *
   * @return array
   *   The default form element properties.
   */
  public function getDefaultPropertiesForm($node = NULL) {
    return array(
      'title' => array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#size' => 30,
        '#required' => TRUE,
        '#description' => t('The title of the form element.'),
        '#default_value' => $this->getValue('child::cm:title', $node),
      ),
      'description' => array(
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#rows' => 2,
        '#cols' => 80,
        '#description' => t('The description of the form element.'),
        '#default_value' => $this->getValue('child::cm:description', $node),
      ),
    );
  }

  private function getFormBuilderProperties($node = NULL) {
    $element = $this->exists('child::cm:form_builder//cm:element', $node);
    $attribute = $this->exists('child::cm:form_builder//cm:attribute', $node);
    $xml = $this->exists('child::cm:form_builder//cm:xml', $node);
    $select_value = $element ? 'Element' : ($attribute ? 'Attribute' : ($xml ? 'XML' : NULL));
    $require_value = $this->getValue('child::cm:form_builder/cm:require_value', $node);
    $require_value = isset($require_value) ? $require_value == 'true' : TRUE;
    return array(
      'form_builder' => array(
        '#type' => 'fieldset',
        '#title' => 'Form Builder',
        '#description' => t('Form Attributes used to edit XML datastreams.'),
        'path' => array(
          '#type' => 'textfield',
          '#title' => 'Path',
          '#description' => t('Path to this form element.'),
          '#default_value' => $this->getValue('child::cm:form_builder//cm:path', $node),
        ),
        'full' => array(
          '#type' => 'checkbox',
          '#title' => t('Full Path?'),
          '#description' => t('Is path full? Or is run at its parents context?'),
          '#default_value' => $this->getValue('child::cm:form_builder//cm:path/@full', $node) == 'true',
        ),
        'parent_path' => array(
          '#type' => 'textfield',
          '#title' => 'Parent Path',
          '#description' => t('Path to the parent element of this form element.'),
          '#default_value' => $this->getValue('child::cm:form_builder//cm:parent_path', $node),
        ),
        'select' => array(
          '#type' => 'select',
          '#title' => 'Select the type of Node you wish to create.',
          '#description' => t('The XML node that will be created if not found.'),
          '#options' => array(
            'Element' => 'Element',
            'Attribute' => 'Attribute',
            'XML' => 'XML'
          ),
          '#default_value' => $select_value,
        ),
        'element' => array(
          '#type' => 'textfield',
          '#title' => 'Element Name',
          '#description' => t('Element to create. Namespaces are allowed.'),
          '#default_value' => $this->getValue('child::cm:form_builder//cm:element', $node),
        ),
        'attribute' => array(
          '#type' => 'textfield',
          '#title' => 'Attribute Name',
          '#description' => t('Attribute to create. Namespaces are allowed.'),
          '#default_value' => $this->getValue('child::cm:form_builder//cm:attribute', $node),
        ),
        'xml' => array(
          '#type' => 'textarea',
          '#title' => 'XML',
          '#description' => t('A XML document fragment that will be created. It must have a root. Namespaces are allowed.'),
          '#default_value' => $this->getValue('child::cm:form_builder//cm:xml', $node),
        ),
        'require_value' => array(
          '#type' => 'checkbox',
          '#title' => t('Require Value?'),
          '#description' => t('Does this node require a value to be created?'),
          '#default_value' => $this->getValue('child::cm:form_builder/cm:require_value', $node) == 'true',
        ),
      ),
    );
  }

  /**
   *
   * @param Array $form_element
   * @return Array
   */
  private function getTextfieldElementProperties($form_element) {
    return array(
      'required' => array(
        '#type' => 'checkbox',
        '#title' => t('Required?'),
        '#description' => t('Is this form element required?'),
        '#default_value' => $this->getValue('child::cm:required', $form_element) == 'true',
      ),
      'default_value' => array(
        '#type' => 'textfield',
        '#title' => t('Default Value'),
        '#size' => 100,
        '#description' => t('Specify the default value for this textfield.'),
        '#default_value' => $this->getValue('child::cm:default_value', $form_element),
      ),
      'autocomplete_path' => array(
        '#type' => 'textfield',
        '#title' => t('Auto Complete Path'),
        '#size' => 100,
        '#description' => t('Specify the autocomplete path for this textfield.'),
        '#default_value' => $this->getValue('child::cm:autocomplete_path', $form_element),
      ),
    );
  }

  /**
   *
   * @param Array $form_element
   * @return Array
   */
  private function getTextareaElementProperties($form_element) {
    return array(
      'required' => array(
        '#type' => 'checkbox',
        '#title' => t('Required?'),
        '#description' => t('Is this form element required?'),
        '#default_value' => $this->getValue('child::cm:required', $form_element) == 'true',
      ),
      'default_value' => array(
        '#type' => 'textarea',
        '#title' => t('Default Value'),
        '#rows' => 1,
        '#cols' => 1,
        '#description' => t('Specify the default value for this textarea.'),
        '#default_value' => $this->getValue('child::cm:default_value', $form_element),
      )
    );
  }

  /**
   *
   * @param Array $form_element
   * @return Array
   */
  private function getSelectElementProperties($form_element) {
    return array(
      'required' => array(
        '#type' => 'checkbox',
        '#title' => t('Required?'),
        '#description' => t('Is this form element required?'),
        '#default_value' => $this->getValue('child::cm:required', $form_element) == 'true',
      ),
      'options' => array(
        '#type' => 'set',
        '#required' => TRUE,
        '#title' => t('Options'),
        '#description' => t('Specify the options.'),
        '#default_value' => $this->getOptions('child::cm:options/child::*', $form_element),
      ),
      'ignoreFirstOption' => array(
        '#type' => 'checkbox',
        '#title' => t('Ignore First Option'),
        '#description' => t('Should the first option be ignored?'),
        '#default_value' => $this->getValue('child::cm:options/@ignoreFirstOption', $form_element) == 'true',
      )
    );
  }

  /**
   *
   * @param Array $form_element
   * @return Array
   */
  private function getSetElementProperties($form_element) {
    return array(
      'required' => array(
        '#type' => 'checkbox',
        '#title' => t('Required?'),
        '#description' => t('Is this form element required?'),
        '#default_value' => $this->getValue('child::cm:required', $form_element) == 'true',
      ),
      'default_value' => array(
        '#type' => 'set',
        '#title' => t('Default Value'),
        '#description' => t('Specify the default value for this list.'),
        '#default_value' => $this->getValue('child::cm:default_value', $form_element),
      ),
      'autocomplete_path' => array(
        '#type' => 'textfield',
        '#title' => t('Auto Complete Path'),
        '#size' => 100,
        '#description' => t('Specify the autocomplete path for this element.'),
        '#default_value' => $this->getValue('child::cm:autocomplete_path', $form_element),
      ),
    );
  }

  /**
   *
   * @return Array
   */
  private function getFieldsetElementProperties($form_element) {
    return array(
      'collapsible' => array(
        '#type' => 'checkbox',
        '#title' => t('Collapsible?'),
        '#description' => t('Indicates whether or not the fieldset can be collapsed with JavaScript. See #collapsed property.'),
        '#default_value' => $this->getValue('child::cm:collapsible', $form_element) == 'true',
      ),
      'collapsed' => array(
        '#type' => 'checkbox',
        '#title' => t('Collapsed?'),
        '#description' => t('Indicates whether or not the fieldset is collapsed by default. See #collapsible property.'),
        '#default_value' => $this->getValue('child::cm:collapsed', $form_element) == 'true',
      ),
      'content' => array(
        '#type' => 'hidden',
        '#default_value' => NULL,
      )
    );
  }

  /**
   * @param Array $form_element
   * @return Array
   */
  private function getTabpanelElementProperties($form_element) {
    return array(
      'content' => array(
        '#type' => 'hidden',
        '#default_value' => NULL,
      ),
      'collapsible' => array(
        '#type' => 'checkbox',
        '#title' => t('Collapsible?'),
        '#description' => t('Indicates whether or not the tabpanel can be collapsed. See #collapsed property.'),
        '#default_value' => $this->getValue('child::cm:collapsible', $form_element) == 'true',
      ),
      'collapsed' => array(
        '#type' => 'checkbox',
        '#title' => t('Collapsed?'),
        '#description' => t('Indicates whether or not the tabpanel is collapsed by default.'),
        '#default_value' => $this->getValue('child::cm:collapsed', $form_element) == 'true',
      ),
    );
  }

  /**
   * @param Array $form_element
   * @return Array
   */
  private function getCheckboxElementProperties($form_element) {
    return array(
      'return_value' => array(
        '#type' => 'textfield',
        '#title' => t('Return Value'),
        '#size' => 100,
        '#description' => t('Specify the return value for this checkbox.'),
        '#default_value' => $this->getValue('child::cm:return_value', $form_element),
      ),
    );
  }

}

/**
 *
 * @param <type> $element
 * @param <type> $form_state
 */
function icm_validate_form_name($element, &$form_state) {
  if (form_get_errors() !== NULL)
    return;
  module_load_include('inc', 'fedora_repository', 'ContentModel');
  $model_pid = trim($form_state['values']['form']['model_pid']);
  $content_model = ContentModel::loadFromModel($model_pid);
  $form_names = $content_model->getFormNames();
  $name = trim($element['#value']);
  foreach ($form_names as $value) {
    if ($name == $value) {
      form_error($element, t('Error: The specified name <b>%name</b> is already in use.', array('%name' => $name)));
      return;
    }
  }
  $result = stripos($name, ' ');
  if ($result !== FALSE) {
    form_error($element, t('Error: The specified name <b>%name</b> is not valid.', array('%name' => $name)));
    return;
  }
}

function icm_validate_form_name_edit($element, &$form_state) {
  $result = stripos($name, ' ');
  if ($result !== FALSE) {
    form_error($element, t('Error: The specified name <b>%name</b> is not valid.', array('%name' => $name)));
    return;
  }
}

/**
 *
 * @param Array $form_state
 * @param Array $params
 * @return Array
 */
function icm_add_form(&$form_state, $params) {
  if (is_array($params)) {
    list($model_pid) = Forms::expandParameters($params);
  }
  else {
    $model_pid = $form_state['post']['form']['model_pid'];
  }
  $form['form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a form to <b>%model_pid</b>', array('%model_pid' => $model_pid)),
    '#tree' => TRUE,
    'model_pid' => array(
      '#type' => 'hidden',
      '#value' => $model_pid
    ),
    'name' => array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#size' => 30,
      '#required' => TRUE,
      '#description' => t('The name of the form.'),
      '#element_validate' => array('icm_validate_form_name'),
    ),
    'dsid' => array(
      '#type' => 'textfield',
      '#title' => t('Datastream Identifier'),
      '#size' => 30,
      '#maxlength' => 64,
      '#required' => TRUE,
      '#description' => t('The datastream ID that will store the output from this method.'),
    ),
    'ingest_module' => array(
      '#type' => 'select',
      '#title' => t('Ingest Module'),
      '#attributes' => array('class' => 'form-builder'),
      '#element_validate' => array('icm_module_validate'),
      '#options' => icm_get_modules(),
      '#required' => FALSE,
      '#description' => t('The name of the module containing the ingest form builder file/class.'),
    ),
    'ingest_filename' => array(
      '#type' => 'select',
      '#title' => t('Ingest File'),
      '#element_validate' => array('icm_filename_validate'),
      '#required' => FALSE,
      '#description' => t('The relative path of the file containing the ingest form builder class.'),
    ),
    'ingest_class' => array(
      '#type' => 'select',
      '#title' => t('Ingest Class'),
      '#element_validate' => array('icm_class_validate'),
      '#required' => FALSE,
      '#description' => t('The name of the ingest form builder class.'),
    ),
    'edit_module' => array(
      '#type' => 'select',
      '#title' => t('Edit Module'),
      '#attributes' => array('class' => 'form-builder'),
      '#element_validate' => array('icm_module_validate'),
      '#options' => icm_get_modules(),
      '#required' => FALSE,
      '#description' => t('The name of the module containing the edit form builder file/class.'),
    ),
    'edit_filename' => array(
      '#type' => 'select',
      '#title' => t('Edit File'),
      '#element_validate' => array('icm_filename_validate'),
      '#required' => FALSE,
      '#description' => t('The relative path of the file containing the edit form builder class.'),
    ),
    'edit_class' => array(
      '#type' => 'select',
      '#title' => t('Edit Class'),
      '#element_validate' => array('icm_class_validate'),
      '#required' => FALSE,
      '#description' => t('The name of the edit form builder class.'),
    ),
    'document_module' => array(
      '#type' => 'select',
      '#title' => t('Document Module'),
      '#attributes' => array('class' => 'form-builder'),
      '#element_validate' => array('icm_module_validate'),
      '#options' => icm_get_modules(),
      '#required' => TRUE,
      '#description' => t('The name of the module containing the metadata document file.'),
    ),
    'document_filename' => array(
      '#type' => 'select',
      '#title' => t('Document File'),
      '#element_validate' => array('icm_filename_validate'),
      '#required' => TRUE,
      '#description' => t('The relative path of the file containing the metatdata document generator class.'),
    ),
    'document_class' => array(
      '#type' => 'select',
      '#title' => t('Document Class'),
      '#element_validate' => array('icm_class_validate'),
      '#required' => TRUE,
      '#description' => t('The name of the metadata document generator class.'),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save')
    ),
    'cancel' => array(
      '#type' => 'button',
      '#value' => t('Cancel'),
      '#id' => 'cancel'
    )
  );
  return $form;
}

/**
 *
 * @param Array $form
 * @param Array $form_state
 */
function icm_add_form_submit($form, &$form_state) {
  if (form_get_errors() !== NULL)
    return;
  $model_pid = trim($form_state['values']['form']['model_pid']);
  $form_name = $form_state['values']['form']['name'];
  $dsid = $form_state['values']['form']['dsid'];
  $ingest_module = $form_state['values']['form']['ingest_module'];
  $ingest_file = $form_state['values']['form']['ingest_filename'];
  $ingest_class = $form_state['values']['form']['ingest_class'];
  $edit_module = $form_state['values']['form']['edit_module'];
  $edit_file = $form_state['values']['form']['edit_filename'];
  $edit_class = $form_state['values']['form']['edit_class'];
  $document_module = $form_state['values']['form']['document_module'];
  $document_file = $form_state['values']['form']['document_filename'];
  $document_class = $form_state['values']['form']['document_class'];

  $document = new DOMDocument();
  $element = $document->createElement('form');
  $document->appendChild($element);
  $element->setAttribute('name', $form_name);
  $element->setAttribute('dsid', $dsid);
  $element->setAttribute('document_module', $document_module);
  $element->setAttribute('document_file', $document_file);
  $element->setAttribute('document_class', $document_class);

  if(trim($ingest_module) != '' && trim($ingest_file) != '' && trim($ingest_class) != '') {
    $element->setAttribute('ingest_module', $ingest_module);
    $element->setAttribute('ingest_file', $ingest_file);
    $element->setAttribute('ingest_class', $ingest_class);
  }

  if(trim($edit_module) != '' && trim($edit_file) != '' && trim($edit_class) != '') {
    $element->setAttribute('edit_module', $edit_module);
    $element->setAttribute('edit_file', $edit_file);
    $element->setAttribute('edit_class', $edit_class);
  }

  $form_tree = Forms::createFromPID($model_pid);
  if ($form_tree->addForm($document->documentElement)) {
    echo 'success:' . t('Added form <b>%name</b> to model <b>%model_pid</b>.', array('%name' => $form_name, '%model_pid' => $model_pid));
    exit();
  }
  else {
    form_set_error('form][name', t('Error: Unable to add form <b>%name</b> to model <b>%model_pid</b>.', array('%name' => $form_name, '%model_pid' => $model_pid)));
  }
}

/**
 *
 * @param Array $form_state
 * @param Array $params
 * @return Array
 */
function icm_edit_form(&$form_state, $params) {
  if (is_array($params)) {
    list($model_pid, $form_name) = Forms::expandParameters($params);
  }
  else {
    $model_pid = $form_state['post']['form']['model_pid'];
    $form_name = $form_state['post']['form']['form_name'];
  }
  $forms = Forms::createFromPID($model_pid);
  $form_element = $forms->getFormElement($form_name);
  $form['form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit form <b>%name</b> from model <b>$model_pid</b>', array('%name' => $form_name, '%model_pid' => $model_pid)),
    '#tree' => TRUE,
    'old_name' => array(
      '#type' => 'hidden',
      '#value' => $form_name,
    ),
    'model_pid' => array(
      '#type' => 'hidden',
      '#value' => $model_pid,
    ),
    'name' => array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#size' => 30,
      '#required' => TRUE,
      '#description' => t('The name of the form.'),
      '#element_validate' => array('icm_validate_form_name'),
    ),
    'dsid' => array(
      '#type' => 'textfield',
      '#title' => t('Datastream Identifier'),
      '#size' => 30,
      '#maxlength' => 64,
      '#required' => TRUE,
      '#description' => t('The datastream ID that will store the output from this method.'),
    ),
    'ingest_module' => array(
      '#type' => 'select',
      '#title' => t('Ingest Module'),
      '#attributes' => array('class' => 'form-builder'),
      '#element_validate' => array('icm_module_validate'),
      '#options' => icm_get_modules(),
      '#required' => TRUE,
      '#description' => t('The name of the module containing the ingest form builder file/class.'),
    ),
    'ingest_filename' => array(
      '#type' => 'select',
      '#title' => t('Ingest File'),
      '#element_validate' => array('icm_filename_validate'),
      '#required' => TRUE,
      '#description' => t('The relative path of the file containing the ingest form builder class.'),
    ),
    'ingest_class' => array(
      '#type' => 'select',
      '#title' => t('Ingest Class'),
      '#element_validate' => array('icm_class_validate'),
      '#required' => TRUE,
      '#description' => t('The name of the ingest form builder class.'),
    ),
    'edit_module' => array(
      '#type' => 'select',
      '#title' => t('Edit Module'),
      '#attributes' => array('class' => 'form-builder'),
      '#element_validate' => array('icm_module_validate'),
      '#options' => icm_get_modules(),
      '#required' => TRUE,
      '#description' => t('The name of the module containing the edit form builder file/class.'),
    ),
    'edit_filename' => array(
      '#type' => 'select',
      '#title' => t('Edit File'),
      '#element_validate' => array('icm_filename_validate'),
      '#required' => TRUE,
      '#description' => t('The relative path of the file containing the edit form builder class.'),
    ),
    'edit_class' => array(
      '#type' => 'select',
      '#title' => t('Edit Class'),
      '#element_validate' => array('icm_class_validate'),
      '#required' => TRUE,
      '#description' => t('The name of the edit form builder class.'),
    ),
    'document_module' => array(
      '#type' => 'select',
      '#title' => t('Document Module'),
      '#attributes' => array('class' => 'form-builder'),
      '#element_validate' => array('icm_module_validate'),
      '#options' => icm_get_modules(),
      '#required' => TRUE,
      '#description' => t('The name of the module containing the metadata document file.'),
    ),
    'document_filename' => array(
      '#type' => 'select',
      '#title' => t('Document File'),
      '#element_validate' => array('icm_filename_validate'),
      '#required' => TRUE,
      '#description' => t('The relative path of the file containing the metatdata document generator class.'),
    ),
    'document_class' => array(
      '#type' => 'select',
      '#title' => t('Document Class'),
      '#element_validate' => array('icm_class_validate'),
      '#required' => TRUE,
      '#description' => t('The name of the metadata document generator class.'),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save')
    ),
    'cancel' => array(
      '#type' => 'button',
      '#value' => t('Cancel'),
      '#id' => 'cancel'
    )
  );
  return $form;
}

/**
 *
 * @param Array $form
 * @param Array $form_state
 */
function icm_edit_form_submit($form, &$form_state) {
  if (form_get_errors() !== NULL)
    return;
  $model_pid = trim($form_state['values']['form']['model_pid']);
  $old_name = $form_state['values']['form']['old_name'];
  $form_name = $form_state['values']['form']['name'];
  $dsid = $form_state['values']['form']['dsid'];
  $ingest_module = $form_state['values']['form']['ingest_module'];
  $ingest_file = $form_state['values']['form']['ingest_filename'];
  $ingest_class = $form_state['values']['form']['ingest_class'];
  $edit_module = $form_state['values']['form']['edit_module'];
  $edit_file = $form_state['values']['form']['edit_filename'];
  $edit_class = $form_state['values']['form']['edit_class'];
  $document_module = $form_state['values']['form']['document_module'];
  $document_file = $form_state['values']['form']['document_filename'];
  $document_class = $form_state['values']['form']['document_class'];

  $document = new DOMDocument();
  $element = $document->createElement('form');
  $document->appendChild($element);
  $element->setAttribute('name', $form_name);
  $element->setAttribute('dsid', $dsid);
  $element->setAttribute('document_module', $document_module);
  $element->setAttribute('document_file', $document_file);
  $element->setAttribute('document_class', $document_class);
  
  if(trim($ingest_module) != '' && trim($ingest_file) != '' && trim($ingest_class) != '') {
    $element->setAttribute('ingest_module', $ingest_module);
    $element->setAttribute('ingest_file', $ingest_file);
    $element->setAttribute('ingest_class', $ingest_class);
  }

  if(trim($edit_module) != '' && trim($edit_file) != '' && trim($edit_class) != '') {
    $element->setAttribute('edit_module', $edit_module);
    $element->setAttribute('edit_file', $edit_file);
    $element->setAttribute('edit_class', $edit_class);
  }

  $content_model = ContentModel::loadFromModel($model_pid);
  if ($content_model) {
    if ($content_model->editForm($old_name, $document->documentElement) && $content_model->saveToFedora()) {
      echo 'success:' . t('Edited form <b>%old_name</b> from model <b>%model_pid</b>.', array('%old_name' => $old_name, '%model_pid' => $model_pid));
      exit();
    }
  }
  form_set_error('form][name', t('Error: Unable to edit form <b>%old_name</b> from model <b>%model_pid</b>.', array('%old_name' => $old_name, '%model_pid' => $model_pid)));
}

/**
 *
 * @param Array $form_state
 * @param Array $params
 * @return Array
 */
function icm_add_form_element(&$form_state, $params) {
  if (is_array($params)) {
    list($model_pid, $form_name, $path) = Forms::expandParameters($params);
  }
  else {
    $model_pid = $form_state['post']['form']['model_pid'];
    $form_name = $form_state['post']['form']['name'];
    $path = $form_state['post']['form']['path'];
  }
  $form_tree = Forms::createFromPID($model_pid);
  $form = $form_tree->getFormElementForm($form_name, $path, FALSE);
  $form['#title'] = t('Add element to form <b>%name</b>', array('%name' => $form_name));
  return $form;
}

/**
 *
 * @param Array $form
 * @param Array $form_state
 */
function icm_add_form_element_submit($form, &$form_state) {
  if (form_get_errors() !== NULL)
    return;
  $model_pid = trim($form_state['values']['model_pid']);
  $form_name = trim($form_state['values']['name']);
  $type = trim($form_state['values']['type']);
  $path = trim($form_state['values']['path']);
  $form_tree = Forms::createFromPID($model_pid);
  $element_properties = $form_state['values']['element_properties'][0];
  if ($form_tree->addFormElement($type, $element_properties, $form_name, $path)) {
    echo 'success:' . t('Added a <b>%type_name</b> to <b>%form_name</b>.', array('%type_name' => $element_type, '%form_name' => $form_name));
    exit();
  }
  else {
    echo t('Error: Unable to add a <b>%type_name</b> to <b>%form_name</b>.', array('%type_name' => $element_type, '%form_name' => $form_name));
    exit();
  }
}

/**
 *
 * @param <type> $form_state
 * @param <type> $params
 * @return <type>
 */
function icm_edit_form_element(&$form_state, $params) {
  if (is_array($params)) {
    list($model_pid, $form_name, $path) = Forms::expandParameters($params);
  }
  else {
    $model_pid = $form_state['post']['form']['model_pid'];
    $form_name = $form_state['post']['form']['name'];
    $path = $form_state['post']['form']['path'];
  }
  $form_tree = Forms::createFromPID($model_pid);
  $form = $form_tree->getFormElementForm($form_name, $path, TRUE);
  $form['#title'] = t('Edit Form Element');
  return $form;
}

/**
 *
 * @param <type> $form
 * @param <type> $form_state
 */
function icm_edit_form_element_submit($form, &$form_state) {
  if (form_get_errors() !== NULL)
    return;
  $model_pid = trim($form_state['values']['model_pid']);
  $form_name = trim($form_state['values']['name']);
  $type = trim($form_state['values']['type']);
  $path = trim($form_state['values']['path']);
  $form_tree = Forms::createFromPID($model_pid);
  $element_properties = $form_state['values']['element_properties'][0];
  if ($form_tree->editFormElement($type, $element_properties, $form_name, $path)) {
    echo 'success:' . t('Added a <b>%type_name</b> to <b>%form_name</b>.', array('%type_name' => $element_type, '%form_name' => $form_name));
    exit();
  }
  else {
    echo t('Error: Unable to add a <b>%type_name</b> to <b>%form_name</b>.', array('%type_name' => $element_type, '%form_name' => $form_name));
    exit();
  }
}