<?php
/**
 * DataObjects error handler, loaded on demand...
 *
 * DB_DataObject_Error is a quick wrapper around pear error, so you can distinguish the
 * error code source.
 *
 * PHP versions 4 and 5
 *
 * LICENSE: This source file is subject to version 3.01 of the PHP license
 * that is available through the world-wide-web at the following URI:
 * http://www.php.net/license/3_01.txt.  If you did not receive a copy of
 * the PHP License and are unable to obtain it through the web, please
 * send a note to license@php.net so we can mail you a copy immediately.
 *
 * @category   Database
 * @package    DB_DataObject
 * @author     Alan Knowles <alan@akbkhome.com>
 * @copyright  1997-2006 The PHP Group
 * @license    http://www.php.net/license/3_01.txt  PHP License 3.01
 * @version    CVS: $Id: Error.php 287158 2009-08-12 13:58:31Z alan_k $
 * @link       http://pear.php.net/package/DB_DataObject
 */
  
 
class DB_DataObject_Error extends PEAR_Error
{
    
    /**
     * DB_DataObject_Error constructor.
     *
     * @param mixed   $code   DB error code, or string with error message.
     * @param integer $mode   what "error mode" to operate in
     * @param integer $level  what error level to use for $mode & PEAR_ERROR_TRIGGER
     * @param mixed   $debuginfo  additional debug info, such as the last query
     *
     * @access public
     *
     * @see PEAR_Error
     */
    function DB_DataObject_Error($message = '', $code = DB_ERROR, $mode = PEAR_ERROR_RETURN,
              $level = E_USER_NOTICE)
    {
        $this->PEAR_Error('DB_DataObject Error: ' . $message, $code, $mode, $level);
        
    }
    
    
    // todo : - support code -> message handling, and translated error messages...
    
    
    
}
