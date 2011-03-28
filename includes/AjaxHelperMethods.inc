<?php

// $Id$

/**
 * @file
 *
 * A collection of helper ajax methods, that are called via page callbacks.
 */

/**
 * Collection of static helper methods
 */
class AjaxHelperMethods {

  /**
   * Loads all the files in the includes directory.
   */
  public static function loadIncludeFiles() {
    $include_path = drupal_get_path('module', 'islandora_content_modeler') . "/includes";
    $dir = opendir($include_path);
    if ($dir !== false) {
      while (($file = readdir($dir)) !== false) {
        if (preg_match('/\.inc$/', $file)) {
          require_once "$include_path/$file";
        }
      }
    }
    closedir($dir);
  }

  /**
   * Builds/Validates/Submits the specified form.
   *
   * @param string $form_name
   */
  public static function processForm($form_name) {
    self::loadIncludeFiles();
    $params = null;
    if (isset($_GET['formReq'])) {
      $params = preg_split('/\s+/', trim($_GET['formReq']));
    }
    $form_content = drupal_get_form($form_name, $params);
    echo theme_status_messages('error');
    echo $form_content;
  }

  /**
   * Executes the specified function passing on any arguments as well.
   *
   * Each icon that has the class 'ajaxButtonIcon' can specify a callback
   * function, and arguments, in its 'id', attribute. These functions are
   * executed here.
   */
  public static function executeButtonPressAction() {
    self::loadIncludeFiles();
    module_load_include('inc', 'fedora_repository', 'CollectionPolicy');
    module_load_include('inc', 'fedora_repository', 'ContentModel');
    $params = null;
    if (isset($_GET['formReq'])) {
      $params = preg_split('/\s+/', trim($_GET['formReq']));
    }

    $func = trim($params[0]);
    if (is_callable($func)) {
      array_shift($params);
      call_user_func($func, $params);
      exit();
    }

    switch (strtolower($params[0])) {
      case 'icm_model_remove_editmetadatamethod':
        if (count($params) == 2) {
          $model_pid = $params[1];
          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeEditMetadataMethod() || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove edit metadata method from model <b>%cm_pid</b>.', array('%cm_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed edit metadata method from model <b>%cm_pid</b>.', array('%cm_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to remove edit metadata method. Please try again.');
        }
        break;

      case 'icm_model_remove_authlistitem':
        if (count($params) == 4) {
          $model_pid = $params[1];
          $elementName = $params[2];
          $authValue = $params[3];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeAuthListItem($elementName, $authValue) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove authoritative list item <b>%value</b> in model <b>%m_pid</b>.', array('%value' => $authValue, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed authoritative list item <b>%value</b> in model <b>%m_pid</b>.', array('%value' => $authValue, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to remove authoritative list item. Please try again.');
        }
        break;

      case 'icm_model_inc_authlistitem':
        if (count($params) == 4) {
          $model_pid = $params[1];
          $elementName = $params[2];
          $authValue = $params[3];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->incAuthListItem($elementName, $authValue) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to increment authoritative list item <b>%value</b> in model <b>%m_pid</b>.', array('%value' => $authValue, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully incremented authoritative list item <b>%value</b> in model <b>%m_pid</b>.', array('%value' => $authValue, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to increment authoritative list item. Please try again.');
        }
        break;

      case 'icm_model_dec_authlistitem':
        if (count($params) == 4) {
          $model_pid = $params[1];
          $elementName = $params[2];
          $authValue = $params[3];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->decAuthListItem($elementName, $authValue) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to decrement authoritative list item <b>%value</b> in model <b>%m_pid</b>.', array('%value' => $authValue, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully decremented authoritative list item <b>%value</b> in model <b>%m_pid</b>.', array('%value' => $authValue, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to decrement authoritative list item. Please try again.');
        }
        break;

      case 'icm_model_remove_ingestformelement':
        if (count($params) == 3) {
          $model_pid = $params[1];
          $name = $params[2];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeIngestFormElement($name) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove ingest form element <b>%name</b> from model <b>%m_pid</b>.', array('%name' => $name, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed ingest form element <b>%name</b> from model <b>%m_pid</b>.', array('%name' => $name, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to remove ingest form element. Please try again.');
        }
        break;

      case 'icm_model_inc_ingestformelement':
        if (count($params) == 3) {
          $model_pid = $params[1];
          $name = $params[2];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->incIngestFormElement($name) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to increment ingest form element <b>%name</b> from model <b>%m_pid</b>.', array('%name' => $name, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully incremented ingest form element <b>%name</b> from model <b>%m_pid</b>.', array('%name' => $name, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to increment ingest form element. Please try again.');
        }
        break;

      case 'icm_model_dec_ingestformelement':
        if (count($params) == 3) {
          $model_pid = $params[1];
          $name = $params[2];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->decIngestFormElement($name) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to decrement ingest form element <b>%name</b> from model <b>%m_pid</b>.', array('%name' => $name, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully decremented ingest form element <b>%name</b> from model <b>%m_pid</b>.', array('%name' => $name, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to decrement ingest form element. Please try again.');
        }
        break;


      case 'icm_model_remove_dispmeth':
        if (count($params) == 7) {
          $model_pid = $params[1];
          $dsid = $params[2];
          $module = $params[3];
          $file = $params[4];
          $class = $params[5];
          $method = $params[6];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeDispMeth($dsid, $module, $file, $class, $method) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove display method for datastream <b>%dsid</b> of model <b>%m_pid</b>.', array('%dsid' => $dsid, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully remove display method for datastream <b>%dsid</b> of model <b>%m_pid</b>.', array('%dsid' => $dsid, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to remove display method of datastream.');
        }
        break;

      case 'icm_model_default_dispmeth':
        if (count($params) == 7) {
          $model_pid = $params[1];
          $dsid = $params[2];
          $module = $params[3];
          $file = $params[4];
          $class = $params[5];
          $method = $params[6];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->setDefaultDispMeth($dsid, $module, $file, $class, $method) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to set default display method for datastream <b>%dsid</b> of model <b>%m_pid</b>.', array('%dsid' => $dsid, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully set default display method for datastream <b>%dsid</b> of model <b>%m_pid</b>.', array('%dsid' => $dsid, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters to set default display method of datastream.');
        }
        break;

      case 'icm_model_remove_ds':
        if (count($params) == 3) {

          $model_pid = $params[1];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeDs($params[2]) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove datastream from model <b>%m_pid</b>.', array('%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed datastream model <b>%m_pid</b>.', array('%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters for removal of application mimetype.');
        }
        break;
      case 'icm_model_remove_appliesto':
        if (count($params) == 4) {

          $model_pid = $params[1];
          $rule_id = $params[2];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeAppliesTo($rule_id, $params[3]) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove application mimetype from <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed application mimetype from <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters for removal of application mimetype.');
        }
        break;


      case 'icm_model_remove_ingestrule':

        if (count($params) == 3) {

          $model_pid = $params[1];
          $rule_id = $params[2];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeIngestRule($rule_id) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove ingest <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed ingest <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters for removal of ingest rule.');
        }
        break;

      case 'icm_model_remove_ingestmethod':
        if (count($params) == 7) {

          $model_pid = $params[1];
          $rule_id = $params[2];
          $module = $params[3];
          $file = $params[4];
          $class = $params[5];
          $method = $params[6];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeIngestMethod($rule_id, $module, $file, $class, $method) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove ingest method of <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed ingest method of <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters for removal of ingest method.');
        }
        break;

      case 'icm_model_remove_ingestmethodparam':

        if (count($params) == 8) {

          $model_pid = $params[1];
          $rule_id = $params[2];
          $module = $params[3];
          $file = $params[4];
          $class = $params[5];
          $method = $params[6];
          $name = $params[7];


          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->removeIngestMethodParam($rule_id, $module, $file, $class, $method, $name) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove parameter of <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed parameter of <b>Rule %rule_id</b> in model <b>%m_pid</b>.', array('%rule_id' => $rule_id, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters for removal of ingest method parameter.');
        }

        break;

      case 'icm_model_remove_ingestelementparam':

        if (count($params) == 4) {

          $model_pid = $params[1];
          $element_name = $params[2];
          $name = $params[3];

          if (($content_model = ContentModel::loadFromModel($model_pid)) !== FALSE) {
            if (!$content_model->setIngestFormElementParam($element_name, $name, FALSE) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to remove parameter of element <b>%element</b> in model <b>%m_pid</b>.', array('%element' => $element_name, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully removed parameter of element <b>%element</b> in model <b>%m_pid</b>.', array('%element' => $element_name, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters for removal of form element parameter.');
        }

        break;

      case 'icm_model_toggle_dsdisplay':
        if (isset($params[1]) && isset($params[2])) {
          if (($content_model = ContentModel::loadFromModel($params[1])) !== FALSE) {
            $dsid = trim($params[2]);
            if ($dsid == '') {
              echo t('Error: Datastream missing or not specified.  Please try again.');
            }
            else if (!$content_model->setDisplayInFieldset($dsid, !$content_model->displayInFieldset($dsid)) || !$content_model->saveToFedora()) {
              echo t('Error: Unable to update datastream <b>%dsid</b> in model <b>%m_pid</b>.', array('%dsid' => $dsid, '%m_pid' => $content_model->pid));
            }
            else {
              echo 'success:' . t('Successfully updated datastream <b>%dsid</b> in model <b>%m_pid</b>', array('%dsid' => $dsid, '%m_pid' => $content_model->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown content model. Please try again.');
          }
        }
        else {
          echo t('Error: Missing parameters for toggle datastream display in fieldset.');
        }
        break;

      case 'icm_collection_remove_cmodel':

        if (isset($params[1]) && isset($params[2])) {
          $cp = CollectionPolicy::loadFromCollection($params[1]);
          $content_model = ContentModel::loadFromModel($params[2]);
          if ($content_model !== false && $cp !== false) {
            if (!$cp->removeModel($content_model) || !$cp->saveToFedora()) {
              echo t('Error: Unable to remove content model <b>%m_pid</b> from collection policy of <b>%c_pid</b>.', array('%m_pid' => $content_model->pid, '%c_pid' => $cp->pid));
            }
            else {
              echo 'success:' . t('Successfully removed content model <b>%m_pid</b> from collection policy of <b>%c_pid</b>.', array('%m_pid' => $content_model->pid, '%c_pid' => $cp->pid));
              exit();
            }
          } else
            echo t('Error: Unknown collection policy or content model.  Please try again.');
        } else
          echo t('Error: Unknown collection policy or content model.  Please try again.');
        break;

      case 'icm_collection_default_term':
        $c_pid = $params[1];
        if (($cp = CollectionPolicy::loadFromCollection(trim($c_pid))) !== FALSE) {
          $field = isset($params[2]) ? $params[2] : '';
          if (trim($field) != '') {
            if (!$cp->setDefaultTerm(htmlentities($field)) || !$cp->saveToFedora()) {
              echo t('Error: Unable to set default search term to <b>%field</b> in collection policy <b>%cp_id</b>.', array('%field' => htmlentities($field), '%pc_pid' => $cp->pid));
            }
            else {
              echo 'success:' . t('Successfully set default search term <b>%field</b> in collection policy <b>%cp_id</b>.', array('%field' => htmlentities($field), '%cp_pid' => $cp->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown search term <b>%field</b> selected.  Please try again.', array('%field' => htmlentities($field)));
          }
        }
        else {
          echo t('Error: Unknown collection policy.  Please try again.');
        }
        break;


      case 'icm_collection_remove_term':
        $c_pid = $params[1];
        if (($cp = CollectionPolicy::loadFromCollection(trim($c_pid))) !== FALSE) {
          $field = isset($params[2]) ? $params[2] : '';
          if (trim($field) != '') {
            if (!$cp->removeTerm(htmlentities($field)) || !$cp->saveToFedora()) {
              echo t('Error: Unable to remove search term <b>%field</b> from collection policy <b>%cp_id</b>.', array('%field' => htmlentities($field), '%c_pid' => $cp->pid));
            }
            else {
              echo 'success:' . t('Successfully removed search term <b>%field</b> from collection policy <b>%cp_id</b>.', array('%field' => htmlentities($field), '%c_pid' => $cp->pid));
              exit();
            }
          }
          else {
            echo t('Error: Unknown search term <b>%field</b> selected for removal.  Please try again.', array('%field' => htmlentities($field)));
          }
        }
        else {
          echo t('Error: Unknown collection policy.  Please try again.');
        }
        break;

      case 'icm_model_remove_mime':
        $m_pid = $params[1];
        if (($content_model = ContentModel::loadFromModel(trim($m_pid))) !== FALSE) {
          $type = isset($params[2]) ? trim($params[2]) : '';
          if ($type == '') {
            echo t('Error: You must specify a mimetype to remove.  Please try again.');
          }
          else {
            if ($content_model->removeMimetype($type) && $content_model->saveToFedora()) {
              echo 'success:' . t('Successfully removed mime type <b>%type</b> from content model <b>%m_pid</b>.', array('%type' => htmlentities($type), '%m_pid' => htmlentities($m_pid)));
              exit();
            }
            else {
              echo t('Error: Unable to remove mime type <b>%type</b> from content model <b>%m_pid</b>.  Please make sure that it isnt the only term left in the content model.', array('%type' => htmlentities($type), '%m_pid' => htmlentities($m_pid)));
            }
          }
        }
        else {
          echo t('Error: Unknown content model.  Please try again.');
        }
        break;

      default:
        echo t('Error: Unknown action <b>%action</b>  Please try again', array('%action' => $params[0]));
    }
  }

  /**
   * Returns html options for a select box.
   * The options consist of files within the plugin directory of the specified module.
   */
  public static function getPlugins() {
    echo '<option></option>';
    $plugin_path = drupal_get_path('module', $_GET['module']);
    if (!$plugin_path) {
      exit();
    }
    $plugin_path.='/plugins';
    $files = array('');
    if (is_dir($plugin_path) && ($dir = opendir($plugin_path)) !== false) {
      while (($file = readdir($dir)) !== false) {
        if (preg_match('/\.inc$/', $file)) {
          echo '<option ' . (isset($_GET['file']) && $_GET['file'] == 'plugins/' . $file ? 'selected="selected"' : '') . '>plugins/' . $file . '</option>';
        }
      }
    }
    exit();
  }

  /**
   * 
   */
  public static function getForms() {
    echo '<option></option>';
    $plugin_path = drupal_get_path('module', $_GET['module']);
    if (!$plugin_path) {
      exit();
    }
    $plugin_path.='/forms';
    $files = array('');
    if (is_dir($plugin_path) && ($dir = opendir($plugin_path)) !== false) {
      while (($file = readdir($dir)) !== false) {
        if (preg_match('/\.inc$/', $file)) {
          echo '<option ' . (isset($_GET['file']) && $_GET['file'] == 'forms/' . $file ? 'selected="selected"' : '') . '>forms/' . $file . '</option>';
        }
      }
    }
    exit();
  }

  public static function getDocuments() {
    echo '<option></option>';
    $plugin_path = drupal_get_path('module', $_GET['module']);
    if (!$plugin_path) {
      exit();
    }
    $plugin_path.='/documents';
    $files = array('');
    if (is_dir($plugin_path) && ($dir = opendir($plugin_path)) !== false) {
      while (($file = readdir($dir)) !== false) {
        if (preg_match('/\.inc$/', $file)) {
          echo '<option ' . (isset($_GET['file']) && $_GET['file'] == 'documents/' . $file ? 'selected="selected"' : '') . '>documents/' . $file . '</option>';
        }
      }
    }
    exit();
  }

  public static function getBuilders() {
    echo '<option></option>';
    $plugin_path = drupal_get_path('module', $_GET['module']);
    if (!$plugin_path) {
      exit();
    }
    $plugin_path.='/builders';
    $files = array('');
    if (is_dir($plugin_path) && ($dir = opendir($plugin_path)) !== false) {
      while (($file = readdir($dir)) !== false) {
        if (preg_match('/\.inc$/', $file)) {
          echo '<option ' . (isset($_GET['file']) && $_GET['file'] == 'builders/' . $file ? 'selected="selected"' : '') . '>builders/' . $file . '</option>';
        }
      }
    }
    exit();
  }

  /**
   * Returns html options for a select box.
   * The options consist of classes within the specified plugine file.
   */
  public static function getClasses() {
    echo '<option></option>';
    $file = drupal_get_path('module', $_GET['module']);
    if (!$file) {
      exit();
    }
    $file.= '/' . $_GET['file'];
    if (file_exists($file)) {
      // hack..  we should really enforce the class name being the same as the file name.
      // either that or consider replacing with a plugin register call in each included plugin.
      $before_classes = get_declared_classes();
      require_once $file;
      $after_classes = get_declared_classes();
      foreach ($after_classes as $class) {
        if (!in_array($class, $before_classes)) {
          echo '<option ' . (isset($_GET['className']) && $_GET['className'] == $class ? 'selected="selected"' : '') . '>' . $class . '</option>';
        }
      }
    }
    exit();
  }

  /**
   * Returns html options for a select box.
   * The options consist of methods within the specified class.
   */
  public static function getMethods() {
    echo '<option></option>';
    $file = drupal_get_path('module', $_GET['module']);
    if (!$file) {
      exit();
    }
    $file.= '/' . $_GET['file'];
    $class = $_GET['className'];
    if (file_exists($file)) {
      require_once $file;
      if (class_exists($class)) {
        $methods = get_class_methods($class);
        foreach ($methods as $method) {
          echo '<option ' . (isset($_GET['method']) && $_GET['method'] == $method ? ' selected="selected"' : '') . '>' . $method . '</option>';
        }
      }
    }
    exit();
  }

}