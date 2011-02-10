<?php

// $Id$

/**
 * @file
 *
 * A collection of helper ajax methods that return html used in making interactive forms.
 */
class HelperMethods {
  /**
   * Gets an array of modules that exist in the fedora repository package.
   *
   * @return Array
   *   An array of module names.
   */
  public static function getIslandoraModules() {
    $files = module_rebuild_cache();
    $options = array('');
    foreach ($files as $key => $val) {
      //only list islandora modules
      if ($val->info['package'] == 'Fedora Repository') {
        $options[$key] = $key;
      }
    }
    return $options;
  }
}
