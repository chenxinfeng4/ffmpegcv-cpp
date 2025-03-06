
Read the video file frame by frame, rewrite it to the file.
Use the `yuv420p` pixel format.
```cpp
ffmpegcv::VideoCapture cap("../input.mp4", "yuv420p");
ffmpegcv::VideoWriter writer("../output.mp4", 
                        "h264",                   // codec
                        cap.fps,                  // frame rate
                        {cap.width, cap.height},  // frame size
                        cap.pix_fmt               // pixel format
);

while (cap.isOpened()) {
    cap >> writer;  // Stream insertion
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