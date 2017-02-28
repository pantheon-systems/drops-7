Feature: CU Image Styles
  When I log into the website
  As a developer I should see the proper domensions for image styles

@api @image-styles
Scenario Outline: Image styles should be the correct dimensions
  Given  CU - I am logged in as a user with the "developer" role
  When I go to "admin/config/media/image-styles/edit/<style>"
  Then I should see <message>

  Examples:
  | style                     | message                                   |
  | hero                      | "Scale width 1500 (upscaling allowed)"    |
  | backstretch               | "Scale 1500x1000"                         |
  | large_square_thumbnail    | "Focal Point Scale And Crop 600x600"      |
  | large_wide_thumbnail      | "Focal Point Scale And Crop 600x300"      |
  | preview                   | "Scale 240x240 (upscaling allowed)"       |
  | small                     | "Scale width 300 (upscaling allowed)"     |
  | small_thumbnail           | "Scale width 100"                         |
  | square                    | "Focal Point Scale And Crop 180x180"      |
  | square_thumbnail          | "Focal Point Scale And Crop 180x180"      |
  | small_square_thumbnail    | "Focal Point Scale And Crop 70x70"        |
  | slider                    | "Focal Point Scale And Crop 1500x563"     |
  | slider-large              | "Focal Point Scale And Crop 1500x1000"    |
  | el_hero                   | "Scale width 1500 (upscaling allowed)"    |
  | flexslider_full           | "Scale and crop 800x500"                  |
  | flexslider_thumbnail      | "Scale and crop 160x100"                  |
  | focal_point_preview       | "Scale width 250 (upscaling allowed)"     |
  | thumbnail                 | "Scale 100x100 (upscaling allowed)"       |
  | medium                    | "Scale width 600 (upscaling allowed)"     |
  | large                     | "Scale width 1200 (upscaling allowed)"    |
  | linkit_thumb              | "Scale 50x50"                             |
