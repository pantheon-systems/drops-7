Feature: CU Image Styles
  When I log into the website
  As a developer I should see the proper domensions for image styles

@api @image-styles
Scenario Outline: Image styles should be the correct dimensions
  Given I am logged in as a user with the "developer" role
  When I go to "admin/config/media/image-styles/edit/<style>"
  Then I should see <message>

  Examples:
  | style                     | message                                   |
  | hero                      | "Scale width 1500 (upscaling allowed)"    |
  | backstretch               | "Scale 1500x1000"                         |

  | large_wide_thumbnail      | "Focal Point Scale And Crop 480x240"      |
  | large_square_thumbnail    | "Focal Point Scale And Crop 480x480"      |

  | small                     | "Scale width 240 (upscaling allowed)"     |
  | small_thumbnail           | "Scale width 100"                         |
  | square                    | "Focal Point Scale And Crop 180x180"      |
  | slider                    | "Focal Point Scale And Crop 960x360"      |
  | slider-large              | "Focal Point Scale And Crop 960x640"      |
  | thumbnail                 | "Scale 100x100 (upscaling allowed)"       |
  | medium                    | "Scale width 480 (upscaling allowed)"     |
  | large                     | "Scale width 960"                         |
  | square                    | "Scale and crop 180x180"                  |
