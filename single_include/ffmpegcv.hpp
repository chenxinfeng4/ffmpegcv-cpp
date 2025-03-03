#pragma once
#define FFMPEGCV_HPP

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <regex>
#include <cstdlib>


//================ Begin Video info ==================

// 函数：提取文件名的后缀名
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
    FILE* stream = popen(command.c_str(), "r");
    if (!stream) return result;

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stream) != nullptr) {
        result += buffer;
    }
    pclose(stream);
    return result;
}

struct VideoInfo {
    std::string codec;
    float duration = 0.0f;
    float fps = 0.0f;
    int width = 0;
    int height = 0;
    int count = 0;
    bool is_complex = false;
};

VideoInfo get_info(const std::string& filename) {
    static const std::vector<std::string> complex_formats = {"mkv", "flv", "ts"};

    // 判断是否为复杂格式
    const bool is_complex = std::find(complex_formats.begin(), complex_formats.end(), 
        get_file_extension(filename)) != complex_formats.end();

    // 构造ffprobe命令
    std::ostringstream cmd;
    cmd << "ffprobe -v quiet -print_format json=compact=1 -select_streams v:0";
    if (is_complex) cmd << " -count_packets";
    cmd << " -show_streams \"" << filename << "\"";

    // 执行命令并获取输出
    const std::string json_output = execute_command(cmd.str());
    VideoInfo info;
    info.is_complex = is_complex;

    static const std::regex codec_re( "codec_name.: .\"(\\w+)");
    static const std::regex width_re( "width.: (\\d+)");
    static const std::regex height_re( "height.: (\\d+)");
    static const std::regex frames_re( "nb_frames.: .(\\d+)");
    static const std::regex nb_read_packets_re( "nb_read_packets.: .(\\d+)");
    static const std::regex duration_re( "duration.: .([\\d.]+)");
    static const std::regex rate_re( "r_frame_rate.: .(\\d+)/(\\d+)");

    std::smatch match;
    auto count_re = is_complex? nb_read_packets_re : frames_re;
    
    // 提取编解码器名称
    if (std::regex_search(json_output, match, codec_re) && match.size() > 1) {
        info.codec = match[1];
    }

    // 提取画面宽度
    if (std::regex_search(json_output, match, width_re) && match.size() > 1) {
        info.width = std::stoi(match[1]);
    }

    // 提取画面高度
    if (std::regex_search(json_output, match, height_re) && match.size() > 1) {
        info.height = std::stoi(match[1]);
    }

    // 提取总帧数（字符串转数字）
    if (std::regex_search(json_output, match, count_re) && match.size() > 1) {
        info.count = std::stoi(match[1]);
    }

    // 提取持续时间（字符串转浮点）
    if (std::regex_search(json_output, match, duration_re) && match.size() > 1) {
        info.duration = strtof(match[1].str().c_str(), nullptr);
    }

    // 提取帧率并计算实际值
    if (std::regex_search(json_output, match, rate_re) && match.size() > 2) {
        float num = std::stof(match[1]);
        float den = std::stof(match[2]);
        if (den != 0) info.fps = float(num) / den;
    }

    return info;
};

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

class FFmpegVideoWriter {
public:
    // 构造函数
    FFmpegVideoWriter(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, int isColor = true)
        : filename(filename), codec(codec), fps(fps), size_wh(size_wh), width(size_wh.width), height(size_wh.height) {
        pix_fmt = isColor ? "bgr24" : "gray";
        _init();
    }

    FFmpegVideoWriter(const std::string& filename, const std::string& codec, double fps, Size_wh size_wh, std::string pix_fmt)
        : filename(filename), codec(codec), fps(fps), size_wh(size_wh), width(size_wh.width), height(size_wh.height), pix_fmt(pix_fmt) {
        _init();
    }

