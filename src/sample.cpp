//#include <iostream>
//#include <fcntl.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <linux/videodev2.h>
//#include <opencv2/opencv.hpp>

//#define DEVICE "/dev/video0"
////#define WIDTH 384
////#define HEIGHT 288
//#define WIDTH 384
//#define HEIGHT 288

//struct buffer {
//    void* start;
//    size_t length;
//};

//void yuyv_to_mat(const void* yuyv_data, cv::Mat& rgb_frame) {
//    cv::Mat yuyv(HEIGHT, WIDTH, CV_8UC2, (void*)yuyv_data);
//    cv::cvtColor(yuyv, rgb_frame, cv::COLOR_YUV2BGR_YUYV);
//}

//cv::Mat edgeEnhanceSobel(const cv::Mat& frame) {
//    if (frame.empty()) {
//        throw std::runtime_error("Input frame is empty!");
//    }

//    // 转换为灰度图（Sobel通常在单通道处理）
//    cv::Mat gray;
//    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

//    // 计算 x 和 y 方向的 Sobel 梯度
//    cv::Mat grad_x, grad_y;
//    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);  // x方向，内核大小3
//    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);  // y方向

//    // 转换为绝对值并缩放到 0-255
//    cv::convertScaleAbs(grad_x, grad_x);
//    cv::convertScaleAbs(grad_y, grad_y);

//    // 合并梯度（边缘强度图）
//    cv::Mat edges;
//    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edges);

//    // 将边缘图转换为彩色格式（与原图匹配）
//    cv::Mat edges_color;
//    cv::cvtColor(edges, edges_color, cv::COLOR_GRAY2BGR);

//    // 增强策略：原图 + 边缘图（通过权重控制增强强度）
//    cv::Mat enhanced;
//    float alpha = 0.7;  // 增强系数（0.5~1.0效果较明显）
//    cv::addWeighted(frame, 1.0, edges_color, alpha, 0, enhanced);

//    return enhanced;
//}


//// 伪彩增强函数（默认使用 COLORMAP_JET，支持自定义颜色映射）
//cv::Mat pseudoColorEnhance(const cv::Mat& frame, int colormap = cv::COLORMAP_JET) {
//    if (frame.empty()) {
//        throw std::runtime_error("Input frame is empty!");
//    }

//    cv::Mat gray;
//    // 若输入为彩色图，转换为灰度图；否则直接使用
//    if (frame.channels() == 3) {
//        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
//    } else {
//        gray = frame.clone();
//    }

//    // 可选：直方图均衡化增强对比度（取消注释启用）
//    // cv::equalizeHist(gray, gray);

//    // 应用伪彩颜色映射
//    cv::Mat pseudo_color;
//    cv::applyColorMap(gray, pseudo_color, colormap);

//    return pseudo_color;
//}



//int main() {
//    // 1. 打开设备
//    int fd = open(DEVICE, O_RDWR);
//    if (fd == -1) {
//        std::cerr << "无法打开设备: " << DEVICE << std::endl;
//        return -1;
//    }

//    // 2. 设置视频格式
//    v4l2_format fmt{};
//    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    fmt.fmt.pix.width = WIDTH;
//    fmt.fmt.pix.height = HEIGHT;
//    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
//    fmt.fmt.pix.field = V4L2_FIELD_NONE;

//    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
//        std::cerr << "设置格式失败" << std::endl;
//        close(fd);
//        return -1;
//    }

//    // 3. 申请缓冲区
//    v4l2_requestbuffers req{};
//    req.count = 4;
//    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    req.memory = V4L2_MEMORY_MMAP;

//    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
//        std::cerr << "申请缓冲区失败" << std::endl;
//        close(fd);
//        return -1;
//    }

//    // 4. 内存映射
//    buffer* buffers = new buffer[req.count];
//    for (unsigned int i = 0; i < req.count; ++i) {
//        v4l2_buffer buf{};
//        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        buf.memory = V4L2_MEMORY_MMAP;
//        buf.index = i;

//        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
//            std::cerr << "查询缓冲区失败" << std::endl;
//            close(fd);
//            return -1;
//        }

//        buffers[i].length = buf.length;
//        buffers[i].start = mmap(NULL, buf.length,
//                              PROT_READ | PROT_WRITE,
//                              MAP_SHARED,
//                              fd, buf.m.offset);

//        if (buffers[i].start == MAP_FAILED) {
//            std::cerr << "内存映射失败" << std::endl;
//            close(fd);
//            return -1;
//        }

//        // 将缓冲区加入队列
//        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
//            std::cerr << "缓冲区入队失败" << std::endl;
//            close(fd);
//            return -1;
//        }
//    }

