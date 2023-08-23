#include <filesystem>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "matplotlibcpp.h"
#include <map>
#include <string>
#include <vector>
#define SQUARE(x) ((x)*(x))

namespace fs = std::filesystem;
namespace plt = matplotlibcpp;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::tuple;
using std::string;
using std::ifstream;
using std::filesystem::path;
using cv::Mat;

//void detect_keypoint();
//void keypoint_matching();
//void good_matching();
//
//class Circle 
//{
//public:
//	int radius;
//	Circle();		// without parameter
//	Circle(int r);	// with radius
//	double getArea();
//};
//
//Circle::Circle()
//{
//	//radius = 1;
//	std::cout << "a circle created" << std::endl;
//}
//
//Circle::Circle(int r)
//{
//	radius = r;
//	std::cout << "Created a circle with radius : " << radius << std::endl;
//}
//double Circle::getArea() 
//{
//	return 3.14 * radius * radius;
//
//}
//
//class Rectangle {
//public:
//	int width;
//	int height;
//	int getArea();
//};
//
//int Rectangle::getArea()
//{
//	return width * height;
//}
//
//class Test
//{
//public:
//	path data_dir;
//	Test(path dir) : data_dir(dir) {}
//	path load_calib(path dir);
//	const path KP = this->load_calib(this->data_dir);
//};
//
//path Test::load_calib(path dir)
//{
//	cout << "dd" << dir << endl;
//	return dir;
//}

int main()
{
	////1. import os -> #include <filesystem>
	//fs::path dir("./KITTI_sequence_1/image_l");
	//
	//// append path
	//fs::path file = "000000.png";
	//fs::path full_path = dir / file;

	//std::cout << full_path << std::endl;
	//
	//// imshow image after convert full_path to native string data type
	//cv::Mat img = cv::imread(full_path.u8string());
	//cv::imshow("Image", img);
	//cv::waitKey();
	//cv::destroyAllWindows();
	
	// separate string with ' '
	/*path directory = "./KITTI_sequence_1";
	path file_name = "calib.txt";
	path full_path = directory / file_name;
	vector<string> lines;
	string line;
	
	ifstream input_file(string(full_path));
	cout << input_file << endl;*/
	//if (input_file.is_open())
	//{
	//	while (!input_file.eof())
	//	{
	//		string str;
	//		std::getline(input_file, str);
	//		cout << str << endl;
	//	}

	//	input_file.close();
	//}
	

	// 2. class

	/*Rectangle rect;
	Circle circle1;
	Circle circle2(4);

	rect.width = 3;
	rect.height = 5;
	std::cout << rect.getArea() << std::endl;

	std::cout << circle2.getArea() << std::endl;*/

	/*fs::path dir = "./KITTI_sequence_1/image_l";
	fs::path filename = "000000.png";
	
	Test test(dir / filename);
	
	cout << test.data_dir << endl;*/

	/*cv::Mat matrix(1,3,CV_8UC1);
	std::cout << matrix << std::endl;

    vector<unsigned char> line(3,1);

    cv::Mat line_m(line,true);

	std::cout << line_m << std::endl;
    line_m=line_m.t();
	std::cout << line_m << std::endl;
    matrix.push_back(line_m);*/
	
	
	// 3. feature detection
	//detect_keypoint();

	// 4. feature matching
	//keypoint_matching();
 
	// 5. sort good matching
	//good_matching();

	/*cv::Mat img = cv::imread("box.png");
	
	cv::imshow("Image", img);
	cv::waitKey();
	cv::destroyAllWindows();*/

	const double pi = 3.14;
	std::vector<double> x(21);
	std::vector<double> y(21);
	for (int i = 1; i < x.capacity(); ++i) {
		x[i] = x[i - 1] + pi / 10;
		y[i] = sin(x[i]);
	}
	plt::plot(x, y); //plot the x,y
	plt::grid(true); //show grid
	plt::show(); // show figure

	return 0;
}

