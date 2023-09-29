#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

int main()
{
    Mat img = imread("001.jpg");
    Mat result;
    float scale = img.size().height / 640.0;
    resize(img, result, Size(640, img.size().height / scale));

    Mat channels[3];
    split(result.clone(), channels);
    channels[0] = channels[0] - channels[2];

    Mat thre;
    threshold(channels[0], thre, 100, 255, THRESH_BINARY);

    Mat blurImg;
    medianBlur(thre, blurImg, 3);

    Mat close;
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    morphologyEx(blurImg, close, MORPH_OPEN, kernel, Point(-1, -1), 1);
    morphologyEx(close, close, MORPH_CLOSE, kernel, Point(-1, -1), 7);

    vector<vector<Point>> contours;
    findContours(close, contours, RETR_LIST, CHAIN_APPROX_NONE);

    int maxContourIndex = 0;
    double maxContourSize = 0;
    for (int i = 0; i < contours.size(); ++i)
    {
        double contourSize = contourArea(contours[i]);
        if (contourSize > maxContourSize)
        {
            maxContourIndex = i;
            maxContourSize = contourSize;
        }
    }

    Mat hull;
    approxPolyDP(contours[maxContourIndex], hull, 12, true);
    polylines(result, hull, true, Scalar(255, 255, 0), 2);

    imshow("result", result);
    waitKey(0);

    return 0;
}
