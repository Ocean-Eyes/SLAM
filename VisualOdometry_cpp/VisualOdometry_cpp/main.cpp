#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/plot.hpp>
#include "matplotlibcpp.h"

namespace fs = std::filesystem;
namespace plt = matplotlibcpp;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using cv::Mat;

void plot_path(vector<std::tuple<double, double>> gt_path, vector<std::tuple<double, double>> estimated_path);


class VisualOdometry 
{
	
public:
	// about constructor
	fs::path dir;
	VisualOdometry(fs::path dir) : dir(dir) {}
	
	// Load Calibration value
	std::pair<Mat, Mat> load_calib(fs::path dir);
	std::pair<Mat, Mat> KP = this->load_calib(this->dir);
	Mat K = KP.first;
	Mat P = KP.second;

	// Load Ground Truth poses
	vector<Mat> load_poses(fs::path dir);
	vector<Mat> gt_poses = this->load_poses(this->dir);

	// Load Images
	vector<Mat> load_images(fs::path dir);
	vector<Mat> images = this->load_images(this->dir);

	// Create ORB detector and Flann based Matcher object
	cv::Ptr<cv::Feature2D> orb = cv::ORB::create();
	cv::Ptr<cv::DescriptorMatcher> flann = cv::FlannBasedMatcher::create();

	// Declare get_matches()
	std::pair<Mat, Mat> get_matches(int i);

	// Declare get_pose()
	Mat get_pose(Mat q1, Mat q2);
	int sum_z(Mat R, Mat t, Mat q1, Mat q2);
	Mat _form_transf(Mat R, Mat t);
	std::pair<Mat, Mat> decomp_essential_mat(Mat E, Mat q1, Mat q2);
};

std::pair<Mat, Mat> VisualOdometry::load_calib(fs::path dir)
{
	fs::path file_name = "calib.txt";
	string first_line;
	Mat params;
	Mat K, P;


	ifstream input_file(dir / file_name);
	if (!input_file.is_open())
	{
		cerr << "Couldn't open the file - " << file_name << endl;
	}

	getline(input_file, first_line, '\n');

	char* first_line_ptr = const_cast<char*>(first_line.c_str());
	char* token = NULL;
	char* context = NULL;
	
	token = strtok_s(first_line_ptr, " ", &context);
	
	while (token != NULL)
	{
		string str(token);
		params.push_back(std::stod(token));
		token = strtok_s(NULL, " ", &context);
	}

	input_file.close();

	P = params.reshape(0, 3); // channel(0 means remain the number of channel), rows
	
	cv::Rect rect(0, 0, 3, 3);
	K = P(rect);

	return std::make_pair(K, P);
}

vector<Mat> VisualOdometry::load_poses(fs::path dir)
{
	fs::path file_name = "poses.txt";
	string line;
	Mat value;
	Mat gt_pose;
	vector<Mat> gt_poses;

	ifstream input_file(dir / file_name);

	if (!input_file.is_open())
	{
		cerr << "Couldn't open the file - " << file_name << endl;
	}

	while (getline(input_file, line, '\n'))
	{
		char* line_ptr = const_cast<char*>(line.c_str());
		char* token = NULL;
		char* context = NULL;

		token = strtok_s(line_ptr, " ", &context);

		value.release();

		while (token != NULL)
		{
			string str(token);
			value.push_back(std::stod(token));
			token = strtok_s(NULL, " ", &context);
		}
		
		double data[4] = { 0, 0, 0, 1 };
		Mat hom_row = Mat(4, 1, CV_64F, data);

		value.push_back(hom_row);

		gt_pose = value.reshape(0, 4);

		gt_poses.push_back(gt_pose);
	}

	return gt_poses;
}

vector<Mat> VisualOdometry::load_images(fs::path dir)
{
	vector<Mat> images;


	for (const fs::directory_entry& entry : fs::directory_iterator(dir / "image_l"))
	{
		images.push_back(cv::imread(entry.path().u8string()));
	}

	return images;
}


