
Read the video file by **NV** IDIA GPU, and rewrite it to a new video file by GPU.

Change the `VideoCapture` to `VideoCaptureNV`, and `VideoWriter` to `VideoWriterNV`. The arguments of GPU version are the same as the CPU version.

The `VideoCaptureNV` supports wide-range codecs ('h264', 'hevc', 'mpeg4', 'mjpeg', 'vp8', ..., see `ffmpeg -decodecs | grep cuvid`).

The `VideoWriterNV` supports only 'h264' and 'hevc', see `ffmpeg -encodecs | grep nvenc`.

```cpp
// VideoCapture ==> VideoCaptureNV, VideoWriter ==> VideoWriterNV
ffmpegcv::VideoCaptureNV cap("../input.mp4", "yuv420p");
ffmpegcv::VideoWriterNV writer("../output_readbyNV.mp4", 
                            "h264",                   // codec
                            cap.fps,                  // frame rate
                            {cap.width, cap.height},  // frame size
                            cap.pix_fmt               // pixel format
);

while (cap.isOpened()) {
    cap >> writer;
}
cap.release();
writer.release();
```


Complie the file to an executable file.

```bash
g++ -std=c++11 -o main main.cpp
```

Run the executable file.
```bash
./main
```