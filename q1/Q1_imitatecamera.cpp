#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <fstream>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace std;

// Constants
const int centerx = 190;
const int centery = 160;
constexpr int rows = 720;
constexpr int cols = 1280;

// Forward declarations
class CameraConverter;

// A helper struct to store 3D points
struct Point3D
{
    double x, y, z;
};

int main()
{
    CameraConverter *converter = new CameraConverter();
    converter->initialize();
    converter->convertWorldToPixel();
    converter->outputResults();
    delete converter;
    return 0;
}

class CameraConverter
{
public:
    // Constructor
    CameraConverter()
    {
        // Initialize camera parameters
        initializeCamera();
    }

    // Initialize camera parameters
    void initializeCamera()
    {
        K << 400., 0., centerx, 0.,
            0., 400., centery, 0.,
            0., 0., 1., 0.;
        T << 2., 2., 2.;
        q = Eigen::Quaterniond(-0.5, 0.5, 0.5, -0.5);
        R = q.toRotationMatrix();
        W = Eigen::Matrix4d::Zero();
        W.block(0, 0, 3, 3) = R.transpose();
        W.block(0, 3, 3, 1) = -R.transpose() * T;
        W(3, 3) = 1;
    }

    // Initialize points from a file
    void initialize()
    {
        ifstream fin("points.txt");
        fin >> n;
        for (int idx = 0; idx < n; idx++)
        {
            double data[3];
            for (auto &tmp : data)
                fin >> tmp;
            pointsWorld.push_back({data[0], data[1], data[2]});
        }
    }

    // Convert world coordinates to pixel coordinates
    void convertWorldToPixel()
    {
        for (auto &point : pointsWorld)
        {
            Eigen::Vector4d pointHomo(point.x, point.y, point.z, 1.0);
            Eigen::Vector4d pixelHomo = K * W * pointHomo;
            pointsPixel.push_back({pixelHomo[0] / pixelHomo[2], pixelHomo[1] / pixelHomo[2]});
        }
    }

    // Output results
    void outputResults()
    {
        int cnt = 0;
        cv::Mat result_img = cv::Mat::zeros(cv::Size(cols, rows), CV_8UC3);
        for (auto point : pointsPixel)
        {
            cv::Point2f p;
            p.x = point.x, p.y = point.y;
            if (p.x < 0 || p.x > cols || p.y < 0 || p.y > rows)
                continue;
            cv::circle(result_img, p, 2, cv::Scalar(255, 255, 255), -1);
            cnt++;
        }

        cv::imshow("res", result_img);
        cv::waitKey(0);
        cv::imwrite("result.jpg", result_img);
        cout << "Total number of points that could be captured by camera:" << cnt << endl;
    }

private:
    int n;
    Eigen::Matrix<double, 3, 4> K;
    Eigen::Vector3d T;
    Eigen::Quaterniond q;
    Eigen::Matrix3d R;
    Eigen::Matrix4d W;
    vector<Point3D> pointsWorld;
    vector<cv::Point2f> pointsPixel;
};
