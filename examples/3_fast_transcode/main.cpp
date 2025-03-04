#include <iostream>
#include "../../single_include/ffmpegcv.hpp"


int main(int argc, char* argv[]) {
    FFmpegVideoCapture cap("../input.mp4", "yuv420p");
    FFmpegVideoWriter writer("../output.mp4", 
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
}