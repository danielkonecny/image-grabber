# Image Grabber
Image Grabber allows you to capture images (JPG) or video (AVI) with multiple Basler USB Cameras at once. It logs all 
captured data to a CSV file for later use.

## How to launch
In the project root folder:
1. Build and compile: `make`.
2. Run with help: `./ImageGrabber -h`.
3. Run with the arguments according to help, for example: `./ImageGrabber -v -f 60 -o "/myoutput"`.
4. Exit the app with `Ctrl+C`.

## Arguments
* `-R` (`--bwr`) - Set balance white (red channel), larger than 0 (default: auto continuous). (=1 red intensity 
  unchanged, >1 intensity increased, <1 intensity decreased).
* `-G` (`--bwg`) - Set balance white (green channel), larger than 0 (default: auto continuous). (=1 green intensity 
  unchanged, >1 intensity increased, <1 intensity decreased).
* `-B` (`--bwb`) - Set balance white (blue channel), larger than 0 (default: auto continuous). (=1 blue intensity 
  unchanged, >1 intensity increased, <1 intensity decreased).
* `-e` (`--exposure`) - Set exposure time in microseconds, range may vary (for example 28 - 1e7).
* `-f` (`--framerate`) - Set framerate (fps) of recording (default: 25).
* `-g` (`--gain`) - Set gain, range may vary (for example 0 - 23.59).
* `-h` (`--help`) - Show help.
* `-i` (`--image`) - Save images instead of video.
* `-o` (`--output`) - Set folder for video/image and log output (default: out). This folder has to contain folders: 
  img, log, vid.
* `-q` (`--quality`) - Set image quality (only for `-i`) between 0 and 100, the higher is the better (default: 95).
* `-v` (`--verbose`) - Print information about the camera state.
