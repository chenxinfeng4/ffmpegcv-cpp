#pragma once
#define FFMPEGCV_H

#include <fstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <regex>
#include <cstdlib>
#include <memory>

#include <cstdint>
#include <utility>

namespace ffmpegcv {


//================ End interface function ===============

//================ Begin Video info ==================
FILE* POPEN_R(const char* command) {
#ifdef _WIN32
    return _popen(command, "rb");  // Windows used _popen + binary mode
#else
    return popen(command, "r");   // Linux/Mac type
#endif
}

FILE* POPEN_W(const char* command) {
#ifdef _WIN32
    return _popen(command, "wb");
#else
    return popen(command, "w");
#endif
}

int PCLOSE(FILE* fp) {
#ifdef _WIN32
    return _pclose(fp);
#else
    return pclose(fp);
#endif
}

std::string get_file_extension(const std::string& filename);
std::string execute_command(const std::string& command);

struct VideoInfo {
    std::string codec;
    float duration = 0.0f;
    float fps = 0.0f;
    int width = 0;
    int height = 0;
    int count = 0;
    bool is_complex = false;
};

VideoInfo get_info(const std::string& filename);

//================ End Video info ==================

//================ Begin Video Writer ==================

struct Size_wh {
    int width = 0;
    int height = 0;

    Size_wh(int w, int h) : width(w), height(h) {}
    Size_wh() {}
    Size_wh(std::initializer_list<int> wh) : width(*wh.begin()), height(*(wh.begin() + 1)) {}
    Size_wh(const VideoInfo& vi) : width(vi.width), height(vi.height) {}

#ifdef OPENCV_CORE_TYPES_HPP
    Size_wh(const cv::Size& sz) : width(sz.width), height(sz.height) {}
#endif

    bool empty() const { return width == 0 || height == 0; }
};

std::vector<int> get_outnumpyshape(Size_wh size_wh, std::string pix_fmt);

class VideoWriter {
public:
    VideoWriter();
    VideoWriter(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, int isColor = true,
        std::string ffmpeg_output_opt = "");
    VideoWriter(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, std::string pix_fmt,
        std::string ffmpeg_output_opt = "");
    ~VideoWriter();
    virtual void initializer();
    void release();
    void close();
    bool write(const void* frame);
#ifdef OPENCV_CORE_TYPES_HPP
    bool write(cv::Mat& frame);
#endif
    bool isOpened() const;

public:
    std::string filename = "";
    std::string codec = "h264";
    double fps = 0;
    Size_wh size_wh = Size_wh(0,0);
    int width = 0;
    int height = 0;
    std::string pix_fmt = "bgr24";
    std::string output_pix_fmt = "yuv420p";
    std::string ffmpeg_output_opt = "";
    bool waitInit = true;
    FILE* process;
    std::vector<int> innumpyshape;
    int bytes_per_frame;
    std::string ffmpeg_cmd = "";

};

//================End Video Writer==================

//================Begin Video Reader==================
std::tuple<Size_wh, Size_wh, std::string> get_videofilter_cpu(
    Size_wh originsize, std::string pix_fmt, std::tuple<int, int, int, int> crop_xywh, Size_wh resize);

class VideoCapture {
public:
    VideoCapture();
    VideoCapture(const std::string& filename, int isColor = true,
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0}, Size_wh resize = Size_wh(0,0));

    VideoCapture(const std::string& filename, std::string pix_fmt,
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0}, Size_wh resize = Size_wh(0,0));

    ~VideoCapture();
    virtual void initializer();
    virtual void release();
    void close();
    uint8_t* getBuffer();
    virtual bool read(void * frame);
    virtual std::tuple<bool, void *> read();
#ifdef OPENCV_CORE_TYPES_HPP
    virtual bool read(cv::Mat& frame);
#endif
    bool isOpened();
    const int size();
    const int len();

