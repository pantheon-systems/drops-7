<?php

class BeanEntityCacheAPIController extends BeanEntityAPIController {

  public function resetCache(array $ids = NULL) {
    EntityCacheControllerHelper::resetEntityCache($this, $ids);
    parent::resetCache($ids);
  }

}
