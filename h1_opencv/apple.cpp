#include <assert.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main()
{
    Mat img = imread("apple.png");
    assert(img.channels() == 3);

    Mat hsv;
    Mat channels[3];
    split(img, channels);
    cvtColor(img, hsv, COLOR_BGR2HSV);

    channels[2] -= channels[1];
    Mat norm_mat;
    normalize(channels[2], norm_mat, 0, 255., NORM_MINMAX);
    Mat result;
    Mat result2;
    threshold(norm_mat, result, 60, 255, THRESH_BINARY);
    threshold(channels[2], result2, 26, 255, THRESH_BINARY);

    Mat yellow_part;
    inRange(hsv, Scalar(10, 210, 10), Scalar(28, 255, 140), yellow_part);

    yellow_part = yellow_part & result2;
    result = yellow_part | result;

    Mat kernel1 = getStructuringElement(MORPH_CROSS, Size(30, 30));
    Mat kernel2 = getStructuringElement(MORPH_CROSS, Size(4, 4));
    Mat final_result;
    morphologyEx(result, final_result, MORPH_CLOSE, kernel1);
    morphologyEx(final_result, final_result, MORPH_OPEN, kernel2, Point(-1, -1), 3); // 去除苹果上方枝干的一小部分

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(final_result, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    for (size_t i = 0; i < contours.size(); i++)
    {
        if (contourArea(contours[i]) > 2000)
        {
            Rect rect = boundingRect(contours[i]);
            Mat contour_mask = Mat::zeros(img.size(), CV_8UC1);
            drawContours(contour_mask, contours, static_cast<int>(i), Scalar(255), FILLED);

            morphologyEx(contour_mask, contour_mask, MORPH_CLOSE, kernel1);

            rectangle(img, rect, Scalar(255, 255, 255), 1);
            drawContours(img, contours, static_cast<int>(i), Scalar(255, 255, 255), 2, LINE_8, hierarchy, 1);
        }
    }

    imshow("apple", img);
    imwrite("result.png", img);
    waitKey(0);
    return 0;
}
