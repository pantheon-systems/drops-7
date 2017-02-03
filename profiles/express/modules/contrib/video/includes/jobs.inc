<?php
/**
 * @file
 * Static class containing transcoding job related operations.
 */

class video_jobs {
  /**
   * Create new transcoder job in the database.
   */
  public static function create($fid, $dimensions, $entity_id, $entity_type, $field_name, $langcode, $delta) {
    $video = new stdClass();
    $video->fid = intval($fid);
    $video->entity_id = intval($entity_id);
    $video->entity_type = $entity_type;
    $video->status = VIDEO_RENDERING_PENDING;
    $video->statusupdated = time();
    $video->dimensions = $dimensions;
    $video->data = array(
      'field_name' => $field_name,
      'langcode' => $langcode,
      'delta' => intval($delta),
    );

    return drupal_write_record('video_queue', $video) === SAVED_NEW ? $video : FALSE;
  }

  public static function update(stdClass $video) {
    // Rewrite the status attribute
    // @todo rename the status field in the video_queue table to something that doesn't
    // collide with the file_managed table.
    $oldstatus = $video->status;
    $video->status = $video->video_status;
    $result = drupal_write_record('video_queue', $video, 'fid') === SAVED_UPDATED;
    $video->status = $oldstatus;

    return $result;
  }

  /**
   * Delete transcoder job and its details from database.
   *
   * @todo improve this method: not everything is deleted.
   */
  public static function delete($fid) {
    db_delete('video_queue')->condition('fid', $fid)->execute();
    db_delete('video_output')->condition('original_fid', $fid)->execute();
  }

  /**
   * Load transcoding job from the database.
   */
  public static function load($fid) {
    $job = db_query('SELECT vf.*, f.*, vf.status as video_status FROM {video_queue} vf LEFT JOIN {file_managed} f ON vf.fid = f.fid WHERE f.fid=vf.fid AND vf.fid = :fid', array(':fid' => $fid))->fetch();
    if (empty($job)) {
      return FALSE;
    }

    $job->data = empty($job->data) ? NULL : unserialize($job->data);

    return $job;
  }

  /**
   * Select videos from our queue.
   *
   * Up to 'video_ffmpeg_instances' videos are returned.
   * The status of all returned videos is set to VIDEO_RENDERING_INQUEUE.
   */
  public static function loadQueue() {
    $total_videos = variable_get('video_ffmpeg_instances', 5);
    $now = time();
    $videos = array();
    $result = db_query_range('SELECT vf.*, f.*, vf.status as video_status FROM {video_queue} vf LEFT JOIN {file_managed} f ON vf.fid = f.fid WHERE vf.status = :vstatus AND f.status = :fstatus ORDER BY f.timestamp', 0, $total_videos, array(':vstatus' => VIDEO_RENDERING_PENDING, ':fstatus' => FILE_STATUS_PERMANENT));
    foreach ($result as $row) {
      $row->video_status = VIDEO_RENDERING_INQUEUE;
      $row->statusupdated = $now;
      $row->data = empty($row->data) ? NULL : unserialize($row->data);
      self::update($row);
      $videos[] = $row;
    }
    return $videos;
  }

  public static function setCompleted(stdClass $video) {
    self::setCompletedOrFailed($video, VIDEO_RENDERING_COMPLETE, 'video_success');
  }

  public static function setFailed(stdClass $video) {
    self::setCompletedOrFailed($video, VIDEO_RENDERING_FAILED, 'video_failed');
  }

  private static function setCompletedOrFailed(stdClass $video, $newstatus, $rulesevent) {
    // Check if the status is already set right to prevent double events.
    if ($video->video_status == $newstatus) {
      return;
    }

    $video->video_status = $newstatus;
    $video->completed = $video->statusupdated = time();
    self::update($video);

    // Clear the cache for the associated entity
    video_utility::clearEntityCache($video->entity_type, $video->entity_id);

    // Invoke Rules
    if (module_exists('rules') && $video->entity_type == 'node') {
      rules_invoke_event($rulesevent, node_load($video->entity_id));
    }
  }
}