//    // 5. 启动视频流
//    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
//        std::cerr << "启动视频流失败" << std::endl;
//        close(fd);
//        return -1;
//    }

////    cv::namedWindow("V4L2 Camera", cv::WINDOW_AUTOSIZE);

//    // 6. 捕获循环
//    while (true) {
//        fd_set fds;
//        FD_ZERO(&fds);
//        FD_SET(fd, &fds);

//        // 等待数据就绪
//        struct timeval tv{};
//        tv.tv_sec = 2;
//        int r = select(fd + 1, &fds, NULL, NULL, &tv);

//        if (r == -1) {
//            std::cerr << "select错误" << std::endl;
//            break;
//        }

//        // 获取帧数据
//        v4l2_buffer buf{};
//        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        buf.memory = V4L2_MEMORY_MMAP;

//        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) {
//            std::cerr << "获取缓冲区失败" << std::endl;
//            break;
//        }

//        // 转换并显示图像
//        cv::Mat frame;
//        yuyv_to_mat(buffers[buf.index].start, frame);
////        cv::imshow("V4L2 Camera", frame);
//        cv::namedWindow("camera", cv::WINDOW_NORMAL);//CV_WINDOW_NORMAL就是0
////        cv::imshow("camera", frame);
////        cv::Mat enhanced = edgeEnhanceSobel(frame);
//        cv::Mat enhanced = pseudoColorEnhance(frame);
//        cv::namedWindow("camera", cv::WINDOW_NORMAL);//CV_WINDOW_NORMAL就是0
//        cv::imshow("camera", frame);
//        cv::namedWindow("enhanced_camera", cv::WINDOW_NORMAL);//CV_WINDOW_NORMAL就是0
//        cv::imshow("enhanced_camera", enhanced);

//        // 重新加入队列
//        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
//            std::cerr << "缓冲区重新入队失败" << std::endl;
//            break;
//        }

//        // 退出条件
//        if (cv::waitKey(1) == 27) break;  // ESC退出
//    }

//    // 7. 清理资源
//    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    ioctl(fd, VIDIOC_STREAMOFF, &type);

//    for (unsigned int i = 0; i < req.count; ++i) {
//        munmap(buffers[i].start, buffers[i].length);
//    }

//    delete[] buffers;
//    close(fd);
//    cv::destroyAllWindows();

//    return 0;
//}











//#include <iostream>
//#include <fcntl.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <linux/videodev2.h>
//#include <opencv2/opencv.hpp>
//#include <chrono>
//#include <string>

//#define DEVICE "/dev/video0"
//#define WIDTH 384
//#define HEIGHT 288

//// ====================== 截图功能结构体 ======================
//struct ScreenshotContext {
//    std::string save_path;
//    bool button_pressed;
//    cv::Rect button_rect;
//    int screenshot_counter;
//    bool show_button_highlight;

//    ScreenshotContext(const std::string& path) :
//        save_path(path),
//        button_pressed(false),
////        button_rect(10, 10, 120, 40),
//        button_rect(10, 10, 20, 20),
//        screenshot_counter(0),
//        show_button_highlight(false) {}
//};

//void screenshotMouseCallback(int event, int x, int y, int, void* userdata) {
//    ScreenshotContext* ctx = static_cast<ScreenshotContext*>(userdata);
//    if (event == cv::EVENT_LBUTTONDOWN && ctx->button_rect.contains(cv::Point(x, y))) {
//        ctx->button_pressed = true;
//        ctx->show_button_highlight = true;
//    }
//}

//void showFrameWithScreenshot(cv::Mat& frame, ScreenshotContext& ctx) {
//    static bool first_call = true;
//    if (first_call) {
//        cv::namedWindow("Camera (Screenshot)", cv::WINDOW_AUTOSIZE);
//        cv::setMouseCallback("Camera (Screenshot)", screenshotMouseCallback, &ctx);
//        first_call = false;
//    }

//    cv::Mat display = frame.clone();
//    cv::Scalar btn_color = ctx.show_button_highlight ? cv::Scalar(0, 200, 0) : cv::Scalar(0, 150, 0);
//    cv::rectangle(display, ctx.button_rect, btn_color, -1);
////    cv::putText(display, "Screenshot", cv::Point(ctx.button_rect.x + 10, ctx.button_rect.y + 28),
////               cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

//    if (ctx.button_pressed) {
//        auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
//        std::string filename = ctx.save_path + "/capture_" +
//                              std::to_string(timestamp) + ".jpg";
//        cv::imwrite(filename, frame);
//        std::cout << "截图已保存: " << filename << std::endl;
//        ctx.button_pressed = false;
//        ctx.show_button_highlight = false;
//    }