public:
    std::string filename = "";
    std::string pix_fmt = "bgr24";
    std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0};
    Size_wh resize = Size_wh(0, 0);
    FILE* process = NULL;
    int bytes_per_frame = 0;
    int width = 0;
    int height = 0;
    int origin_width = 0;
    int origin_height = 0;
    int count = 0;
    int iframe = -1;
    float fps = 0;
    float duration = 0;
    bool waitInit = true;
    void* default_buffer = NULL;
    std::string codec = "";
    Size_wh size_wh = Size_wh(0, 0);
    std::vector<int> outnumpyshape;
    std::string ffmpeg_cmd = "";
};


class VideoReader: public VideoCapture {
};
//================End Video Reader==================



class VideoCaptureStreamRT: public VideoCapture {
public:
    VideoCaptureStreamRT();
    VideoCaptureStreamRT(const std::string& filename, int isColor = true,
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0}, Size_wh resize = Size_wh(0,0));

    VideoCaptureStreamRT(const std::string& filename, std::string pix_fmt,
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0}, Size_wh resize = Size_wh(0,0));

    void initializer() override;
};

std::tuple<Size_wh, Size_wh, std::string, std::string> get_videofilter_gpu(
    Size_wh originsize, std::string pix_fmt, std::tuple<int, int, int, int> crop_xywh, Size_wh resize);
int get_num_NVIDIA_GPUs();

class VideoCaptureNV: public VideoCapture {
public:
    VideoCaptureNV();
    VideoCaptureNV(const std::string& filename, int isColor = true,
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0},
        Size_wh resize = Size_wh(0,0), int gpu = 0);

    VideoCaptureNV(const std::string& filename, std::string pix_fmt,
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0},
        Size_wh resize = Size_wh(0,0), int gpu = 0);

    void initializer() override;

private:
    int gpu = 0;
};

class VideoWriterNV: public VideoWriter {
public:
    VideoWriterNV();
    VideoWriterNV(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, int isColor = true,
        std::string ffmpeg_output_opt = "", int gpu = 0);
    VideoWriterNV(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, std::string pix_fmt,
        std::string ffmpeg_output_opt = "", int gpu = 0);

    void initializer() override;

private:
    int gpu = 0;
};

} //END NAMESPACE FFMPEGCV


void* operator>>(ffmpegcv::VideoCapture& cap, void* frame);
void operator>>(void* frame, ffmpegcv::VideoWriter& writer);
void operator<<(ffmpegcv::VideoWriter& writer, void* frame);
void operator>>(ffmpegcv::VideoCapture& cap, ffmpegcv::VideoWriter& writer);


// ================== cpp file ====================
namespace ffmpegcv{


bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}
std::string decoder_to_nvidia(const std::string& codec);
std::string encoder_to_nvidia(const std::string& codec);

std::string get_file_extension(const std::string& filename) {
    const size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = filename.substr(pos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(),
                        [](unsigned char c) { return std::tolower(c); });
        return ext;
    }
    return "";
}

std::string execute_command(const std::string& command) {
    std::string result;
    FILE* stream = POPEN_R(command.c_str());
    if (!stream) return result;

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != nullptr) {
        result += buffer;
    }
    PCLOSE(stream);
    return result;
}

