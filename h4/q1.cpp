#include <Eigen/Dense>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

const std::vector<cv::Point3d> PW_BIG = {
    {-0.115, 0.0265, 0.},
    {-0.115, -0.0265, 0.},
    {0.115, -0.0265, 0.},
    {0.115, 0.0265, 0.}};

int main()
{
    cv::Mat camMatrix, distCoeff;
    cv::FileStorage reader("f_mat_and_c_mat.yml", cv::FileStorage::READ);
    if (!reader.isOpened())
    {
        std::cerr << "Failed to open file." << std::endl;
        return -1;
    }
    reader["F"] >> camMatrix;
    reader["C"] >> distCoeff;

    std::vector<cv::Point2f> armor_corners = {{575.508, 282.175},
                                              {573.93, 331.819},
                                              {764.518, 337.652},
                                              {765.729, 286.741}};
    cv::Mat rvec, tvec;
    cv::solvePnP(PW_BIG, armor_corners, camMatrix, distCoeff, rvec, tvec);
    cv::Rodrigues(rvec, rMatrix);
    Eigen::Matrix4d R = Eigen::Map<Eigen::Matrix4d>(rMatrix.ptr<double>(), rMatrix.rows * rMatrix.cols);
    Eigen::Vector3d T = Eigen::Map<Eigen::Vector3d>(tvec.ptr<double>(), tvec.rows);
    Eigen::Quaterniond q(-0.0816168, 0.994363, -0.0676645, -0.00122528);
    Eigen::Vector3d result = zero_vector(3);
    for (const cv::Point3d &armor_point : PW_BIG)
    {
        Eigen::Vector3d imgpoint, objpoint;
        imgpoint(0) = armor_point.x;
        imgpoint(1) = armor_point.y;
        imgpoint(2) = armor_point.z;
        objpoint = q * (R * imgpoint + T);
        result += objpoint / 4;
    }
    std::cout << result << std::endl;
    return 0;
}
