#include <iostream>
#include <time.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>

using std::cout;
using std::endl;
using std::vector;
using cv::Mat;
using cv::Point;
using cv::KeyPoint;
using cv::Scalar;

void corner_fast();

int main()
{
	corner_fast();
	return 0;
}

void corner_fast()
{
	clock_t start_time, end_time;
	double elapsed_time;

	start_time = clock();
	Mat src = cv::imread("building.jpg", cv::IMREAD_GRAYSCALE);	// FAST() should get grayscale source image so that you should convert source image to grayscale

	if (src.empty())
	{
		std::cerr << "Image load failed" << std::endl;
		return;
	}

	vector<KeyPoint> keypoints;
	FAST(src, keypoints, 60, true);								// operate FAST method to detect keypoints with non-maximum suppression, in which threshold value is 60 

	Mat dst;
	cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);

	for (KeyPoint kp : keypoints)								// draw circles on each corners
	{
		Point pt(cvRound(kp.pt.x), cvRound(kp.pt.y));
		circle(dst, pt, 5, Scalar(0, 0, 255), 2);

	}

	cv::imshow("src", src);
	cv::imshow("dst", dst);

	end_time = clock();
	elapsed_time = static_cast<double>(end_time - start_time);
	
	cout << elapsed_time / CLOCKS_PER_SEC << "ÃÊ" << endl;
	cv::waitKey();
	cv::destroyAllWindows();

}