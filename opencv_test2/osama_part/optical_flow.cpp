/*
 * SherifCod.cpp
 *
 *  Created on: Jun 29, 2014
 *      Author: Mohamed
 */

#include "optical_flow.h"

optical_flow::optical_flow() {

	up_direction = 1;
	down_direction = 2;
	left_direction = 3;
	right_direction = 4;
	non_direction = 5;

	max_count = 500;
	min_distance = 10;
	block_size = 3;

}

string optical_flow::get_final_direction(Mat m1, Mat m2) {

	left_count = 0;
	up_count = 0;
	down_count = 0;
	right_count = 0;
	non_count = 0;

	TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03);

	cvtColor(m1, old_gray_img, COLOR_BGR2GRAY);//convert to gray
	cvtColor(m2, new_gray_img, COLOR_BGR2GRAY);

	// extract features
	goodFeaturesToTrack(old_gray_img, old_frame_points, max_count, .01,
			min_distance, Mat(), block_size, 0, .04);
	cornerSubPix(old_gray_img, old_frame_points, Size(10, 10), Size(-1, -1),
			termcrit);

	// track features in next frame
	vector<uchar> status;
	vector<float> err;
	calcOpticalFlowPyrLK(old_gray_img, new_gray_img, old_frame_points,
			new_frame_points, status, err, Size(10, 10), 3, termcrit, 0, 0.001);

	for (unsigned int i = 0; i < new_frame_points.size(); i++) {
		Point2f old_point = old_frame_points.at(i);
		Point2f new_point = new_frame_points.at(i);
		float dist = get_distance(old_point, new_point);
		if (dist < threshold) {
			directions.push_back(non_direction);
			non_count++;
		} else {
			float dx = new_point.x - old_point.x;
			float dy = new_point.y - old_point.y;
			if (abs(dx) > abs(dy)) {//horizontal
				if (dx < 0) {
					directions.push_back(left_direction);
					left_count++;
				} else {
					directions.push_back(right_direction);
					right_count++;
				}
			} else { //vertical
				if (dy > 0) {
					directions.push_back(up_direction);
					up_count++;
				} else {
					directions.push_back(down_direction);
					down_count++;
				}
			}
		}
	}

	int dirs_counts[] = { up_count, down_count, left_count, right_count,
			non_count };
	int max_elem = *max_element(dirs_counts, dirs_counts + 5);
	final_direction = "";
	if (up_count == max_elem)
		final_direction = "up";
	else if (down_count == max_elem)
		final_direction = "down";
	else if (left_count == max_elem)
		final_direction = "left";
	else if (right_count == max_elem)
		final_direction = "right";
	else
		final_direction = "none";

	return final_direction;
}

float optical_flow::get_distance(Point2f p1, Point2f p2) {

	return sqrtf(powf((p1.x - p2.x), 2) + powf((p1.y - p2.y), 2));

}
optical_flow::~optical_flow() {
	// TODO Auto-generated destructor stub
}
