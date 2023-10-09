#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
    // 打开视频文件
    VideoCapture cap("armor.mp4");
    if (!cap.isOpened())
    {
        std::cerr << "Failed to open video file!" << std::endl;
        return -1;
    }

    namedWindow("Armor Detection", WINDOW_NORMAL);

    while (true)
    {
        Mat frame;
        cap >> frame;

        if (frame.empty())
        {
            break;
        }

        // 转换图像颜色空间为HSV
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        // 定义橙色的范围（包括偏橙色）
        Scalar lower_orange = Scalar(0, 70, 50);
        Scalar upper_orange = Scalar(30, 130, 255);

        // 筛选出橙色光源并绘制矩形框
        Mat mask;
        inRange(hsv, lower_orange, upper_orange, mask);

        // 进行亮度筛选
        Mat masked_hsv;
        hsv.copyTo(masked_hsv, mask);
        vector<Mat> channels;
        split(masked_hsv, channels);
        Mat value_channel = channels[2];

        // 设置亮度阈值
        int minBrightness = 100;
        threshold(value_channel, mask, minBrightness, 255, THRESH_BINARY);
        bitwise_and(mask, mask, mask, mask = mask);

        // 查找轮廓
        vector<vector<Point>> contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // 绘制满足条件的矩形框
        for (size_t i = 0; i < contours.size(); i++)
        {
            Rect rect = boundingRect(contours[i]);

            int minWidth = 10;  // 设置最小宽度
            int minHeight = 20; // 设置最小高度
            int maxWidth = 45;  // 设置最大宽度
            int maxHeight = 60; // 设置最大高度

            if (rect.width >= minWidth && rect.height >= minHeight && rect.width <= maxWidth && rect.height <= maxHeight)
            {
                rectangle(frame, rect, Scalar(0, 255, 0), 2); // 绘制矩形框
            }
        }

        imshow("Armor Detection", frame);

        char c = waitKey(25);
        if (c == 27)
        {
            break;
        }
    }

    destroyAllWindows();

    return 0;
}
