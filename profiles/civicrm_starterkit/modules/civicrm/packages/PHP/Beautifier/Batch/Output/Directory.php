<?php
/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */
/**
* PHP_Beautifier_Batch_Files
* Handle the batch process for multiple php files to one directory
*
* PHP version 5
*
* LICENSE: This source file is subject to version 3.0 of the PHP license
* that is available through the world-wide-web at the following URI:
* http://www.php.net/license/3_0.txt.  If you did not receive a copy of
* the PHP License and are unable to obtain it through the web, please
* send a note to license@php.net so we can mail you a copy immediately.
* @category   PHP
* @package PHP_Beautifier
* @subpackage Batch
* @author Claudio Bustos <cdx@users.sourceforge.com>
* @copyright  2004-2006 Claudio Bustos
* @link     http://pear.php.net/package/PHP_Beautifier
* @link     http://beautifyphp.sourceforge.net
* @license    http://www.php.net/license/3_0.txt  PHP License 3.0
* @version    CVS: $Id:$
*/
/**
* PHP_Beautifier_Batch_Files
* Handle the batch process for multiple php files to one directory
*
* @category   PHP
* @package PHP_Beautifier
* @author Claudio Bustos <cdx@users.sourceforge.com>
* @copyright  2004-2006 Claudio Bustos
* @link     http://pear.php.net/package/PHP_Beautifier
* @link     http://beautifyphp.sourceforge.net
* @license    http://www.php.net/license/3_0.txt  PHP License 3.0
* @version    Release: 0.1.14
*/
class PHP_Beautifier_Batch_Output_Directory extends PHP_Beautifier_Batch_Output {
    public function save() 
    {
        $aInputFiles = $this->oBatch->getInputFiles();
        $sOutputPath = $this->oBatch->getOutputPath();
        $aOutputFiles = PHP_Beautifier_Common::getSavePath($aInputFiles, $sOutputPath);
        $oLog = PHP_Beautifier_Common::getLog();
        for ($x = 0;$x<count($aInputFiles);$x++) {
            try {
                $this->beautifierSetInputFile($aInputFiles[$x]);
                $this->beautifierProcess();
                PHP_Beautifier_Common::createDir($aOutputFiles[$x]);
                $this->beautifierSave($aOutputFiles[$x]);
            }
            catch(Exception $oExp) {
                $oLog->log($oExp->getMessage() , PEAR_LOG_ERR);
            }
        }
        return true;
    }
    /**
    * Send the output of the files, one after another
    * With a little header
    * @return string
    */
    public function get() 
    {
        $aInputFiles = $this->oBatch->getInputFiles();
        $sText = '';
        foreach($aInputFiles as $sFile) {
            $this->beautifierSetInputFile($sFile);
            $this->beautifierProcess();
            $sText.= $this->beautifierGet()."\n";
        }
        return $sText;
    }
}
?>