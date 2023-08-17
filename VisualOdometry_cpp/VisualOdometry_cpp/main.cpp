#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include <filesystem>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>

namespace fs = std::filesystem;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using cv::Mat;

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
		images.push_back(cv::imread(entry.path().u8string(), cv::IMREAD_GRAYSCALE));
	}

	return images;
}


std::pair<Mat, Mat> VisualOdometry::get_matches(int i)
{
	Mat q1(4, 4, CV_64FC1);
	Mat q2(4, 4, CV_64FC1);
	Mat desc1;
	Mat desc2;

	std::vector<cv::KeyPoint> keypoints1, keypoints2;

	orb->detectAndCompute(images[i-1], Mat(), keypoints1, desc1);
	orb->detectAndCompute(images[i], Mat(), keypoints2, desc2);
	
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::FlannBasedMatcher::create();
	vector<cv::DMatch> matches;
	matcher->match(desc1, desc2, matches);

	std::sort(matches.begin(), matches.end());
	vector<cv::DMatch> good_matches(matches.begin(), matches.begin() + 50);

	cv::Mat dst;
	cv::drawMatches(images[i - 1], keypoints1, images[i], keypoints2, good_matches, dst,
		cv::Scalar::all(-1), cv::Scalar::all(-1), vector<char>(),
		cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	for (cv::DMatch match : good_matches)
	{
		q1.push_back(keypoints1[match.queryIdx].pt);
		q2.push_back(keypoints2[match.trainIdx].pt);
	}

	return std::make_pair(q1, q2);
}


int main()
{
	fs::path data_dir = "C:\\github_repo\\SLAM\\VisualOdometry_cpp\\VisualOdometry_cpp\\KITTI_sequence_1";
	VisualOdometry vo(data_dir);

	Mat cur_pose;
	Mat q1, q2;
	vector<std::tuple<uchar, uchar>> gt_path, estimated_path;	

	for (int i = 0; i < vo.gt_poses.size(); i++)
	{
		if (i == 0)
		{
			cur_pose = vo.gt_poses[0];
		}
		else
		{
			q1 = vo.get_matches(i).first;
			q2 = vo.get_matches(i).second;
			Mat transf = vo.get_pose(q1, q2);
			cur_pose = cur_pose * transf.inv();
		}
		std::tuple cur_gt_path = std::make_tuple(vo.gt_poses[i].at<uchar>(0, 3), vo.gt_poses[i].at<uchar>(2, 3));
		std::tuple cur_estimated_path = std::make_tuple(cur_pose.at<uchar>(0, 3), cur_pose.at<uchar>(2, 3));
		
		gt_path.push_back(cur_gt_path);
		estimated_path.push_back(cur_estimated_path);
	}

	return 0;
}