<?php

// $Id$

/**
 * @file
 *
 * Utility class used to generate the strings for tree view to repersent Icons.
 */

/**
 * Denotes the a root of this module.
 */
global $base_url;
define('CONTENT_MODELER_ROOT', $base_url . '/' . drupal_get_path('module', 'islandora_content_modeler'));

/**
 * Faking enum's for php. These values repersent the different images that
 * can be used with create_icon function.
 */
class Image {
  const Add = 'add.png';
  const Remove = 'remove.png';
  const Edit = 'edit.gif';
  const Info = 'red_info.png';
  const View = 'view.gif';
  const Up = 'blue_up.png';
  const Down = 'blue_down.png';
}

/**
 * Faking enum's for php. These values repersent the different ajax actions
 * that an icon will execute when clicked.
 */
class AjaxAction {
  /**
   * The callback must return a drupal form to be rendered.
   */
  const Form = 'ajaxFormIcon';
  /**
   * The callback preforms some action, no form is generated.
   */
  const Button = 'ajaxButtonIcon';
}

/**
 * Creates a string that repersents an icon for treeview to render.
 *
 * @param Image $icon
 *   @see Image
 * @param AjaxAction $ajax_action
 *   @see AjaxAction
 * @param String $callback
 *   A callback for when a users clicks on this icon.
 * @param Array $arguments
 *   An array of strings that will be passed to the callback.
 *
 * @return String
 *   A String
 */
function create_icon($icon, $ajax_action, $callback, $arguments) {
  return '<img src="' . CONTENT_MODELER_ROOT . '/images/' . $icon . '" class="' . $ajax_action . '" id="' . $callback . ' ' . implode(' ', $arguments) . '"/>';
}