//    cv::imshow("Camera (Screenshot)", display);
//}
//// ====================== 截图功能结束 ======================

//struct buffer {
//    void* start;
//    size_t length;
//};

//void yuyv_to_mat(const void* yuyv_data, cv::Mat& rgb_frame) {
//    cv::Mat yuyv(HEIGHT, WIDTH, CV_8UC2, (void*)yuyv_data);
//    cv::cvtColor(yuyv, rgb_frame, cv::COLOR_YUV2BGR_YUYV);
//}

//cv::Mat edgeEnhanceSobel(const cv::Mat& frame) {
//    if (frame.empty()) throw std::runtime_error("空输入帧");

//    cv::Mat gray, grad_x, grad_y;
//    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
//    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);
//    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);
//    cv::convertScaleAbs(grad_x, grad_x);
//    cv::convertScaleAbs(grad_y, grad_y);

//    cv::Mat edges;
//    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edges);
//    cv::Mat edges_color;
//    cv::cvtColor(edges, edges_color, cv::COLOR_GRAY2BGR);

//    cv::Mat enhanced;
//    cv::addWeighted(frame, 1.0, edges_color, 0.7, 0, enhanced);
//    return enhanced;
//}

//cv::Mat pseudoColorEnhance(const cv::Mat& frame, int colormap = cv::COLORMAP_JET) {
//    if (frame.empty()) throw std::runtime_error("空输入帧");

//    cv::Mat gray;
//    if (frame.channels() == 3) cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
//    else gray = frame.clone();

//    cv::Mat pseudo_color;
//    cv::applyColorMap(gray, pseudo_color, colormap);
//    return pseudo_color;
//}

//int main() {
//    // 初始化摄像头设备
//    int fd = open(DEVICE, O_RDWR);
//    if (fd == -1) {
//        std::cerr << "无法打开设备: " << DEVICE << std::endl;
//        return -1;
//    }

//    // 初始化截图上下文（保存到当前目录的 screenshots 文件夹）
//    ScreenshotContext screenshot_ctx("/home/nnewn/Desktop/AC020_SDK/libir_sample/sample/usb_stream_cmd/fig");

//    // 设置视频格式
//    v4l2_format fmt{};
//    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    fmt.fmt.pix = {WIDTH, HEIGHT, V4L2_PIX_FMT_YUYV, V4L2_FIELD_NONE};
//    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
//        std::cerr << "设置格式失败" << std::endl;
//        close(fd);
//        return -1;
//    }

//    // 申请缓冲区
//    v4l2_requestbuffers req{};
//    req.count = 4;
//    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    req.memory = V4L2_MEMORY_MMAP;
//    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
//        std::cerr << "申请缓冲区失败" << std::endl;
//        close(fd);
//        return -1;
//    }

//    // 内存映射
//    buffer* buffers = new buffer[req.count];
//    for (unsigned i = 0; i < req.count; ++i) {
//        v4l2_buffer buf{};
//        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        buf.memory = V4L2_MEMORY_MMAP;
//        buf.index = i;

//        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
//            std::cerr << "查询缓冲区失败" << std::endl;
//            close(fd);
//            return -1;
//        }

//        buffers[i].length = buf.length;
//        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
//                               MAP_SHARED, fd, buf.m.offset);
//        if (buffers[i].start == MAP_FAILED) {
//            std::cerr << "内存映射失败" << std::endl;
//            close(fd);
//            return -1;
//        }

//        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) {
//            std::cerr << "缓冲区入队失败" << std::endl;
//            close(fd);
//            return -1;
//        }
//    }

//    // 启动视频流
//    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
//        std::cerr << "启动视频流失败" << std::endl;
//        close(fd);
//        return -1;
//    }

//    // 主循环
//    while (true) {
//        fd_set fds;
//        FD_ZERO(&fds);
//        FD_SET(fd, &fds);
//        timeval tv{2, 0};

//        int r = select(fd + 1, &fds, NULL, NULL, &tv);
//        if (r == -1) break;

//        v4l2_buffer buf{};
//        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        buf.memory = V4L2_MEMORY_MMAP;
//        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1) break;

//        // 转换原始帧
//        cv::Mat frame;
//        yuyv_to_mat(buffers[buf.index].start, frame);

//        // 显示带截图按钮的原始画面
//        showFrameWithScreenshot(frame, screenshot_ctx);

//        // 显示增强后的画面
//        cv::Mat enhanced = pseudoColorEnhance(frame);
//        cv::imshow("Enhanced View", enhanced);

//        // 重新入队缓冲区
//        if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) break;