bool file_exsits(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

VideoInfo get_info(const std::string& filename) {
    assert (file_exsits(filename) && "File does not exist");
    static const std::vector<std::string> complex_formats = {"mkv", "flv", "ts"};
    const bool is_complex = std::find(complex_formats.begin(), complex_formats.end(),
        get_file_extension(filename)) != complex_formats.end();

    std::ostringstream cmd;
    cmd << "ffprobe -v quiet -print_format json=compact=1 -select_streams v:0";
    if (is_complex) cmd << " -count_packets";
    cmd << " -show_streams \"" << filename << "\"";

    const std::string json_output = execute_command(cmd.str());
    VideoInfo info;
    info.is_complex = is_complex;

    static const std::regex codec_re( "codec_name.: .(\\w+)");
    static const std::regex width_re( "width.: (\\d+)");
    static const std::regex height_re( "height.: (\\d+)");
    static const std::regex frames_re( "nb_frames.: .(\\d+)");
    static const std::regex nb_read_packets_re( "nb_read_packets.: .(\\d+)");
    static const std::regex duration_re( "duration.: .([0-9]*\\.?[0-9]+)");
    static const std::regex rate_re( "r_frame_rate.: .(\\d+)/(\\d+)");

    std::smatch match;
    auto count_re = is_complex? nb_read_packets_re : frames_re;

    if (std::regex_search(json_output, match, codec_re) && match.size() > 1) {
        info.codec = match[1];
    }

    if (std::regex_search(json_output, match, width_re) && match.size() > 1) {
        info.width = std::stoi(match[1]);
    }

    if (std::regex_search(json_output, match, height_re) && match.size() > 1) {
        info.height = std::stoi(match[1]);
    }

    if (std::regex_search(json_output, match, count_re) && match.size() > 1) {
        info.count = std::stoi(match[1]);
    }

    if (std::regex_search(json_output, match, duration_re) && match.size() > 1) {
        info.duration = strtof(match[1].str().c_str(), nullptr);
    }

    if (std::regex_search(json_output, match, rate_re) && match.size() > 2) {
        float num = std::stof(match[1]);
        float den = std::stof(match[2]);
        if (den != 0) info.fps = float(num) / den;
    }

    return info;
}

VideoInfo get_info_stream(const std::string& filename, int timeout=0, int duration_ms=100){
    std::string rtsp_opt = startsWith(filename, "rtsp://") ? "-rtsp_flags prefer_tcp -pkt_size 736 " : "";
    std::string analyze_duration = " -analyzeduration " + std::to_string(duration_ms) + "000 ";
    std::ostringstream cmd;
    cmd << "ffprobe -v quiet -print_format json=compact=1 "
        << rtsp_opt << analyze_duration
        << " -select_streams v:0 -show_format -show_streams \""
        << filename << "\"";

    const std::string json_output = execute_command(cmd.str());
    VideoInfo info;

    static const std::regex codec_re( "codec_name.: .(\\w+)");
    static const std::regex width_re( "width.: (\\d+)");
    static const std::regex height_re( "height.: (\\d+)");
    static const std::regex rate_re( "r_frame_rate.: .(\\d+)/(\\d+)");

    std::smatch match;

    if (std::regex_search(json_output, match, codec_re) && match.size() > 1) {
        info.codec = match[1];
    }

    if (std::regex_search(json_output, match, width_re) && match.size() > 1) {
        info.width = std::stoi(match[1]);
    }

    if (std::regex_search(json_output, match, height_re) && match.size() > 1) {
        info.height = std::stoi(match[1]);
    }

    if (std::regex_search(json_output, match, rate_re) && match.size() > 2) {
        float num = std::stof(match[1]);
        float den = std::stof(match[2]);
        if (den != 0) info.fps = float(num) / den;
    }

    return info;
}
//================ End Video info ==================

//================ Begin Video Writer ==================

std::vector<int> get_outnumpyshape(Size_wh size_wh, std::string pix_fmt) {
    if (pix_fmt == "bgr24" || pix_fmt == "rgb24") {
        return {size_wh.height, size_wh.width, 3};
    } else if (pix_fmt == "gray") {
        return {size_wh.height, size_wh.width};
    } else if (pix_fmt == "yuv420p" || pix_fmt == "yuvj420p" || pix_fmt == "nv12") {
        return {size_wh.height * 3 / 2, size_wh.width};
    } else {
        assert(false && "pix_fmt not supported");
        return {0, 0};
    }
}

VideoWriter::VideoWriter(){;}

VideoWriter::VideoWriter(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, int isColor,
    std::string ffmpeg_output_opt):
    filename(filename), codec(codec), fps(fps), size_wh(size_wh), pix_fmt(isColor ? "bgr24" : "gray"), ffmpeg_output_opt(ffmpeg_output_opt)
    {
    initializer();
}


VideoWriter::VideoWriter(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, std::string pix_fmt,
    std::string ffmpeg_output_opt):
    filename(filename), codec(codec), fps(fps), size_wh(size_wh), pix_fmt(pix_fmt), ffmpeg_output_opt(ffmpeg_output_opt)
    {
    initializer();
}

void VideoWriter::initializer(){
    codec = codec.empty() ? "h264" : codec;
    width = size_wh.width;
    height = size_wh.height;
    process = 0;
    std::string rtsp_str = startsWith(filename, "rtsp://") ? " -f rtsp -rtsp_transport tcp " : " ";

    std::ostringstream oss;
    oss << "ffmpeg -y -loglevel warning -f rawvideo -pix_fmt " << pix_fmt
        << " -s " << width << "x" << height << " -r " << fps
        << " -i pipe: -c:v " << codec << " -pix_fmt " << output_pix_fmt
        << ffmpeg_output_opt << rtsp_str << " \"" << filename << "\"";
    ffmpeg_cmd = oss.str();
    std::cout << "ffmpeg_cmd: " << ffmpeg_cmd << std::endl;

    innumpyshape = get_outnumpyshape(size_wh, pix_fmt);
    bytes_per_frame = 1;
    for (int num : innumpyshape) {
        bytes_per_frame *= num;
    }
}


VideoWriter::~VideoWriter() {
    release();
}

void VideoWriter::release() {
    if (process) {
        PCLOSE(process);
        process = NULL;
    }
}

void VideoWriter::close() {
    release();
}

bool VideoWriter::write(const void* frame) {
    if (waitInit){
        process = POPEN_W(ffmpeg_cmd.c_str());
        waitInit = false;
    }
    if (frame == NULL) return false;
    if (!process) {
        std::cerr << "Failed to open video writer";
        return false;
    }
    fwrite(frame, sizeof(char), bytes_per_frame, process);
    return true;
}

#ifdef OPENCV_CORE_TYPES_HPP
bool VideoWriter::write(cv::Mat& frame) {
    return write(frame.data);
}
#endif

bool VideoWriter::isOpened() const {
    return process || waitInit;
}

VideoWriterNV::VideoWriterNV(): VideoWriter(){;}

VideoWriterNV::VideoWriterNV(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, int isColor,
        std::string ffmpeg_output_opt, int gpu){
    this->filename = filename;
    this->codec = codec;
    this->fps = fps;
    this->size_wh = size_wh;
    this->pix_fmt = isColor ? "bgr24" : "gray";
    this->ffmpeg_output_opt = ffmpeg_output_opt;
    this->gpu = gpu;
    initializer();
}

VideoWriterNV::VideoWriterNV(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, std::string pix_fmt,
        std::string ffmpeg_output_opt, int gpu){
    this->filename = filename;
    this->codec = codec;
    this->fps = fps;
    this->size_wh = size_wh;
    this->pix_fmt = pix_fmt;
    this->ffmpeg_output_opt = ffmpeg_output_opt;
    this->gpu = gpu;
    initializer();
}


void VideoWriterNV::initializer(){
    int numGPU = get_num_NVIDIA_GPUs();
    assert (numGPU > 0 && "No NVIDIA GPU found");
    gpu = gpu % numGPU;

    codec = codec.empty() ? "h264" : codec;
    std::string codec_gpu = encoder_to_nvidia(codec);
    width = size_wh.width;
    height = size_wh.height;
    process = 0;
    std::string rtsp_str = startsWith(filename, "rtsp://") ? " -f rtsp -rtsp_transport tcp " : " ";

    std::ostringstream oss;
    oss << "ffmpeg -y -loglevel warning -f rawvideo -pix_fmt " << pix_fmt
        << " -s " << width << "x" << height << " -r " << fps
        << " -i pipe: -c:v " << codec_gpu << " -gpu " << gpu
        << " -pix_fmt " << output_pix_fmt
        << ffmpeg_output_opt << rtsp_str << " \"" << filename << "\"";
    ffmpeg_cmd = oss.str();
    std::cout << "ffmpeg_cmd: " << ffmpeg_cmd << std::endl;

    innumpyshape = get_outnumpyshape(size_wh, pix_fmt);
    bytes_per_frame = 1;
    for (int num : innumpyshape) {
        bytes_per_frame *= num;
    }
}
//================End Video Writer==================

//================Begin Video Reader==================
std::tuple<Size_wh, Size_wh, std::string> get_videofilter_cpu(
    Size_wh originsize, std::string pix_fmt, std::tuple<int, int, int, int> crop_xywh, Size_wh resize) {
    static const std::vector<std::string> allowed_pix_fmts = {"rgb24", "bgr24", "yuv420p", "yuvj420p", "nv12", "gray"};
    assert(std::find(allowed_pix_fmts.begin(), allowed_pix_fmts.end(), pix_fmt) != allowed_pix_fmts.end());
    int origin_width = originsize.width;
    int origin_height = originsize.height;
    int crop_x = std::get<0>(crop_xywh);
    int crop_y = std::get<1>(crop_xywh);
    int crop_w = std::get<2>(crop_xywh);
    int crop_h = std::get<3>(crop_xywh);
    int resize_width = resize.width;
    int resize_height = resize.height;

    std::string cropopt;
    if (crop_w != 0 && crop_h != 0) {
        assert(crop_x % 2 == 0 && crop_y % 2 == 0 && crop_w % 2 == 0 && crop_h % 2 == 0);
        assert(crop_w <= origin_width && crop_h <= origin_height);
        cropopt = "crop=" + std::to_string(crop_w) + ":" + std::to_string(crop_h) +
                    ":" + std::to_string(crop_x) + ":" + std::to_string(crop_y);
    } else {
        crop_w = origin_width;
        crop_h = origin_height;
        cropopt = "";
    }
    Size_wh cropsize = {crop_w, crop_h};
    Size_wh final_size_wh = cropsize;

    std::string scaleopt="";
    std::string padopt="";
    if (!resize.empty() && (resize_width != 0 || resize_height != 0)) {
        assert (resize_width % 2 == 0 && resize_height % 2 == 0);
        final_size_wh = resize;
        scaleopt = "scale=" + std::to_string(resize_width) + "x" + std::to_string(resize_height);
    }

    std::string pix_fmt_opt = (pix_fmt == "gray") ? "extractplanes=y" : "";
    std::string filterstr = "";
    if (!cropopt.empty() || !scaleopt.empty() || !pix_fmt_opt.empty()) {
        filterstr = "-vf ";
        if (!cropopt.empty()) filterstr += cropopt + ",";
        if (!scaleopt.empty()) filterstr += scaleopt + ",";
        if (!pix_fmt_opt.empty()) filterstr += pix_fmt_opt + ",";
        filterstr = filterstr.substr(0, filterstr.size() - 1);
    }
    return std::make_tuple(cropsize, final_size_wh, filterstr);
}

VideoCapture::VideoCapture(){;}

VideoCapture::VideoCapture(const std::string& filename, int isColor,
    std::tuple<int, int, int, int> crop_xywh, Size_wh resize):
    filename(filename), pix_fmt(isColor ? "bgr24" : "gray"), crop_xywh(crop_xywh), resize(resize){
    initializer();
}

VideoCapture::VideoCapture(const std::string& filename, std::string pix_fmt,
    std::tuple<int, int, int, int> crop_xywh, Size_wh resize):
    filename(filename), pix_fmt(pix_fmt), crop_xywh(crop_xywh), resize(resize){
    initializer();
}

void VideoCapture::initializer(){
    VideoInfo videoinfo = get_info(filename);
    origin_width = width = videoinfo.width;
    origin_height = height = videoinfo.height;
    codec = videoinfo.codec;
    fps = videoinfo.fps;
    duration = videoinfo.duration;
    count = videoinfo.count;
    iframe = -1;
    default_buffer = NULL;
    waitInit = true;

    assert(width % 2 == 0 && "Height must be even");
    assert(height % 2 == 0 && "Width must be even");

    std::tuple<Size_wh, Size_wh, std::string> filter_options = get_videofilter_cpu(
        {width, height}, pix_fmt, crop_xywh, resize);
    size_wh = std::get<1>(filter_options);
    std::string filterstr = std::get<2>(filter_options);
    width = size_wh.width;
    height = size_wh.height;

    // 初始化 ffmpeg 的 VideoCapture
    std::ostringstream oss;
    oss << "ffmpeg -y -loglevel warning -i \"" << filename << "\" -f rawvideo "
        << filterstr << " -pix_fmt " << pix_fmt << " pipe:";

    ffmpeg_cmd = oss.str();
    std::cout << "ffmpeg_cmd: " << ffmpeg_cmd << std::endl;

    // 计算每帧的位数
    outnumpyshape = get_outnumpyshape(size_wh, pix_fmt);
    bytes_per_frame = 1;
    for (int num : outnumpyshape) {
        bytes_per_frame *= num;
    }
}


VideoCapture::~VideoCapture() {
    release();
}

void VideoCapture::release() {
    if (default_buffer) {
        free(default_buffer);
        default_buffer = NULL;
    }
    if (process) {
        PCLOSE(process);
        process = NULL;
    }
}

void VideoCapture::close() {
    release();
}

uint8_t* VideoCapture::getBuffer() {
    if (default_buffer == NULL){
        default_buffer = (void*)malloc(bytes_per_frame);
    }
    return static_cast<uint8_t*>(default_buffer);
}

bool VideoCapture::read(void * frame) {
    if (waitInit){
        process = POPEN_R(ffmpeg_cmd.c_str());
        waitInit = false;
    }

    if (process) {
        int bytesRead = fread(frame, sizeof(char), bytes_per_frame, process);
        if (bytesRead == bytes_per_frame) {
            iframe += 1;
            return true;
        } else {
            release();
            return false;
        }
    } else {
        return false;
    }
}

std::tuple<bool, void *> VideoCapture::read() {
    uint8_t* buffer = getBuffer();
    bool success = read(buffer);
    if (!success) {buffer = NULL;}
    return std::make_tuple(success, buffer);
}

bool VideoCapture::isOpened() {
    return process != NULL || waitInit;
}

const int VideoCapture::size() {
    return count;
}

const int VideoCapture::len() {
    return count;
}

VideoCaptureStreamRT::VideoCaptureStreamRT():VideoCapture(){;}

VideoCaptureStreamRT::VideoCaptureStreamRT(const std::string& filename, int isColor,
    std::tuple<int, int, int, int> crop_xywh, Size_wh resize):
    VideoCapture(){
    this->filename = filename;
    this->crop_xywh = crop_xywh;
    this->resize = resize;
    this->pix_fmt = isColor ? "bgr24" : "gray";
    initializer();
}

VideoCaptureStreamRT::VideoCaptureStreamRT(const std::string& filename, std::string pix_fmt,
    std::tuple<int, int, int, int> crop_xywh, Size_wh resize):
    VideoCapture(){
    this->filename = filename;
    this->crop_xywh = crop_xywh;
    this->resize = resize;
    this->pix_fmt = pix_fmt;
    initializer();
}

void VideoCaptureStreamRT::initializer() {
    VideoInfo videoinfo = get_info_stream(filename);
    origin_width = width = videoinfo.width;
    origin_height = height = videoinfo.height;
    codec = videoinfo.codec;
    fps = videoinfo.fps;
    duration = 0;
    count = 0;
    iframe = -1;
    default_buffer = NULL;
    waitInit = true;

    assert(width % 2 == 0 && "Height must be even");
    assert(height % 2 == 0 && "Width must be even");

    std::tuple<Size_wh, Size_wh, std::string> filter_options = get_videofilter_cpu(
        {width, height}, pix_fmt, crop_xywh, resize);
    size_wh = std::get<1>(filter_options);
    std::string filterstr = std::get<2>(filter_options);
    width = size_wh.width;
    height = size_wh.height;

    std::string rtsp_opt = startsWith(filename, "rtsp://") ? "-rtsp_flags prefer_tcp -pkt_size 736 " : "";

    // 初始化 ffmpeg 的 VideoCapture
    std::ostringstream oss;
    oss << "ffmpeg -y -loglevel warning " << rtsp_opt
        << "-i \"" << filename << "\" -an -map 0:v -f rawvideo "
        << filterstr << " -pix_fmt " << pix_fmt << " pipe:";

    ffmpeg_cmd = oss.str();
    std::cout << "ffmpeg_cmd: " << ffmpeg_cmd << std::endl;

    // 计算每帧的位数
    outnumpyshape = get_outnumpyshape(size_wh, pix_fmt);
    bytes_per_frame = 1;
    for (int num : outnumpyshape) {
        bytes_per_frame *= num;
    }
}

std::string decoder_to_nvidia(const std::string& codec) {
    if (codec == "av1")  return "av1_cuvid";
    if (codec == "h264") return "h264_cuvid";
    if (codec == "x264") return "h264_cuvid";
    if (codec == "hevc") return "hevc_cuvid";
    if (codec == "x265") return "hevc_cuvid";
    if (codec == "h265") return "hevc_cuvid";
    if (codec == "mjpeg") return "mjpeg_cuvid";
    if (codec == "mpeg1video") return "mpeg1_cuvid";
    if (codec == "mpeg2video") return "mpeg2_cuvid";
    if (codec == "mpeg4") return "mpeg4_cuvid";
    if (codec == "vp1") return "vp1_cuvid";
    if (codec == "vp8") return "vp8_cuvid";
    if (codec == "vp9") return "vp9_cuvid";
    throw std::runtime_error("No NV codec found for " + codec);
}

std::string encoder_to_nvidia(const std::string& codec) {
    if (codec == "")  return "hevc_nvenc";
    if (codec == "h264") return "h264_nvenc";
    if (codec == "x264") return "h264_nvenc";
    if (codec == "hevc") return "hevc_nvenc";
    if (codec == "x265") return "hevc_nvenc";
    if (codec == "h265") return "hevc_nvenc";
    return codec + "_nvenc";
}

int get_num_NVIDIA_GPUs() {
    static int num_NVIDIA_GPUs = -1;
    if (num_NVIDIA_GPUs == -1) {
        std::cout << "Getting number of NVIDIA GPUs...\n";
        std::string cmd = "ffmpeg -f lavfi -i nullsrc -c:v h264_nvenc -gpu list -f null - 2>&1";
        std::string output = execute_command(cmd);

        std::regex pattern(R"(GPU #\d+ - < )");
        auto nv_info_begin = std::sregex_iterator(output.begin(), output.end(), pattern);
        auto nv_info_end = std::sregex_iterator();

        num_NVIDIA_GPUs = std::distance(nv_info_begin, nv_info_end);
    }
    return num_NVIDIA_GPUs;
}


std::tuple<Size_wh, Size_wh, std::string, std::string> get_videofilter_gpu(
    Size_wh originsize, std::string pix_fmt, std::tuple<int, int, int, int> crop_xywh, Size_wh resize
){
    static const std::vector<std::string> allowed_pix_fmts = {"rgb24", "bgr24", "yuv420p", "yuvj420p", "nv12", "gray"};
    assert(std::find(allowed_pix_fmts.begin(), allowed_pix_fmts.end(), pix_fmt) != allowed_pix_fmts.end());
    int origin_width = originsize.width;
    int origin_height = originsize.height;
    int crop_x = std::get<0>(crop_xywh);
    int crop_y = std::get<1>(crop_xywh);
    int crop_w = std::get<2>(crop_xywh);
    int crop_h = std::get<3>(crop_xywh);
    int resize_width = resize.width;
    int resize_height = resize.height;

    std::string cropopt;
    if (crop_w != 0 && crop_h != 0) {
        assert(crop_x % 2 == 0 && crop_y % 2 == 0 && crop_w % 2 == 0 && crop_h % 2 == 0);
        assert(crop_w <= origin_width && crop_h <= origin_height);
        int top(crop_y);
        int left(crop_x);
        int bottom(origin_height - (crop_h + crop_y));
        int right(origin_width - (crop_x + crop_w));
        cropopt = "-crop" + std::to_string(top) + "x" + std::to_string(bottom) +
                    "x" + std::to_string(left) + "x" + std::to_string(right);
    } else {
        crop_w = origin_width;
        crop_h = origin_height;
        cropopt = "";
    }
    Size_wh cropsize = {crop_w, crop_h};
    Size_wh final_size_wh = cropsize;

    std::string scaleopt="";
    std::string padopt="";
    if (!resize.empty() && (resize_width != 0 || resize_height != 0)) {
        assert (resize_width % 2 == 0 && resize_height % 2 == 0);
        final_size_wh = resize;
        scaleopt = "-resize " + std::to_string(resize_width) + "x" + std::to_string(resize_height);
    }

    std::string inputstr = cropopt + " " + scaleopt;
    std::string filterstr = "";
    std::string pix_fmt_opt = (pix_fmt == "gray") ? "extractplanes=y" : "";
    if (!pix_fmt_opt.empty()) {
        filterstr = "-vf " + pix_fmt_opt;
    }
    return std::make_tuple(cropsize, final_size_wh, inputstr, filterstr);
}


VideoCaptureNV::VideoCaptureNV():VideoCapture(){;}

VideoCaptureNV::VideoCaptureNV(const std::string& filename, int isColor,
    std::tuple<int, int, int, int> crop_xywh, Size_wh resize, int gpu):
    VideoCapture(){
    this->filename = filename;
    this->crop_xywh = crop_xywh;
    this->resize = resize;
    this->pix_fmt = isColor ? "bgr24" : "gray";
    this->gpu = gpu;
    initializer();
}

VideoCaptureNV::VideoCaptureNV(const std::string& filename, std::string pix_fmt,
    std::tuple<int, int, int, int> crop_xywh, Size_wh resize, int gpu):
    VideoCapture(){
    this->filename = filename;
    this->crop_xywh = crop_xywh;
    this->resize = resize;
    this->pix_fmt = pix_fmt;
    this->gpu = gpu;
    initializer();
}


void VideoCaptureNV::initializer() {
    int numGPU = get_num_NVIDIA_GPUs();
    assert (numGPU > 0 && "No NVIDIA GPU found");
    gpu = gpu % numGPU;

    VideoInfo videoinfo = get_info(filename);
    origin_width = width = videoinfo.width;
    origin_height = height = videoinfo.height;
    codec = videoinfo.codec;
    codec = decoder_to_nvidia(codec);
    fps = videoinfo.fps;
    duration = 0;
    count = 0;
    iframe = -1;
    default_buffer = NULL;
    waitInit = true;

    assert(width % 2 == 0 && "Height must be even");
    assert(height % 2 == 0 && "Width must be even");

    std::tuple<Size_wh, Size_wh, std::string, std::string> filter_options = get_videofilter_gpu(
        {width, height}, pix_fmt, crop_xywh, resize);
    size_wh = std::get<1>(filter_options);
    std::string inputstr = std::get<2>(filter_options);
    std::string filterstr = std::get<3>(filter_options);
    width = size_wh.width;
    height = size_wh.height;

    // 初始化 ffmpeg 的 VideoCapture
    std::ostringstream oss;
    oss << "ffmpeg -loglevel warning -hwaccel cuda -hwaccel_device "
        << gpu << " -vcodec " << codec << " " << inputstr << " -i \""
        << filename << "\" -f rawvideo "
        << filterstr << " -pix_fmt " << pix_fmt << " pipe:";

    ffmpeg_cmd = oss.str();
    std::cout << "ffmpeg_cmd: " << ffmpeg_cmd << std::endl;

    // 计算每帧的位数
    outnumpyshape = get_outnumpyshape(size_wh, pix_fmt);
    bytes_per_frame = 1;
    for (int num : outnumpyshape) {
        bytes_per_frame *= num;
    }
}

} // END NAMESPACE ffmpegcv


void* operator>>(ffmpegcv::VideoCapture& cap, void* frame) {
    cap.read(frame);
    return frame;
}

void operator>>(void* frame, ffmpegcv::VideoWriter& writer) {
    writer.write(frame);
}

void operator<<(ffmpegcv::VideoWriter& writer, void* frame) {
    writer.write(frame);
}

void operator>>(ffmpegcv::VideoCapture& cap, ffmpegcv::VideoWriter& writer) {
    void* frame = static_cast<void*>(cap.getBuffer());
    if (cap.read(frame)) {
        writer.write(frame);
    }
}
