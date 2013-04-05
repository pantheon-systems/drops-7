The current class library organization is a bridge between the autoloading schemes for Drupal 7 and 8. The
locations of the classes are PSR-0 compatible and meant to work with the Symfony2 autoloader when this
module is upgraded to Drupal 8.

In the meantime, it still uses entries in the module's .info file and non-namespaced classes for compatibility with the D7 autoloader and version of PHP earlier than 5.3.