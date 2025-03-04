#include <iostream>
#include "../../single_include/ffmpegcv.hpp"

void transcode(FFmpegVideoCapture& cap, FFmpegVideoWriter& writer) {
    while (cap.isOpened()) {
        cap >> writer;  // Stream insertion
    }
    
    cap.release();
    writer.release();
}


int main(int argc, char* argv[]) {
    // 1. crop
    FFmpegVideoCapture cap1("../input.mp4",
                           "bgr24",  // pixel format: "gray", "rgb24", "yuv420p"
                           {100,100,500, 300}  // crop_xywh, start from left top corner
    );
    
    FFmpegVideoWriter writer1("../output_crop.mp4", 
                          "h264",                   // codec
                          cap1.fps,                  // frame rate
                          {cap1.width, cap1.height},  // frame size
                          cap1.pix_fmt               // pixel format
    );
    
    transcode(cap1, writer1);


    // 2. resize
    FFmpegVideoCapture cap2("../input.mp4",
        "bgr24",
        {0,0,0, 0},  // crop_xywh, {0,0,0,0} for no crop
        {500, 300}   // resize_wh, {0,0} for no resize
    );

    FFmpegVideoWriter writer2("../output_resize.mp4", 
        "h264",                   // codec
        cap2.fps,                  // frame rate
        {cap2.width, cap2.height},  // frame size
        cap2.pix_fmt               // pixel format
    );

    transcode(cap2, writer2);

}