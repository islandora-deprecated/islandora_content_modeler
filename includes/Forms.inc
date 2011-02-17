<?php

// $Id$

/**
 * @file
 */
module_load_include('inc', 'fedora_repository', 'ContentModel');
module_load_include('inc', 'islandora_content_modeler', 'includes/FormElementDocument');

/**
 *
 */
class Forms {

  protected $contentModel;

  /**
   *
   * @global String $base_url
   * @param ContentModel $content_model
   */
  public function __construct($content_model, $collection_policy) {
    global $base_url;
    $this->moduleRoot = $base_url . '/' . drupal_get_path('module', 'islandora_content_modeler');
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

  public static function getElementProperties() {
    if (!isset($_POST['form_build_id']))
      return;
    $form_state = array('storage' => NULL, 'submitted' => FALSE, 'post' => $_POST);
    $form_build_id = $_POST['form_build_id'];
    if (!$form = form_get_cache($form_build_id, $form_state))
      exit();
    $form_tree = Forms::createFromPID($form_state['post']['form']['model_pid']);
    $form['form']['element_properties'] = $form_tree->getElementPropertiesForm($form_state['post']['form']['type']);
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

//  /**
//   *
//   * @param String $icon
//   * @param Boolean $is_form
//   * @param String $function
//   * @param Array $params
//   * @return HTML
//   */
//  function create_icon($icon, $is_form, $function, $params) {
//    $class = ($is_form) ? 'ajaxFormIcon' : 'ajaxButtonIcon';
//    $arguments = implode(' ', $params);
//    return "<img src='" . $this->moduleRoot . "/images/$icon' class='$class' id='$function $arguments'/>" . " ";
//  }

  /**
   *
   * @param String $form_name
   * @return HTML
   */
  function getFormIcons($form_name, $form) {
    $params = array($this->contentModel->pid, $form_name);
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'icm_remove_form', $params);
    $icons .= create_icon(Image::Add, AjaxAction::Form, 'icm_add_form_element', $params);
    if (!$this->isFirstChildElement($form)) {
      $icons .= create_icon(Image::Up, AjaxAction::Button, 'icm_inc_form_element', $params);
    }
    if (!$this->isLastChildElement($form)) {
      $icons .= create_icon(Image::Down, AjaxAction::Button, 'icm_dec_form_element', $params);
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
    $icons = create_icon(Image::Remove, AjaxAction::Button, 'icm_remove_form_element', $params);
    $result = $form_element->getElementsByTagName('content');
    if ($result && $result->length) {
      $content = $result->item(0);
      $path_to_content = array_reverse($this->getPathToFormElement($content));
      $add_params = array($this->contentModel->pid, $form_name, implode(' ', $path_to_content));
      $icons .= create_icon(Image::Add, AjaxAction::Form, 'icm_add_form_element', $add_params);
    }
    if (!$this->isFirstChildElement($form_element)) {
      $icons .= create_icon(Image::Up, AjaxAction::Button, 'icm_inc_form_element', $params);
    }
    if (!$this->isLastChildElement($form_element)) {
      $icons .= create_icon(Image::Down, AjaxAction::Button, 'icm_dec_form_element', $params);
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
    $xml = $this->contentModel->dumpXML();
    $doc = new DOMDocument();
    $doc->loadXML($xml);
    $xpath = new DOMXPath($doc);
    $xpath->registerNamespace('cm', 'http://www.islandora.ca');
    $result = $xpath->query('//cm:form');
    for($i = 0; $i < $result->length; $i++) {
      $form = $result->item($i);
      $name = $form->getAttribute('name');
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
  private function findFormElementFromPath($form_name, $path) {
    $form = $this->contentModel->getFormElement($form_name);
    $form_element = $form;
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
   * @param String $form_name
   * @return TRUE if successful, False otherwise
   */
  public function removeForm($form_name) {
    $result = $this->contentModel->removeForm($form_name);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   * Removes a form from the content model.
   *
   * @param String $form_name
   * @return TRUE if successful, FALSE otherwise
   */
  public function addForm($form_name) {
    $result = $this->contentModel->addForm($form_name);
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
    $form_element_document = new FormElementDocument($type, $element_properties_form_values);
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->addElementToForm($form_element_document->documentElement, $form_element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   *
   * @param String $form_name
   * @param Array $path
   * @return TRUE if successful FALSE otherwise.
   */
  public function removeFormElement($form_name, $path) {
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->removeElementFromForm($form_element);
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
    $form_element_document = new FormElementDocument($type, $element_properties_form_values);
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->editFormElement($form_element_document->documentElement, $form_element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   *
   * @param String $form_name
   * @param String $path
   */
  public function incFormElement($form_name, $path) {
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->incFormElement($form_element);
    return $result && $this->contentModel->saveToFedora();
  }

  /**
   *
   * @param String $form_name
   * @param String $path
   */
  public function decFormElement($form_name, $path) {
    $form_element = $this->findFormElementFromPath($form_name, $path);
    $result = $this->contentModel->decFormElement($form_element);
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
        '#tree' => TRUE,
        'type' => array(
          '#type' => 'select',
          '#title' => t('Choose element type'),
          '#tree' => TRUE,
          '#options' => array(
            'fieldset' => 'fieldset',
            'tabpanel' => 'tabpanel',
            'textfield' => 'textfield',
            'textarea' => 'textarea',
            'list' => 'list',
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
   *
   * @param <type> $form_element
   * @param <type> $tag_name
   *
   * @return
   */
  private function getElementValue($tag_name, $form_element = NULL) {
    if ($form_element) {
      $result = $form_element->getElementsByTagName($tag_name);
      if ($result && $result->length == 1) {
        return $result->item(0)->textContent;
      }
    }
    return NULL;
  }

  private function getAttributeValue($tag_name, $attribute_name, $form_element = NULL) {
    if ($form_element) {
      $result = $form_element->getElementsByTagName($tag_name);
      if ($result && $result->length == 1) {
        return $result->item(0)->getAttribute($attribute_name);
      }
    }
    return NULL;
  }

  /**
   *
   * @param <type> $element_type
   * @return <type>
   */
  function getElementPropertiesForm($element_type, $form_element = NULL) {
    $form = $this->_getElementPropertiesForm($form_element);
    $func = 'get' . ucwords($element_type) . 'ElementProperties';
    if (method_exists($this, $func)) {
      $form = array_merge($form, call_user_func(array($this, $func), $form_element));
    }
    return array(
      '#type' => 'markup',
      '#prefix' => '<div id="' . AHAH_WRAPPER . '">',
      '#suffix' => '</div>',
      $form
    );
  }

  /**
   *
   * @return <type>
   */
  public function _getElementPropertiesForm($form_element) {
    return array(
      '#tree' => TRUE,
      'title' => array(
        '#type' => 'textfield',
        '#title' => t('title'),
        '#size' => 30,
        '#required' => TRUE,
        '#description' => t('The title of the form element.'),
        '#default_value' => $this->getElementValue('title', $form_element),
      ),
      'description' => array(
        '#type' => 'textarea',
        '#title' => t('Description'),
        '#rows' => 1,
        '#cols' => 1,
        '#description' => t('The description of the form element.'),
        '#default_value' => $this->getElementValue('description', $form_element),
      ),
      'xpath' => array(
        '#type' => 'textfield',
        '#title' => t('XPath'),
        '#size' => 100,
        '#description' => t('The xpath of the form element.'),
        '#default_value' => $this->getElementValue('xpath', $form_element),
      ),
      'full_path' => array(
        '#type' => 'checkbox',
        '#title' => t('XPath Full?'),
        '#description' => t('Is the xpath complete? Or is it build with the help of a parent element?'),
        '#default_value' => $this->getAttributeValue('xpath', 'full_path', $form_element) == 'true',
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
        '#default_value' => $this->getElementValue('required', $form_element) == 'true',
      ),
      'default_value' => array(
        '#type' => 'textfield',
        '#title' => t('Default Value'),
        '#size' => 100,
        '#description' => t('Specify the default value for this textfield.'),
        '#default_value' => $this->getElementValue('default_value', $form_element),
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
        '#default_value' => $this->getElementValue('required', $form_element) == 'true',
      ),
      'default_value' => array(
        '#type' => 'textarea',
        '#title' => t('Default Value'),
        '#rows' => 1,
        '#cols' => 1,
        '#description' => t('Specify the default value for this textarea.'),
        '#default_value' => $this->getElementValue('default_value', $form_element),
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
        '#default_value' => $this->getElementValue('required', $form_element) == 'true',
      ),
      'options' => array(
        '#type' => 'list',
        '#required' => TRUE,
        '#title' => t('Options'),
        '#description' => t('Specify the options.'),
        '#default_value' => $this->getElementValue('options', $form_element),
      ),
      'ignoreFirstOption' => array(
        '#type' => 'checkbox',
        '#title' => t('Ignore First Option'),
        '#description' => t('Should the first option be ignored?'),
        '#default_value' => $this->getAttributeValue('options', 'ignoreFirstOption', $form_element) == 'true',
      )
    );
  }

  /**
   *
   * @param Array $form_element
   * @return Array
   */
  private function getListElementProperties($form_element) {
    return array(
      'required' => array(
        '#type' => 'checkbox',
        '#title' => t('Required?'),
        '#description' => t('Is this form element required?'),
        '#default_value' => $this->getElementValue('required', $form_element) == 'true',
      ),
      'default_value' => array(
        '#type' => 'list',
        '#title' => t('Default Value'),
        '#description' => t('Specify the default value for this list.'),
        '#default_value' => $this->getElementValue('default_value', $form_element),
      )
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
        '#default_value' => $this->getElementValue('collapsible', $form_element) == 'true',
      ),
      'collapsed' => array(
        '#type' => 'checkbox',
        '#title' => t('Collapsed?'),
        '#description' => t('Indicates whether or not the fieldset is collapsed by default. See #collapsible property.'),
        '#default_value' => $this->getElementValue('collapsed', $form_element) == 'true',
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
      )
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
        '#default_value' => $this->getElementValue('return_value', $form_element),
      ),
    );
  }

}

/**
 * Below are hook/helper functions for the various forms that are related to the Forms.
 *
 */
function icm_process_params($params) {
  if (is_array($params)) {
    list($model_pid, $form_name) = $params;
    $path = implode(' ', array_splice($params, 2));
    return array($model_pid, $form_name, $path);
  }
  return array();
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

/**
 *
 * @param Array $params
 */
function icm_remove_form($params) {
  list($model_pid, $form_name) = $params;
  $form_tree = Forms::createFromPID($model_pid);
  if ($form_tree->removeForm($form_name)) {
    echo 'success:' . t('Removed form <b>%name</b> from model <b>%model_pid</b>.', array('%name' => $form_name, '%model_pid' => $model_pid));
  }
  else {
    echo t('Error: Unable to remove form <b>%name</b> from model <b>%model_pid</b>.', array('%name' => $name, '%model_pid' => $model_pid));
  }
  exit();
}

/**
 *
 * @param Array $params
 */
function icm_inc_form_element($params) {
  list($model_pid, $form_name, $path) = icm_process_params($params);
  $form_tree = Forms::createFromPID($model_pid);
  if ($form_tree->incFormElement($form_name, $path)) {
    echo 'success:' . t('Successful.');
  }
  else {
    echo t('Error:');
  }
  exit();
}

/**
 *
 * @param Array $params
 */
function icm_dec_form_element($params) {
  list($model_pid, $form_name, $path) = icm_process_params($params);
  $form_tree = Forms::createFromPID($model_pid);
  if ($form_tree->decFormElement($form_name, $path)) {
    echo 'success:' . t('Successful');
  }
  else {
    echo t('Error:');
  }
  exit();
}

/**
 *
 * @param Array $params
 */
function icm_remove_form_element($params) {
  list($model_pid, $form_name, $path) = icm_process_params($params);
  $form_tree = Forms::createFromPID($model_pid);
  if ($form_tree->removeFormElement($form_name, $path)) {
    echo 'success:' . t('Removed form element from model <b>%model_pid</b>.', array('%name' => $form_name, '%model_pid' => $model_pid));
  }
  else {
    echo t('Error: Unable to remove form element from model <b>%model_pid</b>.', array('%name' => $form_name, '%model_pid' => $model_pid));
  }
  exit();
}

/**
 *
 * @param Array $form_state
 * @param Array $params
 * @return Array
 */
function icm_add_form(&$form_state, $params) {
  if (is_array($params)) {
    list($model_pid) = icm_process_params($params);
  }
  else {
    $model_pid = $form_state['post']['form']['model_pid'];
  }
  $form['form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a form to <b>%model_pid</b>', array('%model_pid' => $model_pid)),
    '#tree' => TRUE,
    'name' => array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#size' => 30,
      '#required' => TRUE,
      '#description' => t('The name of the form.'),
      '#element_validate' => array('icm_validate_form_name'),
    ),
    'model_pid' => array(
      '#type' => 'hidden',
      '#value' => $model_pid
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
  $form_tree = Forms::createFromPID($model_pid);
  if ($form_tree->addForm($form_name)) {
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
    list($model_pid, $form_name) = icm_process_params($params);
  }
  else {
    $model_pid = $form_state['post']['form']['model_pid'];
    $form_name = $form_state['post']['form']['name'];
  }
  list($model_pid, $form_name) = icm_process_params($params);
  $form['form'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit form <b>%name</b> from model <b>$model_pid</b>', array('%name' => $form_name, '%model_pid' => $model_pid)),
    '#tree' => TRUE,
    'name' => array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#size' => 30,
      '#required' => TRUE,
      '#description' => t('The new name of the form.'),
      '#element_validate' => array('icm_validate_form_name'),
      '#default_value' => $form_name,
    ),
    'old_name' => array(
      '#type' => 'hidden',
      '#value' => $form_name,
    ),
    'model_pid' => array(
      '#type' => 'hidden',
      '#value' => $model_pid,
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
    'cancel' => array(
      '#type' => 'button',
      '#value' => t('Cancel'),
      '#id' => 'cancel',
    ),
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
  $content_model = ContentModel::loadFromModel($model_pid);
  $old_name = $form_state['values']['form']['old_name'];
  $form_name = $form_state['values']['form']['name'];
  if ($content_model) {
    if ($content_model->editForm($old_name, $form_name) && $content_model->saveToFedora()) {
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
    list($model_pid, $form_name, $path) = icm_process_params($params);
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
  $model_pid = trim($form_state['values']['form']['model_pid']);
  $form_name = trim($form_state['values']['form']['name']);
  $type = trim($form_state['values']['form']['type']);
  $path = trim($form_state['values']['form']['path']);
  $form_tree = Forms::createFromPID($model_pid);
  $element_properties = $form_state['values']['form']['element_properties'][0];
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
    list($model_pid, $form_name, $path) = icm_process_params($params);
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
  $model_pid = trim($form_state['values']['form']['model_pid']);
  $form_name = trim($form_state['values']['form']['name']);
  $type = trim($form_state['values']['form']['type']);
  $path = trim($form_state['values']['form']['path']);
  $form_tree = Forms::createFromPID($model_pid);
  $element_properties = $form_state['values']['form']['element_properties'][0];
  if ($form_tree->editFormElement($type, $element_properties, $form_name, $path)) {
    echo 'success:' . t('Added a <b>%type_name</b> to <b>%form_name</b>.', array('%type_name' => $element_type, '%form_name' => $form_name));
    exit();
  }
  else {
    echo t('Error: Unable to add a <b>%type_name</b> to <b>%form_name</b>.', array('%type_name' => $element_type, '%form_name' => $form_name));
    exit();
  }
}