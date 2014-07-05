/*
 * test1.h
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#ifndef TEST1_H_
#define TEST1_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"
#include "optical_flow.h"
using namespace std;
using namespace cv;

#include <iostream>
#include <string.h>
#include "filewriter.h"
#include "My_SVM.h"
#include "windows.h"
#include "Mmsystem.h"

class detection {
public:
	detection(bool test_mood);
	virtual ~detection();
	//variables
	CvCapture* capture;
	IplImage* img;
	IplImage* gray_img;
	IplImage * gray_im; // used with ycrcb
	CvSeq* contour; //pointer to a contour.
	CvBox2D hand_boundary;
	CvSeq* largest_contour;
	CvSeq* hull;
	CvPoint pt0, pt;
	CvMemStorage* space;
	bool loop;
	string currentGestur, prevGesture;
	bool take_actions;
	CvFont f;//for text on image
	IplImage* hand;
	My_SVM s;
	static const int grid_size = 10;
	static const int number_of_features = grid_size * grid_size;
	string extract_feature();
	float label_gesture();
	void apply_action(String gesture_name, bool take_dynamic_action);

	//dynamic part vars
	optical_flow o_f;
	int target_gestures_count;
	Mat first_frame;
	Mat last_frame;
	bool take_dynamic_action;
	string dynamic_gesture_direction;
	Rect old_hand_boundary;
	Rect new_hand_boundary;
	void check_dynamic_frames(string gesture_name);
	void check_dynamic_frames2(string gesture_name);
	vector<string> stored_dirs;
	int none_dir_count;
	static const int max_non_dir = 5;//4;
	static const int wait_time = 100;//33;
	static const int take_action_interval = 10; //3;
};

#endif /* TEST1_H_ */