//        // ESC退出
//        if (cv::waitKey(1) == 27) break;
//    }

//    // 清理资源
//    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    ioctl(fd, VIDIOC_STREAMOFF, &type);
//    for (unsigned i = 0; i < req.count; ++i)
//        munmap(buffers[i].start, buffers[i].length);
//    delete[] buffers;
//    close(fd);
//    cv::destroyAllWindows();

//    return 0;
//}






#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <string>

#define DEVICE "/dev/video0"
#define WIDTH 384
#define HEIGHT 288

// ===================== algorithm ======================
void do_CLAHE(cv::Mat& src,cv::Mat& dst){
    //int clip_limit = 3; // 定义限制对比度的参数  best_config
//    int clip_limit = 4.5; // 定义限制对比度的参数
    int clip_limit = 2; // 定义限制对比度的参数
    int tile_size = 1; // 定义块的大小

    cv::Ptr<cv::CLAHE> clahe = createCLAHE(clip_limit, cv::Size(tile_size, tile_size)); // 创建CLAHE对象
    clahe->apply(src, dst); // 应用CLAHE算法
}

void do_CLAHE_sobelprewitt(cv::Mat& src,cv::Mat& dst){
    //int clip_limit = 3; // 定义限制对比度的参数  best_config
//    int clip_limit = 4.5; // 定义限制对比度的参数
    float clip_limit = 3.5; // 定义限制对比度的参数
    int tile_size = 1; // 定义块的大小

    cv::Ptr<cv::CLAHE> clahe = createCLAHE(clip_limit, cv::Size(tile_size, tile_size)); // 创建CLAHE对象
    clahe->apply(src, dst); // 应用CLAHE算法
}

void do_CLAHE_edge(cv::Mat& src,cv::Mat& dst){
    float clip_limit = 1.3; // 定义限制对比度的参数
    //int tile_size = 8; // 定义块的大小
    int tile_size = 1; // 定义块的大小

    cv::Ptr<cv::CLAHE> clahe = createCLAHE(clip_limit, cv::Size(tile_size, tile_size)); // 创建CLAHE对象
    clahe->apply(src, dst); // 应用CLAHE算法
}


void do_CLAHE_edge_Frei_Chen(cv::Mat& src,cv::Mat& dst){
    int clip_limit = 3; // 定义限制对比度的参数
    //int tile_size = 8; // 定义块的大小
    int tile_size = 1; // 定义块的大小

    cv::Ptr<cv::CLAHE> clahe = createCLAHE(clip_limit, cv::Size(tile_size, tile_size)); // 创建CLAHE对象
    clahe->apply(src, dst); // 应用CLAHE算法
}


cv::Mat SobelPrewitt(cv::Mat img) {
    // ---------------- Sobel 边缘检测 ----------------
    cv::Mat sobel_x, sobel_y, sobel_magnitude;
    cv::Sobel(img, sobel_x, CV_64F, 1, 0, 3);
    cv::Sobel(img, sobel_y, CV_64F, 0, 1, 3);
    cv::magnitude(sobel_x, sobel_y, sobel_magnitude);

    // ---------------- Prewitt 边缘检测 ----------------
    cv::Mat prewitt_x, prewitt_y, prewitt_magnitude;
    cv::Mat kernel_prewitt_x = (cv::Mat_<float>(3,3) << -1, 0, 1,
                                                        -1, 0, 1,
                                                        -1, 0, 1);
    cv::Mat kernel_prewitt_y = (cv::Mat_<float>(3,3) <<  1,  1,  1,
                                                         0,  0,  0,
                                                        -1, -1, -1);
    cv::filter2D(img, prewitt_x, CV_64F, kernel_prewitt_x);
    cv::filter2D(img, prewitt_y, CV_64F, kernel_prewitt_y);
    cv::magnitude(prewitt_x, prewitt_y, prewitt_magnitude);

    // ---------------- 综合两者 ----------------
    cv::Mat combined_magnitude_64F;
    cv::addWeighted(sobel_magnitude, 0.5, prewitt_magnitude, 0.5, 0, combined_magnitude_64F);

    // 将最终结果转换为 CV_8U 以便显示或保存
    cv::Mat combined_magnitude_8U;
    combined_magnitude_64F.convertTo(combined_magnitude_8U, CV_8U);

    return combined_magnitude_8U;
}


