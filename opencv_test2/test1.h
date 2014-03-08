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

#include <iostream>
#include <string.h>
#include "Recognition.h"
using namespace std;

class test1 {
public:
	test1(bool method1);
	virtual ~test1();
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
	CvFont f;//for text on image
	Recognition r;

};

#endif /* TEST1_H_ */
