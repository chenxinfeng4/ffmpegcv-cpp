# FFmpegCV - OpenCV Alternative for Video I/O
![C++ versions](https://img.shields.io/badge/C++-≥11-blue.svg)
![Last Commit](https://shields.io/github/last-commit/chenxinfeng4/ffmpegcv-cpp)![GitHub issues](https://img.shields.io/github/issues/chenxinfeng4/ffmpegcv-cpp)
![Code size](https://shields.io/github/languages/code-size/chenxinfeng4/ffmpegcv-cpp
)

FFmpegCV provides high-performance video reading and writing capabilities using FFmpeg as its backend. Designed as a drop-in replacement for OpenCV's video I/O functionality, it offers:

- 🚀 **Fast Speed** through FFmpeg integration
- 📦 **Lightweight Architecture** - Zero OpenCV dependencies (26 KB vs OpenCV's 500 MB+)
- 🌐 **Universal Compatibility** - Windows/Linux/macOS with consistent API
- 🎨 **Multi-ColorSpace Support** - Native RGB24, BGR24, YUV420P, and Grayscale
- 💾 **Direct Memory Access** - Frame data stored in contiguous `uint8_t` arrays
- ⚡ **Stream-Optimized API** - Native C++ operators (`>>`, `<<`) for pipeline processing
- **ROI Operations** - Crop, resize the video while reading
- **GPU Support** for CUDA-enabled GPUs (e.g RTX 2060, Tesla V100, ...)


For Python users, check out the [Python version of FFmpegCV](https://github.com/chenxinfeng4/ffmpegcv).

## Installation

1. Install FFmpeg:
    - **Linux** : `sudo apt install ffmpeg`
    - **macOS** (homebrew): `brew install ffmpeg`
    - **Windows** : Download FFmpeg and add it to your system *path*
    - **Conda** : `conda install -c conda-forge ffmpeg=6.0.0` (Avoid using default 4.x.x versions)

    Test the installation by running `ffmpeg -version` in your terminal.

2. Obtain the FFMPEGCV header:
    ```bash
    curl -O https://raw.githubusercontent.com/chenxinfeng4/ffmpegcv-cpp/main/single_include/ffmpegcv.hpp
    ```

3. Include `ffmpegcv.hpp` in your project and compile with C++11 or later:
    ```
    g++ -std=c++11 -o main main.cpp
    ```

## Basic Usage

### Video Read/Write Example
```cpp
#include "ffmpegcv.hpp"

int main() {
    // Initialize video capture
    ffmpegcv::VideoCapture cap("input.mp4");
    auto frame_size = {cap.width, cap.height};
    
    // Initialize video writer (H.264 codec)
    ffmpegcv::VideoWriter writer("output.mp4", "h264", cap.fps, frame_size);
    
    // Frame buffer (BGR format)
    uint8_t* frame = new uint8_t[cap.height * cap.width * 3];
    
    // Processing loop
    while (cap.read(frame)) {
        writer.write(frame);
        // Add your processing here
    }
    
    // Cleanup
    delete[] frame;
    cap.release();
    writer.release();
    return 0;
}
```

## API Comparison with OpenCV

### OpenCV Implementation
```cpp
#include <opencv2/opencv.hpp>

int main() {
    cv::VideoCapture cap("input.mp4");
    cv::Mat frame;
    
    while (cap.read(frame)) {
        // Processing frame
        cv::imshow("Frame", frame);
        cv::waitKey(1);
    }
    
    cap.release();
    return 0;
}
```

### FFmpegCV Equivalent
```cpp
#include "ffmpegcv.hpp"

int main() {
    ffmpegcv::VideoCapture cap("input.mp4");
    uint8_t* frame = new uint8_t[cap.height * cap.width * 3];

    while (cap.read(frame)) {
        // Processing frame
    }
    
    delete[] frame;
    cap.release();
    return 0;
}
```

## Advanced Features

### Stream-like API
```cpp
ffmpegcv::VideoCapture cap("input.mp4");
ffmpegcv::VideoWriter writer("output.mp4", "h264", cap.fps, {cap.width, cap.height});
uint8_t* frame = new uint8_t[cap.height * cap.width * 3];

while (true) {
    cap >> frame;  // Stream extraction
    if (!cap.isOpened()) break;
    
    // Process frame
    writer << frame;  // Stream insertion
}
delete[] frame;
cap.release()
writer.release()
```

### Efficient Transcoding
```cpp
// Direct pipe-through with YUV420P format
ffmpegcv::VideoCapture cap("input.mp4", "yuv420p");
ffmpegcv::VideoWriter writer("output.mp4", "h264", cap.fps, 
                            {cap.width, cap.height}, cap.pix_fmt);

while (cap.isOpened()) {
    cap >> writer;  // Direct stream transfer
    uint8_t* frame = cap.getBuffer();  // Access current frame if needed
}
```

## GPU support (NVIDIA only)
Use GPU to accelerate decoding and encoding, via changing `VideoCapture` and `VideoWriter` to `VideoCaptureNV` and `VideoCaptureNV` respectively.

The decoder-codecs support wide-range, including `h264`, `hevc`, `mpeg4`, `mjpeg` and more.
```c++
ffmpegcv::VideoCapture   cap_cpu(file);
ffmpegcv::VideoCaptureNV cap_gpu(file);              #NVIDIA GPU0
ffmpegcv::VideoCaptureNV cap_gpu1(file, ..., gpu=1)  #NVIDIA GPU1
```

The encoder-codecs support `h264` and `hevc` only.
```cpp
ffmpegcv::VideoCapture   writer_cpu(file, "h264", fps, pix_fmt, size_wh);
ffmpegcv::VideoCaptureNV writer_gpu(...);             #NVIDIA GPU0
ffmpegcv::VideoCaptureNV writer_gpu1(..., gpu=1)      #NVIDIA GPU1
```

## Codecs for Video Writer
Please run `ffmpeg -codecs` in your terminal to get the list of supported codecs.

| Codec   | OpenCV alias   | Description       |
|---------|--------|---------------------------|
| `h264`  | `avc1` | H.264 codec (recommended) |
| `hevc`  | `hev1` | H.265 codec               |
| `mpeg4` | `mp4v` | MPEG-4 codec              | 
| `mjpeg` | `mjpg` | Motion JPEG codec         | 
| ...     | ...    | ...                       |

To use the `h264` codec, you can run one of the following:
```cpp
ffmpegcv::VideoWriter writer(filename, "", fps, pix_fmt); //equally. default codec is h264
ffmpegcv::VideoWriter writer(filename, "h264", fps, pix_fmt);
```

To use the `hevc` codec, you can run
```cpp
ffmpegcv::VideoWriter writer(filename, "hevc", fps, pix_fmt);
```

## Supported Color Spaces
FFmpegCV **only supports common color spaces** for video processing.

| Pixel format | Description | Frame Shape |
|-------------|-------------| -------------|
| `rgb24` | 24-bit RGB (default) |  (h, w, 3) |
| `bgr24` | 24-bit BGR | (h, w, 3) |
| `yuv420p` | YUV420P format (efficient for transcode) | (h*3/2, w) |
| `gray` | gray format | (h, w) |

The `gray` color would save much time than `bgr24/rgb24`, because it extract the `Y` from `yuv420p` without any color space transfermation. To use the `gray` pixel format, you can run
```cpp
ffmpegcv::VideoCapture cap(filename, "gray");
ffmpegcv::VideoWriter writer(filename, codec, fps, "gray");
```

## Supported ROI Operations
Crop xywh rectangle from the video frame, via `crop_xywh`.
```cpp
ffmpegcv::VideoCapture cap(filename,
                            "bgr24",
                            {x, y, w, h}  //crop rectangle

ffmpegcv::VideoCaptureNV cap_gpu(...)   //same for GPU
);  
// The origin point is top-left corner of video. 
// All values should be even (x%2==0, y%2==0, w%2==0, h%2==0).
```

Resize the video frame, via `resize`.
```cpp
ffmpegcv::VideoCapture cap(filename,
                            "bgr24",
                            {0,0,0,0},  // no crop
                            {w, h}

ffmpegcv::VideoCaptureNV cap_gpu(...)   //same for GPU
);
// All values should be even (w%2==0, h%2==0).
```
Use the NVIDIA GPU can accelerate both ROI operations.


## Documentation
| Class                | Methods                          | Properties                  |
|----------------------|----------------------------------|-----------------------------|
| `ffmpegcv::VideoCapture` | `read()`, `isOpened()`, `release()` | `width`, `height`, `fps`, `codec`, `count` |
| `ffmpegcv::VideoWriter`  | `write()`, `release()`           | `width`, `height`, `fps`, `codec`|

## License
MIT License - Free for commercial and personal use

## Contact
For questions or suggestions, feel free to open an issue or contact me directly.