cv::Mat Kirsch(cv::Mat img){
   // 定义 8 个 Kirsch 卷积核
   std::vector<cv::Mat> kirsch_kernels = {
       (cv::Mat_<float>(3,3) <<  5,  5,  5,
                                -3,  0, -3,
                                -3, -3, -3), // N

       (cv::Mat_<float>(3,3) <<  5,  5, -3,
                                 5,  0, -3,
                                -3, -3, -3), // NE

       (cv::Mat_<float>(3,3) <<  5, -3, -3,
                                 5,  0, -3,
                                 5, -3, -3), // E

       (cv::Mat_<float>(3,3) << -3, -3, -3,
                                 5,  0, -3,
                                 5,  5, -3), // SE

       (cv::Mat_<float>(3,3) << -3, -3, -3,
                                -3,  0, -3,
                                 5,  5,  5), // S

       (cv::Mat_<float>(3,3) << -3, -3, -3,
                                -3,  0,  5,
                                -3,  5,  5), // SW

       (cv::Mat_<float>(3,3) << -3, -3,  5,
                                -3,  0,  5,
                                -3, -3,  5), // W

       (cv::Mat_<float>(3,3) << -3,  5,  5,
                                -3,  0,  5,
                                -3, -3, -3)  // NW
   };

   // 初始化最大响应图
   cv::Mat max_response = cv::Mat::zeros(img.size(), CV_32F);

   // 对每个方向卷积并保留最大值
   for (const auto& kernel : kirsch_kernels) {
       cv::Mat response;
       cv::filter2D(img, response, CV_32F, kernel);
       cv::max(max_response, response, max_response);
   }

   // 转换为 8 位图像以便显示
   cv::Mat kirsch_edge;
   max_response.convertTo(kirsch_edge, CV_8U);

   return kirsch_edge;
}


cv::Mat Frei_Chen(cv::Mat img) {
    // 定义 sqrt(2)
    float sqrt2 = std::sqrt(2.0f);

    // Frei-Chen X方向卷积核
    cv::Mat kernel_x = (cv::Mat_<float>(3, 3) <<
        1,      sqrt2,  1,
        0,      0,      0,
       -1,     -sqrt2, -1);

    // Frei-Chen Y方向卷积核
    cv::Mat kernel_y = (cv::Mat_<float>(3, 3) <<
         1,     0,    -1,
         sqrt2, 0, -sqrt2,
         1,     0,    -1);

    // 进行卷积运算
    cv::Mat frei_x, frei_y;
    cv::filter2D(img, frei_x, CV_32F, kernel_x);
    cv::filter2D(img, frei_y, CV_32F, kernel_y);

    // 计算幅值图
    cv::Mat magnitude;
    cv::magnitude(frei_x, frei_y, magnitude);

    // 转换为 8 位图像
    cv::Mat edge_output;
    magnitude.convertTo(edge_output, CV_8U);

    return edge_output;
}

cv::Mat unsharpMasking(cv::Mat &input,
                      double sigma = 1.5,
                      double strength = 1.0,
                      int ksize = 5)
{
    // 参数校验
    CV_Assert(ksize > 0 && ksize % 2 == 1);
    CV_Assert(strength >= 0);

    cv::Mat blurred, sharpened;

    // Step 1: 高斯模糊（创建非锐化掩模的基础）
    cv::GaussianBlur(input, blurred,
                    cv::Size(ksize, ksize), // 核尺寸
                    sigma,                  // X方向标准差
                    sigma,                  // Y方向标准差（设为相同值）
                    cv::BORDER_REPLICATE);  // 边界处理方式

    // Step 2: 计算锐化图像（原图 + (原图 - 模糊图) * 强度）
    if (input.channels() == 1) { // 灰度图处理
        cv::subtract(input, blurred, sharpened);
        cv::addWeighted(input, 1.0,
                       sharpened, strength,
                       0.0,       // 偏移量
                       sharpened);
    } else { // 彩色图处理（逐通道处理）
        std::vector<cv::Mat> channels;
        cv::split(input, channels);

        for (auto &ch : channels) {
            cv::Mat ch_blurred, ch_sharpened;
            cv::GaussianBlur(ch, ch_blurred,
                            cv::Size(ksize, ksize),
                            sigma, sigma,
                            cv::BORDER_REPLICATE);
            cv::subtract(ch, ch_blurred, ch_sharpened);
            cv::addWeighted(ch, 1.0,
                           ch_sharpened, strength,
                           0.0,
                           ch);
        }
        cv::merge(channels, sharpened);
    }

    // 处理像素溢出（确保值在0-255之间）
    sharpened.convertTo(sharpened, CV_8U);
    return sharpened;
}



