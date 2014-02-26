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
using namespace std;

class test1 {
public:
	test1(bool method1);
	virtual ~test1();
	bool is_finger_point(CvPoint * p);
	int find_index_of_closest_point_on_contour(CvPoint* p);
	CvPoint* find_Closeset_Point(CvPoint* p,int start_index,bool fwd_dir);
	void filter_hull_points(bool checkBoundary);
	bool merge_by_threshold(CvPoint*src,CvPoint* dest);
	//variables
	CvCapture* capture;
		IplImage* img;
		IplImage* gray_img;
		IplImage * gray_im; // used with ycrcb
		CvSeq* contour; //pointer to a contour.
		CvSeq* contour_seq_points; //hold seq of points of a contour.
		CvMemStorage* space ;
		CvBox2D  hand_boundary;
		CvSeq* largest_contour;
		CvSeq* hull;
		CvPoint pt0, pt;
		CvMemStorage* defects_space ;
		CvSeq* defects;
		vector<CvPoint*> contourPoints,hullPoint;
	vector<CvPoint*> filtered_hull_points;
	vector<CvPoint*> finger_points;
		bool loop ;
};

#endif /* TEST1_H_ */
