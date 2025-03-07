// g++ -o main main.cpp && ./main
#include <iostream>
#include "../../single_include/ffmpegcv.hpp"


void fun_opt1(ffmpegcv::VideoCapture &cap, ffmpegcv::VideoWriter &writer) {
    while (cap.isOpened()) {
        cap >> writer;  // Stream insertion
    }
}


int main(int argc, char* argv[]) {
    ffmpegcv::VideoCaptureNV cap("../input.mp4", "yuv420p");
    ffmpegcv::VideoWriterNV writer("../output_readbyNV.mp4", 
                                "h264",                   // codec
                                cap.fps,                  // frame rate
                                {cap.width, cap.height},  // frame size
                                cap.pix_fmt               // pixel format
    );
    
    fun_opt1(cap, writer);
    cap.release();
    writer.release();


    ffmpegcv::VideoCaptureNV cap2("../input.mp4", "yuv420p",
                                {0,0,0,0},                // crop_xywh, {0,0,0,0} for skip
                                {400, 300});              // resize_wh, {0,0} for skip
    ffmpegcv::VideoWriterNV writer2("../output_readbyNV_resized.mp4", 
                                "h264",                   // codec
                                cap2.fps,                  // frame rate
                                {cap2.width, cap2.height},  // frame size
                                cap2.pix_fmt               // pixel format
    );
    
    fun_opt1(cap2, writer2);
    cap2.release();
    writer2.release();
    return 0;
}