//void detect_keypoint()
//{
//	cv::Mat src = cv::imread("box_in_scene.png", cv::IMREAD_GRAYSCALE);
//
//	if (src.empty())
//	{
//		std::cerr << "Image load failed!" << std::endl;
//		return;
//	}
//
//	cv::Ptr<cv::Feature2D> feature = cv::ORB::create();	// Create ORB object and store into smart pointer 'feature'
//
//
//	// the code below is equivalent to following code
//	// feature->detectAndCompute(src, Mat(), keypoints, desc);
//
//	std::vector<cv::KeyPoint> keypoints;				// detect feature and then store in 'keypoints' vector
//	feature->detect(src, keypoints);
//
//	cv::Mat desc;										// compute descriptor and then store into desc matrix
//	feature->compute(src, keypoints, desc);
//
//
//
//	std::cout << "keypoints.size() : " << keypoints.size() << std::endl;	// print the number of feature and a size of desriptor
//	std::cout << "desc.size() : " << desc.size() << std::endl;
//
//	cv::Mat dst;															// store the result into dst after drawing keypoint on src
//	drawKeypoints(src, keypoints, dst, cv::Scalar::all(-1),
//		cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);							// The size of circle indicate the size of area considered when feature detection operated
//																			// The line from each center of the circle indicates dominant direction around feature point
//	cv::imshow("src", src);
//	cv::imshow("dst", dst);
//
//	cv::waitKey();
//	cv::destroyAllWindows();
//
//}
//
//void keypoint_matching()
//{
//	cv::Mat src1 = cv::imread("box.png");
//	cv::Mat src2 = cv::imread("box_in_scene.png");
//
//	if (src1.empty() || src2.empty())
//	{
//		std::cerr << "Image load failed!" << std::endl;
//		return;
//	}
//
//	cv::Ptr<cv::Feature2D> feature = cv::ORB::create();
//
//	std::vector<cv::KeyPoint> keypoints1, keypoints2;
//	cv::Mat desc1, desc2;
//
//	feature->detectAndCompute(src1, cv::Mat(), keypoints1, desc1);
//	feature->detectAndCompute(src2, cv::Mat(), keypoints2, desc2);
//
//	cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING);
//
//	std::vector<cv::DMatch> matches;
//	matcher->match(desc1, desc2, matches);
//
//	cv::Mat dst;
//	cv::drawMatches(src1, keypoints1, src2, keypoints2, matches, dst);
//
//	cv::imshow("dst", dst);
//
//	cv::waitKey();
//	cv::destroyAllWindows();
//}
//
//void good_matching()
//{
//	cv::Mat src1 = cv::imread("box.png", cv::IMREAD_GRAYSCALE);
//	cv::Mat src2 = cv::imread("box_in_scene.png", cv::IMREAD_GRAYSCALE);
//
//	if (src1.empty() || src2.empty())
//	{
//		std::cerr << "Image load failed!" << std::endl;
//		return;
//	}
//
//	cv::Ptr<cv::Feature2D> feature = cv::ORB::create();
//
//	std::vector<cv::KeyPoint> keypoints1, keypoints2;
//	cv::Mat desc1, desc2;
//
//	feature->detectAndCompute(src1, cv::Mat(), keypoints1, desc1);
//	feature->detectAndCompute(src2, cv::Mat(), keypoints2, desc2);
//
//	cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING);
//
//	std::vector<cv::DMatch> matches;
//	matcher->match(desc1, desc2, matches);
//
//	std::sort(matches.begin(), matches.end());
//	std::vector<cv::DMatch> good_matches(matches.begin(), matches.begin() + 50);
//
//	cv::Mat dst;
//	cv::drawMatches(src1, keypoints1, src2, keypoints2, good_matches, dst,
//		cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(),
//		cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
//
//	cv::imshow("dst", dst);
//
//	cv::waitKey();
//	cv::destroyAllWindows();
//
//}