    void _init() {
        if (codec.empty()) codec = "x264"; // if codec empty, set to "x264"
        std::ostringstream oss;
        oss << "ffmpeg -y -loglevel warning -f rawvideo -pix_fmt " << pix_fmt << " -s " << width << "x" << height
            << " -r " << fps << " -i pipe: -c:v " << codec << " -pix_fmt yuv420p \"" << filename << "\"";
        std::string ffmpeg_cmd = oss.str();
        std::cout << "ffmpeg_cmd: " << ffmpeg_cmd << std::endl;
        process = popen(ffmpeg_cmd.c_str(), "w");
        if (!process) {
            std::cerr << "Failed to open video writer" << process << std::endl;
            return;
        }

        // 计算每帧的位数
        std::vector<int> innumpyshape = get_outnumpyshape(size_wh, pix_fmt);
        bytes_per_frame = 1;
        for (int num : innumpyshape) {
            bytes_per_frame *= num;
        }
    }

    // 析构函数
    ~FFmpegVideoWriter() {
        release();
    }

    // release
    void release() {
        if (process) {
            pclose(process);
            process = NULL;
        }
    }

    // close
    void close() {
        release();
    }

    // 写入一帧
    bool write(const void* __restrict__ frame) {
        if (frame == NULL) return false;
        if (!process) {
            std::cerr << "Failed to open video writer";
            return false;
        }
        fwrite(frame, sizeof(char), bytes_per_frame, process);
        return true;
    }

#ifdef OPENCV_CORE_TYPES_HPP
    bool write(cv::Mat& frame) {
        write(frame.data);
    }
#endif

    // 检查是否成功打开
    bool isOpened() const {
        return process;
    }

public:
    std::string filename="";       // 输出文件名
    std::string codec="x264";          // 编码格式
    double fps = 1.0f;             // 帧率
    Size_wh size_wh = {0, 0};        // 输入的尺寸
    int width = 0;
    int height = 0;
    std::string pix_fmt = "bgr24"; // 输入的像素格式
    FILE* process = NULL;          // 内部使用 VideoWriter
    std::vector<int> innumpyshape = {0, 0}; // 输入的尺寸
    int bytes_per_frame = 0;       // 每帧的位数
};

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

    // crop option
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

    // resize option
    std::string scaleopt="";
    std::string padopt="";
    if (!resize.empty() && (resize_width != 0 || resize_height != 0)) {
        assert (resize_width % 2 == 0 && resize_height % 2 == 0);
        int dst_width = resize_width;
        int dst_height = resize_height;
        scaleopt = "scale=" + std::to_string(dst_width) + "x" + std::to_string(dst_height);
    }

    //pix_fmt option
    std::string pix_fmt_opt = (pix_fmt == "gray") ? "extractplanes=y" : "";
    std::string filterstr = "";
    if (!cropopt.empty() || !scaleopt.empty() || !pix_fmt_opt.empty()) {
        filterstr = "-vf ";
        if (!cropopt.empty()) filterstr += cropopt + ",";
        if (!scaleopt.empty()) filterstr += scaleopt + ",";
        if (!pix_fmt_opt.empty()) filterstr += pix_fmt_opt + ",";
        filterstr = filterstr.substr(0, filterstr.size() - 1); // remove the last comma
    }
    return std::make_tuple(cropsize, final_size_wh, filterstr);
}

