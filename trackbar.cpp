
/*Created by Alberto Chimenti on 26th April 2021*/

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "trackbar.h"

Trackbar::Trackbar() {
}

// create custom multipliers (mult1, mult2) for adaptive use
void Trackbar::on_trackbar1(int pos, void* ptr) {
	Trackbar* obj = reinterpret_cast<Trackbar*> (ptr); // Use Polymorphism of subclasses
	// Recast integer slider to double parameter
	obj->setParam1(static_cast<double>(obj->slider1) / static_cast<double>(obj->mult1));
	// Perform detection exploiting virtual function override!
	obj->doTask();
}
void Trackbar::on_trackbar2(int pos, void* ptr) {
	Trackbar* obj = reinterpret_cast<Trackbar*> (ptr);
	obj->setParam2(static_cast<double>(obj->slider2) / static_cast<double>(obj->mult2));
	obj->doTask();
}

void Trackbar::on_trackbar_int(int pos, void* ptr) {
	Trackbar* obj = reinterpret_cast<Trackbar*> (ptr);
	obj->doTask();
}

void Trackbar::setParam1(double p) { param1 = p; };
void Trackbar::setParam2(double p) { param2 = p; };
void Trackbar::setParam_int(int p) { param_int = p; };
void Trackbar::destroyWindow() { cv::destroyWindow(window_name); };
void Trackbar::doTask() {};

/*/////////////////////////////////////////////////////////////*/			

CannyTrackbar::CannyTrackbar(cv::Mat src, double t1, double t2, std::string w_n) : Trackbar(){
	
	input_image = src;
	result_image = input_image.clone();
	window_name = w_n;
	param1 = t1;
	param2 = t2;
	// init sliders multipliers
	mult1 = 10;
	mult2 = 10;
	// init sliders
	slider1 = (int)t1 * mult1;
	slider2 = (int)t2 * mult2;
	// Create window
	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
	doTask(); //Optional
	cv::imshow(window_name, result_image);
	cv::createTrackbar("thr1*10", window_name, &slider1, 10000, on_trackbar1, static_cast<void*>(this));
	cv::createTrackbar("thr2*10", window_name, &slider2, 10000, on_trackbar2, static_cast<void*>(this));

}

void CannyTrackbar::doTask() {
	//std::cout << param1 << ", " << param2 << std::endl;
	cv::Canny(input_image, result_image, param1, param2);
	cv::imshow(window_name, result_image);
}

cv::Mat CannyTrackbar::getResult() { return result_image; }

/*////////////////////////////////////////////////////////////*/

HoughCTrackbar::HoughCTrackbar(cv::Mat src, cv::Mat gray, double cannythr, double n_pts, std::string w_n) : Trackbar() {

	original = src;
	input_image = gray;
	result_image = original.clone();
	window_name = w_n;
	param1 = cannythr;
	param2 = n_pts;
	param_int = 0;
	// init sliders multipliers
	mult1 = 10;
	mult2 = 1;
	// init sliders
	slider1 = (int)param1 * mult1;
	slider2 = (int)param2 * mult2;
}

cv::Mat HoughCTrackbar::getResult() { return result_image; }

std::vector<cv::Vec3f> HoughCTrackbar::getCircles() { return circles; }

void HoughCTrackbar::startDetection() {

	// Create window
	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
	doTask(); //Optional
	cv::imshow(window_name, result_image); // Show window
	// Initialize trackbars
	cv::createTrackbar("Cnythr*10", window_name, &slider1, 9000, on_trackbar1, static_cast<void*>(this));
	cv::setTrackbarMin("Cnythr*10", window_name, 1 * mult1);
	cv::createTrackbar("Acc_Thr", window_name, &slider2, 200, on_trackbar2, static_cast<void*>(this));
	cv::setTrackbarMin("Acc_Thr", window_name, 1 * mult2);
	cv::createTrackbar("max_radius", window_name, &param_int, 200, on_trackbar_int, static_cast<void*>(this));
}

