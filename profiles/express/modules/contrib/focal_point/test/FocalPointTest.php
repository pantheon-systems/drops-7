<?php

/**
 * @file
 * PHP Unit tests for focal point.
 */

include_once __DIR__ . './../focal_point.module';
include_once __DIR__ . './../focal_point.effects.inc';

class focalPointTest extends PHPUnit_Framework_TestCase {

  /**
   * @dataProvider parseFocalPointProvider
   */
  public function testFocalPoint($focal_point, $expected) {
    $this->assertSame($expected, focal_point_parse($focal_point));
  }

  public function parseFocalPointProvider() {
    return array(
      array('23,56', array('x-offset' => '23', 'y-offset' => '56')),
      array('56,23', array('x-offset' => '56', 'y-offset' => '23')),
      array('0,0', array('x-offset' => '0', 'y-offset' => '0')),
      array('100,100', array('x-offset' => '100', 'y-offset' => '100')),
      array('', array('x-offset' => '50', 'y-offset' => '50')),
    );
  }

  /**
   * @dataProvider calculateEffectAnchorProvider
   */
  public function testCalculateEffectAnchor($image_size, $crop_size, $focal_point_offset, $focal_point_shift, $expected) {
    $this->assertSame($expected, focal_point_effect_calculate_anchor($image_size, $crop_size, $focal_point_offset, $focal_point_shift));
  }

  public function calculateEffectAnchorProvider() {
    return array(
      array(640, 300, 50, 0, 170),
      array(640, 300, 80, 0, 340),
      array(640, 300, 10, 0, 0),
      array(640, 640, 640, 0, 0),
      array(640, 800, 50, 0, 0),
      array(640, 300, 50, 10, 160),
      array(640, 300, 50, -10, 180),
      array(640, 300, 50, 10000, 0),
      array(640, 300, 50, -10000, 340),
    );
  }

  /**
   * @dataProvider resizeDataProvider
   */
  public function testResizeData($image_width, $image_height, $crop_width, $crop_heigt, $expected) {
    $this->assertSame($expected, focal_point_effect_resize_data($image_width, $image_height, $crop_width, $crop_heigt));
  }

  public function resizeDataProvider() {
    return array(
      array(640,480,300,100,array('width' => 300, 'height' => 225)), // Horizontal image with horizontal crop
      array(640,480,100,300,array('width' => 400, 'height' => 300)), // Horizontal image with vertical crop
      array(480,640,300,100,array('width' => 300, 'height' => 400)), // Vertical image with horizontal crop
      array(480,640,100,300,array('width' => 225, 'height' => 300)), // Vertical image with vertical crop
      array(640,480,3000,1000,array('width' => 3000, 'height' => 2250)), // Horizontal image with too large crop
      array(1920,1080,400,300,array('width' => 533, 'height' => 300)), // Image would be too small to crop after resize
    );
  }

}
