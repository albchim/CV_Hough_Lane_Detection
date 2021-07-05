/*Created by Alberto Chimenti on 26th April 2021*/


// include openCV and standard headers
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "trackbar.h"


int main(int argc, char** argv) {
	// read image into cv::Mat input_img and visualize
	const char* filename = argc >= 2 ? argv[1] : "imaages/road8.jpg";

	cv::Mat src = cv::imread(cv::samples::findFile(filename));
	//if fail to read the image
	if (src.empty()) { std::cout << "Error loading the image \n" << std::endl; return EXIT_FAILURE; };
	// resize image 
	double ratio = 720 / static_cast<double>(src.cols); // ratio is needed to always get the same output size for the image
	std::cout << src.cols << " , " << src.rows << " ; " << ratio << std::endl;
	cv::resize(src, src, cv::Size(), ratio, ratio);

	// convert to gray image
	cv::Mat src_gray;
	cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);
	// Gaussian Blur 
	cv::GaussianBlur(src_gray, src_gray, cv::Size(5, 5), 3, 3);
	cv::imshow("Gray G-Blurred", src_gray);
	cv::waitKey(0); cv::destroyWindow("Gray G-Blurred");

	HoughCTrackbar circ(src, src_gray);
	circ.startDetection();
	cv::waitKey(0); circ.destroyWindow();

	CannyTrackbar edges(src_gray);
	cv::waitKey(0); edges.destroyWindow();

	HoughLTrackbar lines(circ.getResult() , edges.getResult());
	lines.setTrapezoidalMask();
	lines.startDetection();
	
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;

}