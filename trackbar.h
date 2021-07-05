#pragma once

class Trackbar {

public:
	// Use standard constructor
	Trackbar();

	// Static trackbar callback function
	static void on_trackbar1(int, void*);
	static void on_trackbar2(int, void*);
	static void on_trackbar_int(int, void*);

	// Mock virtual functions
	void setParam1(double p);
	void setParam2(double p);
	void setParam_int(int p);
	void destroyWindow();
	virtual void doTask();

protected:
	double param1, param2;
	double mult1, mult2; // multipliers
	int slider1, slider2; // trackbar sliders
	int param_int; 
	std::string window_name; // window name
};

class CannyTrackbar : public Trackbar {

public:
	
	// constructor
	CannyTrackbar(cv::Mat src, double t1 = 200, double t2 = 250, std::string w_n = "Canny Edges");
	// override virtual function
	void doTask();
	// output object method
	cv::Mat getResult();


protected:
	cv::Mat input_image, result_image;
};

class HoughCTrackbar : public Trackbar {

public:
	HoughCTrackbar(cv::Mat src, cv::Mat gray, double cannythr = 150, double n_pts = 100, std::string w_n = "Hough Circles");

	cv::Mat getResult();

	std::vector<cv::Vec3f> getCircles();

	void startDetection();

	void draw_circles();

	void doTask();

protected:
	cv::Mat input_image, original, result_image;
	std::vector<cv::Vec3f> circles;

};

class HoughLTrackbar : public Trackbar {

public:

	HoughLTrackbar(cv::Mat src, cv::Mat edge_map, double rho = 2, double theta = CV_PI / 180, int thr = 150, std::string w_n = "Hough Lines");

	cv::Mat getResult();

	std::vector<cv::Vec2f> getLines();

	cv::Mat getMask();

	void setMask(cv::Mat m);

	void applyMask();

	void setTrapezoidalMask();

	void startDetection();

	void filter_lines();

	void draw_lines();

	void doTask();

protected:
	cv::Mat input_image, edges, mask, result_image;
	std::vector<cv::Vec2f> lines;
};
