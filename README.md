# FFMPEGCV is an alternative to OPENCV for video reading&writing.

The ffmpegcv provide Video Reader and Video Witer with ffmpeg backbone, which are faster and powerful than cv2. Integrating ffmpegcv into your deeplearning pipeline is very smooth.

Here is the C++ version of ffmpegcv. You can also use the python version [ffmpegcv](https://github.com/chenxinfeng4/ffmpegcv).


- The ffmpegcv is api **compatible** to open-cv. 
- The ffmpegcv supports **RGB** & BGR & GRAY format as you like.
- ...On the way

In all, ffmpegcv is just similar to opencv api. But it has more codecs and does't require opencv installed at all. It's great for deeplearning pipeline.


## Install
You need to download ffmpeg before you can use ffmpegcv.
```
 #1A. LINUX: sudo apt install ffmpeg
 #1B. MAC (No NVIDIA GPU): brew install ffmpeg
 #1C. WINDOWS: download ffmpeg and add to the path
 #1D. CONDA: conda install ffmpeg=6.0.0     #don't use the default 4.x.x version
 
 #2
 wget xxx/ffmpegcv_cpp/single_include/ffmpegcv.hpp

```


## Basic example
Read a video by CPU, and rewrite it.
```cpp
// g++ -std=c++11 -o main main.cpp; ./main
#include "ffmpegcv.hpp"

void main() {
    FFmpegVideoCapture cap("input.mp4");
    auto size = {cap.width, cap.height};  // {int, int}
    FFmpegVideoWriter writer("output.mp4", "x264", cap.fps, size);

    uint8_t *frame = new uint8_t[cap.heigth, cap.width, 3]; //BGR format
    while (cap.read(frame)) {
        writer.write(frame);
    }

    cap.release();
    writer.release();
}
```

## Video Reader
---
The ffmpegcv is just similar to opencv in api.

```cpp
// opencv api
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // 1. 打开视频文件
    VideoCapture cap("your_video_file.mp4");

    // 2. 获取视频信息
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH)); // 视频的宽度
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT)); // 视频的高度
    double fps = cap.get(CAP_PROP_FPS); // 视频的帧率
    int totalFrames = static_cast<int>(cap.get(CAP_PROP_FRAME_COUNT)); // 总帧数
    cout << "Frame Width: " << frameWidth << ", Frame Height: " << frameHeight << endl;
    cout << "FPS: " << fps << ", Total Frames: " << totalFrames << endl;

    // 3. 逐帧读取并显示视频
    Mat frame;
    namedWindow("Video", WINDOW_AUTOSIZE); // 创建一个窗口用于显示视频

    while (true) {
        cap >> frame; // 读取一帧

        if (frame.empty()) { // 如果视频结束，退出循环
            break;
        }

        imshow("Video", frame); // 显示当前帧
    }

    // 4. 释放资源并关闭窗口
    cap.release();
    destroyAllWindows();

    return 0;
}


```cpp
// ffmpegcv api without opencv
#include <ffmpegcv.hpp>
using namespace std;

int main() {
    // 1. 打开视频文件
    FFmpegVideoCapture cap("your_video_file.mp4");

    // 2. 获取视频信息
    int frameWidth = cap.width;
    int frameHeight = cap.height; // 视频的高度
    double fps = cap.fps; // 视频的帧率
    int totalFrames = cap.count; // 总帧数
    cout << "Frame Width: " << frameWidth << ", Frame Height: " << frameHeight << endl;
    cout << "FPS: " << fps << ", Total Frames: " << totalFrames << endl;

    // 3. 逐帧读取并显示视频
    uint8_t *frame = new uint8_t[cap.heigth, cap.width, 3]; //frame buffer
    while (cap.read(frame)) {
        ; // do something
    }

    cap.release();
    return 0;
}
```

```cpp
// ffmpegcv api with opencv
#include <ffmpegcv.hpp>
using namespace std;

int main() {
    // 1. 打开视频文件
    FFmpegVideoCapture cap("your_video_file.mp4");

    // 2. 获取视频信息
    int frameWidth = cap.width;
    int frameHeight = cap.height; // 视频的高度
    double fps = cap.fps; // 视频的帧率
    int totalFrames = cap.count; // 总帧数
    cout << "Frame Width: " << frameWidth << ", Frame Height: " << frameHeight << endl;
    cout << "FPS: " << fps << ", Total Frames: " << totalFrames << endl;

    // 3. 逐帧读取并显示视频
    uint8_t *frame = new uint8_t[cap.heigth, cap.width, 3]; //frame buffer
    while (cap.read(frame)) {
        ; // do something
    }

    cap.release();
    return 0;
}
```