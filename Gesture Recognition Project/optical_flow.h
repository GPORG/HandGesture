/*
 * SherifCod.h
 *
 *  Created on: Jun 29, 2014
 *      Author: Mohamed
 */

#ifndef OPTICAL_FLOW_H_
#define OPTICAL_FLOW_H_

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <ctype.h>
#include <vector>
#include <math.h>
using namespace std;
using namespace cv;

class optical_flow {
public:
	optical_flow();
	virtual ~optical_flow();
	vector<Point2f> old_frame_points;
	vector<Point2f> new_frame_points;
	Mat img, new_gray_img, old_gray_img;
	bool first_frame;
	int max_count;
	int min_distance;
	int block_size;
	static const float threshold = 2.5;
	static const float thresh_in_on_dir = 11;
	int up_count, down_count, left_count, right_count, non_count;
	string final_direction;
	vector<int> directions;

	int up_direction;
	int down_direction;
	int left_direction;
	int right_direction;
	int non_direction;

	float get_distance(Point2f p1, Point2f p2);
	string get_final_direction(Mat m1, Mat m2, Rect old_hand_boundary,
			Rect new_hand_boundary);
};

#endif /* SHERIFCOD_H_ */
