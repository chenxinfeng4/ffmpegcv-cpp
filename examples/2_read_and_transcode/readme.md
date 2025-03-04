
Read the video file frame by frame, rewrite it to the file.
```cpp
FFmpegVideoCapture cap("../input.mp4");
FFmpegVideoWriter out("../output.mp4", 
                        "x264",                   // codec
                        cap.fps,                  // frame rate
                        {cap.width, cap.height},  // frame size
);

uint8_t* frame = new uint8_t[cap.height * cap.width * 3];
while (true) {
    if(cap.read(frame)) {  // Stream extraction
        writer.write(frame);  // Stream insertion
    }
    else break;
}
delete[] frame;

cap.release()
writer.release()
```

Complie the file to an executable file.

```bash
g++ -std=c++11 -o main main.cpp
```

Run the executable file.
```bash
./main
```