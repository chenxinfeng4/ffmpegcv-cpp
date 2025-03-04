# FFmpegCV - OpenCV Alternative for Video I/O
![Last Commit](https://shields.io/github/last-commit/chenxinfeng4/ffmpegcv-cpp)![GitHub issues](https://img.shields.io/github/issues/chenxinfeng4/ffmpegcv-cpp)


FFmpegCV provides high-performance video reading and writing capabilities using FFmpeg as its backend. Designed as a drop-in replacement for OpenCV's video I/O functionality, it offers:

- 🚀 **Fast Speed** through FFmpeg integration
- 📦 **Lightweight Architecture** - Zero OpenCV dependencies (1KB vs OpenCV's 500MB+)
- 🌐 **Universal Compatibility** - Windows/Linux/macOS with consistent API
- 🎨 **Multi-ColorSpace Support** - Native RGB24, BGR24, YUV420P, and Grayscale
- 💾 **Direct Memory Access** - Frame data stored in contiguous `uint8_t` arrays
- ⚡ **Stream-Optimized API** - Native C++ operators (`>>`, `<<`) for pipeline processing


For Python users, check out the [Python version of FFmpegCV](https://github.com/chenxinfeng4/ffmpegcv).

## Installation

1. Install FFmpeg:
    - **Linux** : `sudo apt install ffmpeg`
    - **macOS** (homebrew): `brew install ffmpeg`
    - **Windows** : Download FFmpeg and add it to your system *path*
    - **Conda** : `conda install -c conda-forge ffmpeg=6.0.0` (Avoid using default 4.x.x versions)

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
    FFmpegVideoCapture cap("input.mp4");
    auto frame_size = {cap.width, cap.height};
    
    // Initialize video writer (H.264 codec)
    FFmpegVideoWriter writer("output.mp4", "h264", cap.fps, frame_size);
    
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

}
```

## Advanced Features

### Stream-like API
```cpp
FFmpegVideoCapture cap("input.mp4");
FFmpegVideoWriter writer("output.mp4", "h264", cap.fps, {cap.width, cap.height});
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
FFmpegVideoCapture cap("input.mp4", "yuv420p");
FFmpegVideoWriter writer("output.mp4", "h264", cap.fps, 
                        {cap.width, cap.height}, cap.pix_fmt);

while (cap.isOpened()) {
    cap >> writer;  // Direct stream transfer
    uint8_t* frame = cap.getBuffer();  // Access current frame if needed
}
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

To use the `hevc` codec, you can run
```cpp
FFmpegVideoWriter writer(filename, "hevc", fps, pix_fmt);
```

## Supported Color Spaces
FFmpegCV **only supports common color spaces** for video processing.

| Pixel format | Description | Frame Shape |
|-------------|-------------| -------------|
| `rgb24` | 24-bit RGB (default) |  (h, w, 3) |
| `bgr24` | 24-bit BGR | (h, w, 3) |
| `yuv420p` | YUV420P format (efficient for transcode) | (h*3/2, w) |
| `gray` | gray format | (h, w) |

To use the `gray` codec, you can run
```cpp
FFmpegVideoCapture cap(filename, "gray");
FFmpegVideoWriter writer(filename, codec, fps, "gray");
```

## Supported ROI Operations
Crop xywh rectangle from the video frame, via `crop_xywh`.
```cpp
FFmpegVideoCapture cap(filename,
                       "bgr24",
                       {x, y, w, h}  //crop rectangle
);  
// The origin point is top-left corner of video. 
// All values should be even (x%2==0, y%2==0, w%2==0, h%2==0).
```

Resize the video frame, via `resize`.
```cpp
FFmpegVideoCapture cap(filename,
                       "bgr24",
                       {0,0,0,0},  // no crop
                       {w, h}
);
// All values should be even (w%2==0, h%2==0).
```


## Documentation
| Class                | Methods                          | Properties                  |
|----------------------|----------------------------------|-----------------------------|
| `FFmpegVideoCapture` | `read()`, `isOpened()`, `release()` | `width`, `height`, `fps`, `codec`, `count` |
| `FFmpegVideoWriter`  | `write()`, `release()`           | `width`, `height`, `fps`, `codec`|

## License
MIT License - Free for commercial and personal use

## Contact
For questions or suggestions, feel free to open an issue or contact me directly.