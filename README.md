# flipper-zero-video-player
 An application for playing videos (with sound) on Flipper Zero.
# How to use:

First, place your video (`source.mp4`) in some empty folder, create "frames" folder there and run this (install ffmpeg if you don't have it) (change the 1280x720 resolution to match your source video resolution if necessary, change 96x64 resolution according to your video aspect ratio if necessary):

YOUR HORIZONTAL RESOLUTION MUST BE A MULTIPLE OF 8!! 96 WORK AND 128 WORK BUT E.G. 86 DOES NOT!

```
ffmpeg -i source.mp4 -f lavfi -i color=gray:s=1280x720 -f lavfi -i color=black:s=1280x720 -f lavfi -i color=white:s=1280x720 -filter_complex "[0:v]scale=1280x720,threshold" -s 96x64 -pix_fmt rgb24 frames/frame%07d.bmp
```

The filter is needed to ensure that we are not getting any grayscale, black and white only.

After this, extract the audio:

```
ffmpeg -i source.mp4 -ac 1 -acodec pcm_u8 -ar 44100 audio.wav
```

We need mono 8 bit unsigned PCM audio for Flipper so yeah

After that run the Visual Studio project, don't forget to change the user settings: path to your folder, FPS, aspect ratio of images (if necessary, default one is for 4:3 video).

Then you will have `bundle.bnd` file. Rename it if you want but keep the extension. Copy it to `apps_data/video_player`. If the folder does not exist, create it. Since file would be very large, I advise to remove SD card from Flipper and connect it to your PC/laptop somehow, or wait all the night when it uploads via qFlipper.