//egde enhancement bsed on sobelprewitt
cv::Mat edgeSobelPrewitt(const cv::Mat& frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
//    do_CLAHE(gray,gray);
    do_CLAHE_sobelprewitt(gray,gray);
    cv::Mat dst = SobelPrewitt(gray);
    {
    // 将灰度图转换为 BGR 三通道图
    cv::Mat edge_bgr;
    cv::cvtColor(dst, edge_bgr, cv::COLOR_GRAY2BGR);

    // 分离通道
    std::vector<cv::Mat> channels(3);
    cv::split(edge_bgr, channels);

    // 红色和蓝色通道清零
    channels[0] = cv::Mat::zeros(channels[0].size(), channels[0].type());  // 蓝色
    channels[2] = cv::Mat::zeros(channels[2].size(), channels[2].type());  // 红色

    // 可选：阈值化绿色通道以增强高亮
    // cv::threshold(channels[1], channels[1], 70, 255, cv::THRESH_BINARY);

    // 或者用掩码增强高亮（例如 >70 的像素提亮为 255）
    cv::Mat mask = (channels[1] > 70);
    channels[1].setTo(255, mask);  // 绿色通道

    // 合并回三通道图像
    cv::Mat dst;
    cv::merge(channels, dst);
    }
    cv::Mat show_mat = dst;
    return show_mat;
}

//egde enhancement bsed on kirsch
cv::Mat kirsch(const cv::Mat frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    do_CLAHE_edge(gray,gray);

    cv::Mat dst = Kirsch(gray);
//    dst = AddnonCircle(dst);
    cv::Mat show_mat = dst;
    return show_mat;
}



//egde enhancement bsed on Frei_Chen
cv::Mat frei_Chen(const cv::Mat frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    do_CLAHE_edge_Frei_Chen(gray,gray);
    cv::Mat dst = Frei_Chen(gray);
//    dst = AddnonCircle(dst);
    cv::Mat show_mat = dst;
    return show_mat;

}


cv::Mat defaultmethod(const cv::Mat frame) {
//    float temp2 = 55.0;
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    gray = unsharpMasking(gray, 1.5, 1.0, 5);
    cv::Mat temp2;
//    do_CLAHE(gray,temp2);
    do_CLAHE(gray,gray);
    // do_CLAHE250110(gray,temp2);
    // do_USM(temp2, dst);

//    dst = AddCircle(temp2);

//    show_mat = dst;
//    dst = AddnonCircle(dst);
    cv::Mat show_mat = gray;
    return show_mat;

}



// ===================== algorithm ======================


// ====================== 增强型UI上下文 ======================
struct AppContext {
    // 截图相关
    std::string save_path;
    bool screenshot_button_pressed;
    cv::Rect screenshot_button_rect;
    int screenshot_counter;
    bool show_screenshot_highlight;

    // 算法切换相关
    bool algorithm_button_pressed;
    cv::Rect algorithm_button_rect;
    bool show_algorithm_highlight;
    int current_algorithm; // 0: 无增强, 1: 边缘增强
    bool exit_button_pressed,exit_requested;
    bool show_exit_highlight;
//    bool exit_requested;
    cv::Rect exit_button_rect;
    AppContext(const std::string& path) :
        save_path(path),
        screenshot_button_pressed(false),
        screenshot_button_rect(10, 10, 20, 20),
        screenshot_counter(0),
        show_screenshot_highlight(false),
        algorithm_button_pressed(false),
        algorithm_button_rect(10, 35, 20, 20),
        show_algorithm_highlight(false),
//        current_algorithm(1) {} ,// 默认使用边缘增强
        current_algorithm(1),// 默认使用边缘增强
        exit_button_pressed(false),
        exit_button_rect(10, 60, 20, 20), // 退出按钮位置
        show_exit_highlight(false),
        exit_requested(false) {}
};

void mouseCallback(int event, int x, int y, int, void* userdata) {
    AppContext* ctx = static_cast<AppContext*>(userdata);

    // 截图按钮事件
    if (event == cv::EVENT_LBUTTONDOWN && ctx->screenshot_button_rect.contains(cv::Point(x, y))) {
        ctx->screenshot_button_pressed = true;
        ctx->show_screenshot_highlight = true;
    }

    // 算法切换按钮事件
    if (event == cv::EVENT_LBUTTONDOWN && ctx->algorithm_button_rect.contains(cv::Point(x, y))) {
        ctx->algorithm_button_pressed = true;
        ctx->show_algorithm_highlight = true;
    }

    // exit按钮事件
    if (event == cv::EVENT_LBUTTONDOWN && ctx->exit_button_rect.contains(cv::Point(x, y))) {
        ctx->exit_button_pressed = true;
        ctx->show_exit_highlight = true;
         ctx->exit_requested = true; // 设置退出请求标志
    }
}

