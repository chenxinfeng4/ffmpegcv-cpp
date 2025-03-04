#include <iostream>
#include "../../single_include/ffmpegcv.hpp"

int main(int argc, char* argv[]) {
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
    return 0;
}