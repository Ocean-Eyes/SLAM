#include <iostream>
#include <time.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using cv::Mat;

void corner_harris();

int main()
{
	corner_harris();
	return 0;
}

void corner_harris()
{
	clock_t start_time = clock();
	Mat src = cv::imread("building.jpg", cv::IMREAD_GRAYSCALE);	// get source image with grayscale

	if (src.empty())											// print error message if the image is not available
	{
		std::cerr << "Image load failed" << std::endl;
		return;
	}

	Mat harris;
	cv::cornerHarris(src, harris, 3, 3, 0.04);									// source img, destination mat, blockSize(for getting matrix M), ksize(sobel operator), k(coefficient), borderType(default value is BORDER_DEFAULT)
	
	Mat harris_norm;															// for visualize harris corner response function R.
	cv::normalize(harris, harris_norm, 0, 255, cv::NORM_MINMAX, CV_8UC1);		// normalize harris matrix to a range from 0 to 255 and convert to CV_8UC1 and store it in harris_norm

	Mat dst;																	// define destination image
	cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);

	for (int j = 1; j < harris.rows - 1; j++)									// determine whether it's corner or not
	{
		for (int i = 1; i < harris.cols - 1; i++)
		{
			if (harris_norm.at<uchar>(j, i) > 120)								// if each pixel values of harris_norm are more than 120, those pixel are classified as corners
			{
				if (harris.at<float>(j, i) > harris.at<float>(j - 1, i) &&		// operate non-maximum suppression to classify as a corner only if its pixel value is maximum around local area
					harris.at<float>(j, i) > harris.at<float>(j + 1, i) &&
					harris.at<float>(j, i) > harris.at<float>(j, i - 1) &&
					harris.at<float>(j, i) > harris.at<float>(j, i + 1)
					)
				{
					circle(dst, cv::Point(i, j), 5, cv::Scalar(0, 0, 255), 2);	// visualize harris corner point with circle after non-maximum suppression
				}
			}
		}
	}
	cv::imshow("src", src);
	cv::imshow("harris_norm", harris_norm);										// gray dots indicate harris corner points
	cv::imshow("dst", dst);														

	clock_t end_time = clock();
	double elapsed_time = double(end_time - start_time);
	std::cout << elapsed_time / CLOCKS_PER_SEC << "ÃÊ" << std::endl;

	cv::waitKey();
	cv::destroyAllWindows();
}