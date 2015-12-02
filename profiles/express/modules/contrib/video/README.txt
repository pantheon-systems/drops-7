# Video module 2 for Drupal 7

This readme file is still under construction.

## Troubleshooting

### FFmpeg errors

#### "File for preset 'xyz' not found"

Select "None" in the "FFmpeg video preset" drop down for your preset.

#### "broken ffmpeg default settings detected" "use an encoding preset (vpre)"

Select "libx264-default" in the "FFmpeg video preset" drop down for your preset.

#### "Could not write header for output file #0"

You probably selected the wrong codec for your extension. For instance,
for MP4 you need to select the libx264 video codec and AAC audio codec.

#### "constant rate-factor is incompatible with 2pass"

Either enable "Force one-pass encoding" for your preset, or set the 
"Video bitrate" setting to some bitrate.

#### "Additional information: rc_twopass_stats_in requires at least two packets."

Enable "Force one-pass encoding" for your preset.

#### "video codec not compatible with flv"

Choose a different video codec for your FLV preset.

Examples of codecs that should work:

- Flash Video (FLV) / Sorenson Spark / Sorenson H.263
- libx264 H.264 / AVC / MPEG-4 AVC / MPEG-4 part 10

#### "[aac @ 0x______] Too many bits per frame requested"

The sample rate of your input video is not valid for AAC audio encoding.
Edit your preset and set "Audio sample rate" to 44100 in the
"Advanced audio settings" section.
