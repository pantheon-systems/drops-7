<?php

/**
 * @file
 * The default base path provider.
 */

/**
 * Default FacetApiPrettyPathsBasePathProvider.
 */
class FacetApiPrettyPathsDefaultBasePathProvider implements FacetApiPrettyPathsBasePathProvider {

  /**
   * @param FacetapiUrlProcessorPrettyPaths $urlProcessor
   * @return base path.
   */
  public function getBasePath(FacetapiUrlProcessorPrettyPaths $urlProcessor) {
    return $urlProcessor->getPathWithoutSegments();
  }

}