void showFrameWithUI(cv::Mat& frame, AppContext& ctx) {
    float factor=2;
    static bool first_call = true;
//    if (first_call) {
        cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
        cv::resizeWindow("Camera",384*factor,288*factor);
//        cv::namedWindow("Camera", );
//    cv::namedWindow("Camera", cv::WINDOW_NORMAL);
//    cv::setWindowProperty("Camera", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
    cv::setMouseCallback("Camera", mouseCallback, &ctx);
        first_call = false;
//    }

//    cv::Size upsize=cv::Size(384*factor,288*factor);
//    cv::Mat display;
//    cv::resize(frame,display,upsize,cv::INTER_AREA);
      cv::Mat display=frame;
//    cv::Mat display = frame.clone();
//    cv::Mat updisplay = frame.clone();
//    cv::Mat updisplay = cv::Mat::zeros(384*3, 288*3, CV_8UC3);

    // 绘制截图按钮
    cv::Scalar screenshot_btn_color = ctx.show_screenshot_highlight ?
        cv::Scalar(0, 200, 0) : cv::Scalar(0, 150, 0);
    cv::rectangle(display, ctx.screenshot_button_rect, screenshot_btn_color, -1);
//    cv::putText(display, "",
//               cv::Point(ctx.screenshot_button_rect.x + 10, ctx.screenshot_button_rect.y + 28),
//               cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

    // 绘制算法切换按钮
    cv::Scalar algo_btn_color = ctx.show_algorithm_highlight ?
        cv::Scalar(200, 0, 0) : cv::Scalar(150, 0, 0);
    cv::rectangle(display, ctx.algorithm_button_rect, algo_btn_color, -1);

    // 根据当前算法显示不同的文本
    int al_num =5 ;
    std::string algo_text;
    if (ctx.current_algorithm == 1){
        algo_text="default";
    }
    else if(ctx.current_algorithm == 2)
    {
        algo_text="SobelPrewitt";
    }
    else if(ctx.current_algorithm == 3){
        algo_text="kirsch";
    }
    else if(ctx.current_algorithm == 4){
        algo_text="frei_Chen";
    }
    else {
        algo_text="ori";
    }
    cv::putText(display, algo_text,
               cv::Point(ctx.screenshot_button_rect.x+20 , ctx.screenshot_button_rect.y+15),
               cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);

    // 绘制算法切换按钮
    cv::Scalar exit_btn_color = ctx.show_exit_highlight ?
        cv::Scalar(0, 0, 200) : cv::Scalar(0, 0, 150);
    cv::rectangle(display, ctx.exit_button_rect, exit_btn_color, -1);
    cv::putText(display, "X", // 添加"X"表示退出
               cv::Point(ctx.exit_button_rect.x + 5, ctx.exit_button_rect.y + 15),
               cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);


    // 处理截图按钮点击
    if (ctx.screenshot_button_pressed) {
        auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        std::string filename = ctx.save_path + "/capture_" +algo_text+
                              std::to_string(timestamp) + ".jpg";
        cv::imwrite(filename, frame);
        std::cout << "截图已保存: " << filename << std::endl;
        ctx.screenshot_button_pressed = false;
        ctx.show_screenshot_highlight = false;
    }

    // 处理算法切换按钮点击
//    int al_num =4 ;
    if (ctx.algorithm_button_pressed) {
//        ctx.current_algorithm = (ctx.current_algorithm == 1) ? 0 : 1; // 切换状态
        ctx.current_algorithm = (ctx.current_algorithm ==(al_num) )?1:ctx.current_algorithm +1;

//        std::cout << "算法切换: "
//                  << (ctx.current_algorithm == 1 ? "边缘增强" : "原始图像")
//                  << std::endl;
        ctx.algorithm_button_pressed = false;
        ctx.show_algorithm_highlight = false;
    }

    cv::imshow("Camera", display);
//    cv::Size upsize=cv::Size(384*3,288*3);
//    cv::Mat upimg;
//    cv::resize(display,upimg,upsize,cv::INTER_AREA);
//    cv::imshow("Camera", upimg);
}
// ====================== UI功能结束 ======================

struct buffer {
    void* start;
    size_t length;
};

void yuyv_to_mat(const void* yuyv_data, cv::Mat& rgb_frame) {
    cv::Mat yuyv(HEIGHT, WIDTH, CV_8UC2, (void*)yuyv_data);
    cv::cvtColor(yuyv, rgb_frame, cv::COLOR_YUV2BGR_YUYV);
}