void HoughCTrackbar::draw_circles() {

	result_image = original.clone(); // Re-initialize image
	// Draw circles over clean image
	if (!circles.empty()) {
		std::cout << "\n\nCircles:\n" << std::endl;
		for (size_t i = 0; i < circles.size(); i++)
		{
			cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			std::cout << circles[i] << std::endl; // Print circle vector
			int radius = cvRound(circles[i][2]);
			// draw the circle center
			circle(result_image, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			// draw the circle outline
			circle(result_image, center, radius, cv::Scalar(0, 255, 0), 3, 8, 0);
		}
	}
}

void HoughCTrackbar::doTask() {

	circles.clear(); // Clear the previously found circles if present
	cv::HoughCircles(input_image, circles, /*method*/ cv::HOUGH_GRADIENT, /*dp*/ 1.4, 
		/*minDist*/ input_image.rows/6, /*Canny*/ param1, /*Acc_thr*/ param2, /*Minrad*/ 0, /*Maxrad*/ param_int);
	draw_circles();
	cv::imshow(window_name, result_image);
}



/*////////////////////////////////////////////////////////////*/

HoughLTrackbar::HoughLTrackbar(cv::Mat src, cv::Mat edge_map, double rho, double theta, int thr, std::string w_n) : Trackbar() {

	input_image = src;
	edges = edge_map;
	cv::Mat mask = cv::Mat::zeros(src.size(), CV_8UC1); // init mask
	result_image = input_image.clone();
	window_name = w_n;
	param1 = rho;
	param2 = theta;
	param_int = thr;
	// init sliders multipliers
	mult1 = 1;
	mult2 = 180 / CV_PI;
	// init sliders
	slider1 = (int)rho * mult1;
	slider2 = (int)theta * mult2 + 1; // radian units

}

cv::Mat HoughLTrackbar::getResult() { return result_image; }

std::vector<cv::Vec2f> HoughLTrackbar::getLines() { return lines; }

cv::Mat HoughLTrackbar::getMask() { return mask; }

void HoughLTrackbar::setMask(cv::Mat m) { mask = m; }

void HoughLTrackbar::applyMask() {

	cv::Mat temp; // Temporary image
	edges.copyTo(temp, mask);
	edges = temp.clone();
}

void HoughLTrackbar::setTrapezoidalMask() {

	// Image size
	cv::Size size = edges.size();
	// Create edge points for the triangle
	std::vector<cv::Point> points;
	points.push_back(cv::Point(0, size.height - 1)); //left corner
	//points.push_back(cv::Point(((int)size.width/3), size.height - 1)); //left corner
	points.push_back(cv::Point(0, size.height - ((int)size.height / 3)));
	points.push_back(cv::Point(((int)size.width / 2), ((int)size.height / 2))); //center
	points.push_back(cv::Point(size.width - 1, size.height - ((int)size.height / 3)));
	points.push_back(cv::Point(size.width - 1, size.height - 1)); //right corner

	// Re initialize mask
	mask = cv::Mat::zeros(size, CV_8UC1);
	// Fill polygon white
	cv::fillConvexPoly(mask, points, cv::Scalar(255, 8, 0));

}


void HoughLTrackbar::startDetection() {
	// Create window
	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
	doTask(); //Optional
	cv::imshow(window_name, result_image); // Show window
	cv::createTrackbar("rho", window_name, &slider1, 10, on_trackbar1, static_cast<void*>(this));
	cv::setTrackbarMin("rho", window_name, 1 * mult1); // set minimum 1 pixel
	cv::createTrackbar("thta (rad)", window_name, &slider2, 10, on_trackbar2, static_cast<void*>(this));
	cv::setTrackbarMin("thta (rad)", window_name, 1); // set minimum 1 rad
	cv::createTrackbar("min#points", window_name, &param_int, 250, on_trackbar_int, static_cast<void*>(this));
}


void HoughLTrackbar::filter_lines() {
	if (!lines.empty()) {
		std::vector<cv::Vec2f> n_lines;
		for (size_t i = 0; i < lines.size(); i++) {
			// Filter the lines with slope close to zero (horizontal)
			if (cos(lines[i][1]) / sin(lines[i][1]) < - 0.15 | cos(lines[i][1]) / sin(lines[i][1]) > 0.15) {
				n_lines.push_back(lines[i]); // Append vector
			}
		}
		lines = n_lines;
	}
}

void HoughLTrackbar::draw_lines() {
	// Re-initialize image
	result_image = input_image.clone();
	cv::Mat temp = input_image.clone();
	// Draw the lines
	if (!lines.empty()) {
		std::cout << "\n\nLines:\n" << std::endl;
		for (size_t i = 0; i < lines.size(); i++)
		{
			std::cout << lines[i] << std::endl;
			float rho = lines[i][0], theta = lines[i][1];
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a * rho, y0 = b * rho;
			pt1.x = cvRound(x0 + 2000 * (-b));
			pt1.y = cvRound(y0 + 2000 * (a));
			pt2.x = cvRound(x0 - 2000 * (-b));
			pt2.y = cvRound(y0 - 2000 * (a));
			cv::line(temp, pt1, pt2, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
		}
		// Mask part of the line for cleaner visualization
		cv::Mat rect = cv::Mat::zeros(edges.size(), CV_8UC1); 
		// define rectangular region of interest for bottom side of image
		cv::rectangle(rect, cv::Point(0, ((int)edges.rows/2) + ((int)edges.rows / 9)), 
			cv::Point(edges.cols-1, edges.rows-1), cv::Scalar(255, 255, 255), cv::FILLED);
		// Filter resulting lines
		temp.copyTo(result_image, rect);
	}
}


void HoughLTrackbar::doTask() {
	
	//setTrapezoidalMask();
	applyMask();
	std::fill(lines.begin(), lines.end(), 0);
	cv::HoughLines(edges, lines, param1, param2, param_int);
	filter_lines();
	cv::imshow("Mask", mask);
	cv::imshow("Edgemap", edges);
	draw_lines();
	cv::imshow(window_name, result_image);
}