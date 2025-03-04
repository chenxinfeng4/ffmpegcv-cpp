
Read the video file frame by frame, rewrite it to the file.
Use the `yuv420p` pixel format.

1. crop
```cpp
FFmpegVideoCapture cap("../input.mp4",
                        "bgr24",  // pixel format: "gray", "rgb24", "yuv420p"
                        {100,100,500, 300}  // crop_xywh, start from left top corner
);

cap.width == 500; cap.height==300;
```

2. resize   
```cpp
FFmpegVideoCapture cap("../input.mp4",
    "bgr24",
    {0,0,0, 0},  // crop_xywh, {0,0,0,0} for no crop
    {500, 300}   // resize_wh, {0,0} for no resize
);

cap.width == 500; cap.height==300;
```

Complie the file to an executable file.

```bash
g++ -std=c++11 -o main main.cpp
```

Run the executable file.
```bash
./main
```