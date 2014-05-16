/*
 * detection2.h
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#ifndef detection2_H_
#define detection2_H_

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
#define PI 3.14159265
class detection2 {
public:
	detection2(bool test_mood);
	virtual ~detection2();
	//variables
	CvCapture* capture;
	IplImage* img;
	IplImage* gray_img;
	IplImage * gray_im; // used with ycrcb
	CvSeq* contour; //pointer to a contour.
	CvMemStorage* space;
	CvBox2D hand_boundary;
	CvSeq* largest_contour;
	CvSeq* hull;
	CvPoint pt0, pt;
	bool loop;
	CvFont f;//for text on image
	IplImage* hand_gray;
	IplImage* binary_hand;
	My_SVM s;
	float mean, variance, area, permiter;
	int num_of_defects;
	CvMoments* moments;
	float xc, yc, angle_theta;
	static const int num_of_features = 141;
	float features[num_of_features];
	float kernal_data[5];
	Mat kernal;
	void Orientation_Histo();
	float label_gesture();
};

#endif /* detection2_H_ */
