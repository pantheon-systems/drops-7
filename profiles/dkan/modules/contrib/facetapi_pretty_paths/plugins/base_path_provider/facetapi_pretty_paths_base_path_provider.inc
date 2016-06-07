<?php

/**
 * @file
 * Base path provider interface.
 */

/**
 * Default FacetApiPrettyPathsBasePathProvider.
 */
interface FacetApiPrettyPathsBasePathProvider {

  /**
   * @param FacetapiUrlProcessorPrettyPaths $urlProcessor
   * @return base path.
   */
  public function getBasePath(FacetapiUrlProcessorPrettyPaths $urlProcessor);

}
