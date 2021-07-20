# Image Grabber

Image Grabber allows you to capture images or video with multiple Basler USB Cameras at once. It logs all captured data
to a CSV file for later use.

## Prerequisites

Tested on Ubuntu 20.04, may not work properly on other Linux distributions. This software is necessary for the code to
compile and run properly.

* Compiler supporting C++17.
* CMake 3.16 and higher.
* [OpenCV library](https://opencv.org/).
* [Pylon SDK](https://www.baslerweb.com/en/sales-support/downloads/software-downloads/installationnotes-pylon-6-2-0-linux-x86-arm/)
  (tested on version 6.2.0).

Devices supported are:

* [Basler ace USB 3.0](https://docs.baslerweb.com/basler-ace-usb-30) cameras
  (tested on acA2000-165uc and acA1920-150uc).

## How to use the app

In the project root folder:

1. Build and compile: `make`.
2. Run with arguments (see below), for example: `./ImageGrabber -v -f 60 -o "/myoutput"`.
3. Exit the app with `SIGINT` or `SIGTERM` signal (`Ctrl+C`).

## Arguments

* `-R` (`--bwr`) - Balance white (red channel), larger than 0, double precision, (default: auto continuous),
  (=1 red intensity unchanged, >1 intensity increased, <1 intensity decreased).
* `-G` (`--bwg`) - Balance white (green channel), larger than 0, double precision, (default: auto continuous),
  (=1 green intensity unchanged, >1 intensity increased, <1 intensity decreased).
* `-B` (`--bwb`) - Balance white (blue channel), larger than 0, double precision, (default: auto continuous),
  (=1 blue intensity unchanged, >1 intensity increased, <1 intensity decreased).
* `-e` (`--exposure`) - Set exposure time in microseconds, range may vary (for example 28 - 1e7), double precision,
  (default: auto continuous).
* `-f` (`--framerate`) - Set framerate (fps) of recording, double precision, (default: 25).
* `-g` (`--gain`) - Set gain, range may vary (for example 0 - 23.59), double precision, (default: auto continuous).
* `-h` (`--help`) - Show help.
* `-i` (`--image`) - Save images instead of video.
* `-o` (`--output`) - Set directory for video/image and log output (default: out), directories will be created.
* `-q` (`--quality`) - Set image quality (use only together with `-i`) between 0 and 100, the higher is the better,
  integer precision, (default: 95).
* `-v` (`--verbose`) - Print information about the camera state.

## Output format

Saved images are in JPG format.

Saved video is in AVI container.

Saved log information is in CSV file with format shown below. Each captured image on a separate line.

* `index` - Index of the captured image.
* `mode` - Format of recording: `img` or `vid`, stays same for one log file.
* `camera` - Camera identification number, stays same for one log file.
* `file_path` - Path to the captured image or video.
* `timestamp_in_ms` - System timestamp of the captured image in milliseconds.
* `iso_datetime` - Datetime of the captured image in ISO format `YYYY-MM-DDThh:mm:ss.sss`.
* `exposure_time` - Exposure time of the captured image.
* `gain` - Gain of the captured image.
* `white_balance_r` - Balance white (red channel) of the captured image.
* `white_balance_g` - Balance white (green channel) of the captured image.
* `white_balance_b` - Balance white (blue channel) of the captured image.

## Cameras configuration

Cameras can be configured using the arguments when launching the app. Settings are applied to all cameras. Default
values for all available settings are `auto continuous` which means that before each image is captured, values are
automatically set. Available settings are stated below.

* Exposure time
    * Available range depends on the camera used,
      see [documentation](https://docs.baslerweb.com/exposure-time#specifics) to find these values.
    * Maximal exposure time is limited to values that do not interfere with the requested frame rate.
* Gain
    * Available range depends on the camera used, see [documentation](https://docs.baslerweb.com/gain#specifics) to find
      these values.
* White Balance
    * Configure each channel independently - red, green, blue.

## Cameras handling

App detects all Basler cameras connected to the device and starts recording on all of them with the same settings. Once
any camera is disconnected, recording is stopped on all of the cameras. As soon as the disconnected camera is connected
again, the recording is resumed and the newly obtained data is appended.

## Image handling

Each captured image is handled by the OpenCV library and it is saved using it. Videos are also saved with OpenCV.

## Synchronization of capturing

Capturing images with multiple cameras is parallelized using Pylon library - each camera has its own thread.
Synchronization of capturing images from all cameras at the same time is done by software trigger that is serially sent
to all cameras. Software sleep until the necessary time is used when waiting for the next image. With this mechanism, 
precision of +-1 ms is achieved.