class FFmpegVideoCapture {
public:
    // 构造函数
    FFmpegVideoCapture(const std::string& filename, int isColor = true, 
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0}, Size_wh resize = {0, 0})
        : filename(filename), crop_xywh(crop_xywh), resize(resize) {
        pix_fmt = isColor ? "bgr24" : "gray";
        _init();
    }

    FFmpegVideoCapture(const std::string& filename, const std::string& pix_fmt,
        std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0}, Size_wh resize = {0, 0})
        : filename(filename), pix_fmt(pix_fmt), crop_xywh(crop_xywh), resize(resize) {
        _init();
    }

    void _init() {
        VideoInfo videoinfo = get_info(filename);
        origin_width = width = videoinfo.width;
        origin_height = height = videoinfo.height;
        codec = videoinfo.codec;
        fps = videoinfo.fps;
        iframe = -1;
        default_buffer = NULL;

        assert(width % 2 == 0 && "Height must be even");
        assert(height % 2 == 0 && "Width must be even");

        // 获取 filter options
        std::tuple<Size_wh, Size_wh, std::string> filter_options = get_videofilter_cpu(
            {width, height}, pix_fmt, crop_xywh, resize);
        Size_wh final_size_wh = std::get<1>(filter_options);
        std::string filterstr = std::get<2>(filter_options);
        width = final_size_wh.width;
        height = final_size_wh.height;

        // 初始化 ffmpeg 的 VideoCapture
        std::ostringstream oss;
        oss << "ffmpeg -y -loglevel warning -i \"" << filename << "\" -f rawvideo "
            << filterstr << " -pix_fmt " << pix_fmt << " pipe:";

        std::string ffmpeg_cmd = oss.str();
        std::cout << "ffmpeg_cmd: " << ffmpeg_cmd << std::endl;
        process = popen(ffmpeg_cmd.c_str(), "r");
        if (!process) {
            std::cerr << "Failed to open video reader" << std::endl;
            return;
        }

        // 计算每帧的位数
        size_wh = {width, height};
        outnumpyshape = get_outnumpyshape(size_wh, pix_fmt);
        bytes_per_frame = 1;
        for (int num : outnumpyshape) {
            bytes_per_frame *= num;
        }
    }

    // 析构函数
    ~FFmpegVideoCapture() {
        release();
    }

    // release
    void release() {
        if (default_buffer) {
            free(default_buffer);
            default_buffer = NULL;
        }
        if (process) {
            pclose(process);
            process = NULL;
        }
    }

    // close
    void close() {
        release();
    }

    void* getBuffer(){
        if (default_buffer == NULL){
            default_buffer = (void*)malloc(bytes_per_frame);
        }
        return default_buffer;
    }

    // 读取一帧
    bool read(void *__restrict__ frame) {
        if (process) {
            int bytesRead = fread(frame, sizeof(char), bytes_per_frame, process);
            if (bytesRead == bytes_per_frame) {
                iframe += 1;
                return true;
            } else {
                release();
                return false;
            }
        } else{
            return false;
        }
    }

#ifdef OPENCV_CORE_TYPES_HPP
    bool read(cv::Mat& frame) {
        read(frame.data);
    }
#endif

    // 检查是否成功打开
    bool isOpened() const {
        return process;
    }

    const int size() {
        return count;
    }

    const int len() {
        return count;
    }

public:
    std::string filename;   // 输入文件名
    std::string pix_fmt;    // 输出像素格式
    std::tuple<int, int, int, int> crop_xywh = {0, 0, 0, 0};
    Size_wh resize;
    FILE* process;          // 内部使用 VideoCapture
    int bytes_per_frame;    // 每帧的位数
    int width = 0;
    int height = 0;
    int origin_width = 0;
    int origin_height = 0;
    int count = 0;
    int iframe = -1;
    float fps = 0;
    float duration = 0;
    void* default_buffer;
    std::string codec = "";
    Size_wh size_wh = {0, 0};
    std::vector<int> outnumpyshape;
};

//================End Video Reader==================

void* operator>>(FFmpegVideoCapture& cap, void* __restrict__ frame) {
    cap.read(frame);
    return frame;
}

void operator>>(void* __restrict__ frame, FFmpegVideoWriter& writer) {
    writer.write(frame);
}

void operator>>(FFmpegVideoCapture& cap, FFmpegVideoWriter& writer) {
    void* frame = cap.getBuffer();
    if (cap.read(frame)) {
        writer.write(frame);
    }
}

FFmpegVideoWriter& operator<<(FFmpegVideoWriter& writer, FFmpegVideoCapture& cap) {
    void* frame = cap.getBuffer();
    if (cap.read(frame)) {
        writer.write(frame);
    }
    return writer;
}


