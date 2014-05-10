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
	CvSeq* contour_seq_points; //hold seq of points of a contour.
	CvMemStorage* space;
	CvBox2D hand_boundary;
	CvSeq* largest_contour;
	CvSeq* hull;
	CvPoint pt0, pt;
	CvMemStorage* defects_space;
	CvSeq* defects;
	vector<CvPoint*> contourPoints, hullPoint;
	vector<CvPoint*> filtered_hull_points;
	vector<CvPoint*> finger_points;
	bool loop;
	string currentGestur,prevGesture;
	bool take_actions;
	CvFont f;//for text on image
	IplImage* hand;
	My_SVM s;
	string extract_feature();
	float label_gesture();
	void apply_action(String gesture_name);

};

#endif /* TEST1_H_ */