std::pair<Mat, Mat> VisualOdometry::get_matches(int i)
{
	Mat q1;
	Mat q2;
	Mat desc1;
	Mat desc2;

	std::vector<cv::KeyPoint> keypoints1, keypoints2;

	orb->detectAndCompute(images[i-1], Mat(), keypoints1, desc1);
	orb->detectAndCompute(images[i], Mat(), keypoints2, desc2);
	
	desc1.convertTo(desc1, CV_32F);
	desc2.convertTo(desc2, CV_32F);

	cv::Ptr<cv::DescriptorMatcher> matcher = cv::FlannBasedMatcher::create();
	vector<cv::DMatch> matches;
	matcher->match(desc1, desc2, matches);

	std::sort(matches.begin(), matches.end());
	vector<cv::DMatch> good_matches(matches.begin(), matches.begin() + 100);

	cv::Mat dst;

	cv::drawMatches(images[i-1], keypoints1, images[i], keypoints2, good_matches, dst,
		cv::Scalar::all(-1), cv::Scalar::all(-1), vector<char>(),
		cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	cv::imshow("image", dst);
	cv::waitKey(10);

	for (cv::DMatch good_match : good_matches)
	{	
		cv::Point2f pt1 = keypoints1[good_match.queryIdx].pt;
		cv::Point2f pt2 = keypoints2[good_match.trainIdx].pt;
		cv::Mat ptMat1 = (cv::Mat_<double>(1, 2) << pt1.x, pt1.y);
		cv::Mat ptMat2 = (cv::Mat_<double>(1, 2) << pt2.x, pt2.y);

		q1.push_back(ptMat1);
		q2.push_back(ptMat2);
	}

	return std::make_pair(q1, q2);
}

Mat VisualOdometry::_form_transf(Mat R, Mat t)
{
	Mat T = Mat::eye(4,4, CV_64F);
	cv::Rect roi_R(0, 0, 3, 3);			// R R R t
	cv::Rect roi_t(3, 0, 1, 3);			// R R R t
										// R R R t
										// 0 0 0 1

	R.copyTo(T(roi_R));
	t.copyTo(T(roi_t));

	return T;
}


Mat VisualOdometry::get_pose(Mat q1, Mat q2)
{
	Mat transf;
	std::pair<Mat, Mat> Rt;
	Mat E, R, t;

	E = cv::findEssentialMat(q1, q2, this->K);

	Rt = decomp_essential_mat(E, q1, q2);
	R = Rt.first;
	t = Rt.second;

	transf = this->_form_transf(R, t);

	return transf;
}

int VisualOdometry::sum_z(Mat R, Mat t, Mat q1, Mat q2)
{
	Mat T = this->_form_transf(R, t);
	Mat P;
	Mat temp;
	Mat hom_Q1, hom_Q2;
	Mat uhom_Q1, uhom_Q2;
	

	cv::hconcat(this->K, Mat::zeros(3, 1, CV_64F), temp);

	P = temp * T;
	
	cv::triangulatePoints(this->P, P, q1.t(), q2.t(), hom_Q1);
	hom_Q2 = T * hom_Q1;

	// extract the last row of hom_Q1, hom_Q2 
	Mat row_last_Q1 = hom_Q1.row(3);
	Mat row_last_Q2 = hom_Q2.row(3);

	// Un-homogenize
	cv::divide(hom_Q1, cv::repeat(row_last_Q1, hom_Q1.rows, 1), uhom_Q1);
	cv::divide(hom_Q2, cv::repeat(row_last_Q2, hom_Q2.rows, 1), uhom_Q2);

	// Find the number of points that has positive z coordinate in both cameras
	int sum_of_pos_z_Q1 = cv::countNonZero(uhom_Q1.row(2) > 0);
	int sum_of_pos_z_Q2 = cv::countNonZero(uhom_Q2.row(2) > 0);

	return sum_of_pos_z_Q1 + sum_of_pos_z_Q2;

}

std::pair<Mat, Mat> VisualOdometry::decomp_essential_mat(Mat E, Mat q1, Mat q2)
{
	Mat R1, R2, t;
	Mat temp_R, temp_t;
	vector<int> z_sums;
	int z_sum, right_pair_idx;
	Mat result_R, result_t;

	cv::decomposeEssentialMat(E, R1, R2, t);
	vector<std::pair<Mat, Mat>> pairs;
	std::pair<Mat, Mat> right_pair;

	pairs.push_back(std::make_pair(R1, t));
	pairs.push_back(std::make_pair(R1, -t));
	pairs.push_back(std::make_pair(R2, t));
	pairs.push_back(std::make_pair(R2, -t));

	for (std::pair<Mat, Mat> pair : pairs)
	{
		temp_R = pair.first;
		temp_t = pair.second;
		z_sum = this->sum_z(temp_R, temp_t, q1, q2);
		z_sums.push_back(z_sum);
	}

	right_pair_idx = static_cast<int>(std::max_element(z_sums.begin(), z_sums.end()) - z_sums.begin());
	right_pair = pairs[right_pair_idx];
	result_R = right_pair.first;
	result_t = right_pair.second;

	return std::make_pair(result_R, result_t);
}


void plot_path(vector<std::tuple<double, double>> gt_path, vector<std::tuple<double, double>> estimated_path)
{
	vector<double> gt_x(gt_path.size()), gt_y(gt_path.size());
	vector<double> estimated_x(estimated_path.size()), estimated_y(estimated_path.size());

	for (size_t i = 0; i < gt_path.size(); ++i)
	{
		gt_x[i] = std::get<0>(gt_path[i]);
		gt_y[i] = std::get<1>(gt_path[i]);
		estimated_x[i] = std::get<0>(estimated_path[i]);
		estimated_y[i] = std::get<1>(estimated_path[i]);
	}

	plt::plot(gt_x, gt_y, "b");
	plt::plot(estimated_x, estimated_y, "r");
	plt::show();
}

int main()
{
	fs::path data_dir = "C:\\github_repo\\SLAM\\VisualOdometry_cpp\\VisualOdometry_cpp\\KITTI_sequence_1";
	VisualOdometry vo(data_dir);

	Mat cur_pose;
	Mat q1, q2;
	std::pair<Mat, Mat> pair;
	vector<std::tuple<double, double>> gt_path, estimated_path;

	for (int i = 0; i < vo.gt_poses.size(); i++)
	{
		if (i == 0)
		{
			cur_pose = vo.gt_poses[0];
		}
		else
		{
			pair = vo.get_matches(i);
			q1 = pair.first;
			q2 = pair.second;
			Mat transf = vo.get_pose(q1, q2);
			cur_pose = cur_pose * transf.inv();
		}
		std::tuple cur_gt_path = std::make_tuple(vo.gt_poses[i].at<double>(0, 3), vo.gt_poses[i].at<double>(2, 3));
		std::tuple cur_estimated_path = std::make_tuple(cur_pose.at<double>(0, 3), cur_pose.at<double>(2, 3));
		
		gt_path.push_back(cur_gt_path);
		estimated_path.push_back(cur_estimated_path);
	}

	plot_path(gt_path, estimated_path);

	return 0;
}