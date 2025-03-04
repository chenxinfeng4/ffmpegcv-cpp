#include <iostream>
#include "../../single_include/ffmpegcv.hpp"


void fun_opt1(FFmpegVideoCapture &cap, FFmpegVideoWriter &writer) {
    uint8_t* frame = new uint8_t[cap.height * cap.width * 3];
    while (true) {
        if(cap.read(frame)) {  // Stream extraction
            writer.write(frame);  // Stream insertion
        }
        else break;
    }
    delete[] frame;
}

void fun_opt2(FFmpegVideoCapture &cap, FFmpegVideoWriter &writer) {
    uint8_t* frame = new uint8_t[cap.height * cap.width * 3];
    while (true) {
        cap >> frame;  // Stream extraction
        if (!cap.isOpened()) break;
        
        // Process frame
        writer << frame;  // Stream insertion
    }
    delete[] frame;
}


void fun_opt3(FFmpegVideoCapture &cap, FFmpegVideoWriter &writer) {
    uint8_t* frame = new uint8_t[cap.height * cap.width * 3];
    while (cap.isOpened()) {
        cap >> frame >> writer;  // Stream extraction and insertion
    }
    delete[] frame;
}


void fun_opt4(FFmpegVideoCapture &cap, FFmpegVideoWriter &writer) {
    while (cap.isOpened()) {
        cap >> writer;  // Stream insertion
        uint8_t* frame = cap.getBuffer(); // Access current frame if needed
    }
}


int main(int argc, char* argv[]) {
    FFmpegVideoCapture cap("../input.mp4");
    FFmpegVideoWriter writer("../output.mp4", 
                          "h264",                   // codec
                          cap.fps,                  // frame rate
                          {cap.width, cap.height}  // frame size
    );
    
    fun_opt1(cap, writer);
    
    cap.release();
    writer.release();
}