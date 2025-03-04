
Read the video file frame by frame, and display the information.
```cpp
FFmpegVideoCapture cap("../input.mp4");
std::cout << "Video resolution: " << cap.width << "x" << cap.height << std::endl
            << "Video fps: " << cap.fps << std::endl
            << "Video frame count: " << cap.count << std::endl
            << "Video duration: " << cap.duration << std::endl
            << "Video codec: " << cap.codec << std::endl;

uint8_t* frame = new uint8_t[cap.height * cap.width * 3];

while (cap.read(frame)) {
    // Processing frame
}

delete[] frame;
cap.release();
```

Complie the file to an executable file.

```bash
g++ -std=c++11 -o main main.cpp
```

Run the executable file.
```bash
./main
```

Output:
```
ffmpeg_cmd: ffmpeg -y -loglevel warning -i "../input.mp4" -f rawvideo  -pix_fmt bgr24 pipe:
Video resolution: 640x480
Video fps: 30
Video frame count: 600
Video duration: 20
Video codec: h264
```
