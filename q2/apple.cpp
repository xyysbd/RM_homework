#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
    const int board_c = 9, board_r = 6;
    const int board_n = board_c * board_r;
    Size board_size(9, 6);
    Mat gray_img, drawn_img;
    vector<Point2f> point_pix_pos_buf;
    vector<vector<Point2f>> point_pix_pos;
    int found, successes = 0;
    Size img_size;
    int cnt = 0;
    int m = 0, n = 0;
    String str;
    for (int i = 0; i < 40; i++)
    {
        str = to_string(i);
        Mat src0 = imread("chess/" + str + ".jpg");
        if (!cnt)
        {
            img_size.width = src0.cols;
            img_size.height = src0.rows;
        }
        found = findChessboardCorners(src0, board_size, point_pix_pos_buf);
        if (found && point_pix_pos_buf.size() == board_n)
        {
            successes++;
            cvtColor(src0, gray_img, COLOR_BGR2GRAY);
            find4QuadCornerSubpix(gray_img, point_pix_pos_buf, Size(5, 5));
            point_pix_pos.push_back(point_pix_pos_buf);
            drawn_img = src0.clone();
            drawChessboardCorners(drawn_img, board_size, point_pix_pos_buf, found);
            imshow("corners", drawn_img);
            waitKey(50);

            // 保存图片到 result_calibrate 文件夹
            string filename = "result_calibrate/" + to_string(successes) + ".jpg";
            imwrite(filename, drawn_img);
        }
        else
        {
            cout << "Image " << i << " passed, but failed to find all chess board corners in this image" << endl;
        }
        point_pix_pos_buf.clear();
        cnt++;
    }
    cout << successes << " useful chess boards" << endl;
    Size square_size(10, 10);
    vector<vector<Point3f>> point_grid_pos;
    vector<Point3f> point_grid_pos_buf;
    vector<int> point_count;
    Mat camera_matrix(3, 3, CV_32FC1, Scalar::all(0));
    Mat dist_coeffs(1, 5, CV_32FC1, Scalar::all(0));
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    for (int i = 0; i < successes; i++)
    {
        for (int j = 0; j < board_r; j++)
        {
            for (m = 0; m < board_c; m++)
            {
                Point3f pt;
                pt.x = m * square_size.width;
                pt.y = j * square_size.height;
                pt.z = 0;
                point_grid_pos_buf.push_back(pt);
            }
        }
        point_grid_pos.push_back(point_grid_pos_buf);
        point_grid_pos_buf.clear();
        point_count.push_back(board_r * board_c);
    }
    cout << "Camera calibration result: " << calibrateCamera(point_grid_pos, point_pix_pos, img_size, camera_matrix, dist_coeffs, rvecs, tvecs) << endl;
    cout << "Camera matrix:" << endl
         << camera_matrix << endl;
    cout << "Distortion coefficients:" << endl
         << dist_coeffs << endl;
    return 0;
}
