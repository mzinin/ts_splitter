# Ts Splitter
ts_splitter is a simple util to split [MPEG-TS](https://en.wikipedia.org/wiki/MPEG_transport_stream) stream into raw audio or/and video streams.

# Build

Building the project will result in creating two executable files &mdash; `ts_splitter` and `ts_splitter_test` (with `.exe` extension on Windows). The former one is the util itself, the letter one is set of unit tests.

## Prerequisites

    C++ compiler with C++11 support.
    VS2017 or GNU Make.
    Python 2/3 and ffmpeg for auto testing.

## Windows build

Just open solution in Visual Studio and compile it.

## Linux build

Simply run `make` in the root directory of the project. `bin` and `obj` subdirs will be created. Both executables will be saved into `bin` subdir.

# Testing

## Unit tests

Run `ts_splitter_test` and check STDOUT output. No command line options are supported.

## Auto test

`ffmpeg` is required for auto test. Run `autotest.py` with options:

    -u, --util <path to ts_splitter>
Required.

    -f, --files <path to directory containing TS files>
Required.

    --ffmpeg <path to FFmpeg util>
    
Optional. If no `--ffmpeg` option is provided, `autotest.py` checks `ffmpeg` presence in the system. If this check fails, auto test fails.
The idea of the auto test is to split every found TS file and check every produced output with `ffmpeg`.

# Usage

`ts_splitter` supports following comamnd line options:

    -i <input file to split>
Optional. If omitted STDIN is used.

    -oa <output file for 1st audio track>
    
All other audio tracks are saved into files with the save name and suffix. For instance: `-oa audio.out` will produce files `audio.out`, `audio_2.out`, etc. `-oa audio_1.out` will produce files `audio_1.out`, `audio_2.out`, etc. Optional. If omitted but video output file is set, no audio output is written. If both omitted, `audio_1.out` is used by default.

    -ov <output file for 1st video track>

All other video tracks are saved into files with the save name and suffix. For instance: `-ov video.out` will produce files `video.out`, `video_2.out`, etc. `-ov video_1.out` will produce files `video_1.out`, `video_2.out`, etc. Optional. If omitted but audio output file is set, no video output is written. If both omitted, `video_1.out` is used by default.

