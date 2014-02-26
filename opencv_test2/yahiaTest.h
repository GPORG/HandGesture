/*
 * yahiaTest.h
 *
 *  Created on: Feb 19, 2014
 *      Author: Mohamed
 */

#ifndef YAHIATEST_H_
#define YAHIATEST_H_


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include "opencv2/nonfree/features2d.hpp"
#include <opencv2/legacy/compat.hpp>
#include <opencv2/video/background_segm.hpp>
#include "opencv2/video/background_segm.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
using namespace std;

class yahiaTest {
public:
	yahiaTest();
	virtual ~yahiaTest();

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
	bool loop;
};

#endif /* YAHIATEST_H_ */