// 伪彩增强函数（默认使用 COLORMAP_JET，支持自定义颜色映射）
cv::Mat pseudoColorEnhance(const cv::Mat& frame, int colormap = cv::COLORMAP_JET) {
    if (frame.empty()) {
        throw std::runtime_error("Input frame is empty!");
    }

    cv::Mat gray;
    // 若输入为彩色图，转换为灰度图；否则直接使用
    if (frame.channels() == 3) {
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = frame.clone();
    }

    // 可选：直方图均衡化增强对比度（取消注释启用）
    // cv::equalizeHist(gray, gray);

    // 应用伪彩颜色映射
    cv::Mat pseudo_color;
    cv::applyColorMap(gray, pseudo_color, colormap);

    return pseudo_color;
}


cv::Mat edgeEnhanceSobel(const cv::Mat& frame) {
    if (frame.empty()) throw std::runtime_error("空输入帧");

    cv::Mat gray, grad_x, grad_y;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::Sobel(gray, grad_x, CV_16S, 1, 0, 3);
    cv::Sobel(gray, grad_y, CV_16S, 0, 1, 3);
    cv::convertScaleAbs(grad_x, grad_x);
    cv::convertScaleAbs(grad_y, grad_y);

    cv::Mat edges;
    cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edges);
    cv::Mat edges_color;
    cv::cvtColor(edges, edges_color, cv::COLOR_GRAY2BGR);

    cv::Mat enhanced;
    cv::addWeighted(frame, 1.0, edges_color, 0.7, 0, enhanced);
    return enhanced;
}

// 新增：原始图像处理（直接返回）
cv::Mat noEnhancement(const cv::Mat& frame) {
    return frame.clone();
}

int main() {
    // 打开摄像头设备
    int fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("打开设备失败");
        return EXIT_FAILURE;
    }

    // 设置视频格式
    v4l2_format fmt = {};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("设置格式失败");
        close(fd);
        return EXIT_FAILURE;
    }

    // 请求缓冲区
    v4l2_requestbuffers req = {};
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("请求缓冲区失败");
        close(fd);
        return EXIT_FAILURE;
    }

    // 映射缓冲区
    buffer* buffers = new buffer[req.count];
    for (int i = 0; i < req.count; ++i) {
        v4l2_buffer buf = {};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("查询缓冲区失败");
            close(fd);
            return EXIT_FAILURE;
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length,
                               PROT_READ | PROT_WRITE,
                               MAP_SHARED,
                               fd, buf.m.offset);

        if (buffers[i].start == MAP_FAILED) {
            perror("内存映射失败");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    // 入队所有缓冲区
    for (int i = 0; i < req.count; ++i) {
        v4l2_buffer buf = {};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("缓冲区入队失败");
            close(fd);
            return EXIT_FAILURE;
        }
    }

    // 开始视频流
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("启动流失败");
        close(fd);
        return EXIT_FAILURE;
    }

    // 创建UI上下文
//    AppContext ctx("./screenshots");
    AppContext ctx("/home/nnewn/Desktop/AC020_SDK/libir_sample/sample/usb_stream_cmd/fig");

    // 主循环
    while (true) {
        // 获取一帧
        v4l2_buffer buf = {};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("获取帧失败");
            break;
        }

        // 转换格式
        cv::Mat frame;
        yuyv_to_mat(buffers[buf.index].start, frame);

        // 应用当前选择的算法
        cv::Mat processed_frame;
        float factor2=2;
        cv::Size upsize=cv::Size(384*factor2,288*factor2);
//        cv::Mat display;
        cv::resize(frame,frame,upsize,cv::INTER_AREA);

        if (ctx.current_algorithm == 1) {
            processed_frame = defaultmethod(frame);
//              processed_frame = edgeEnhanceSobel(frame);
        }
        else if((ctx.current_algorithm == 2)){
//            processed_frame = noEnhancement(frame);
              processed_frame = edgeSobelPrewitt(frame);
        }
        else if((ctx.current_algorithm == 3)){
            processed_frame = kirsch(frame);
        }
        else if((ctx.current_algorithm == 4)){
            processed_frame = frei_Chen(frame);
        }
        else{
            processed_frame = noEnhancement(frame);
//                       processed_frame = kirsch(frame);
        }


            // 显示带UI的帧
        showFrameWithUI(processed_frame, ctx);

        // 重新入队缓冲区
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("缓冲区重新入队失败");
            break;
        }

        if (ctx.exit_requested) {
            std::cout << "退出按钮被点击，程序即将退出..." << std::endl;
            break;
        }

        // 检查退出键
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    // 停止视频流
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(fd, VIDIOC_STREAMOFF, &type);

    // 清理资源
    for (int i = 0; i < req.count; ++i) {
        munmap(buffers[i].start, buffers[i].length);
    }
    delete[] buffers;
    close(fd);

    return EXIT_SUCCESS;
}



