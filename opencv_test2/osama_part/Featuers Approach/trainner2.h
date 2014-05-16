/*
 * trainner2.h
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#ifndef trainner2_H_
#define trainner2_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"
#include "FileReader.h"
#include "filewriter.h"
#include <iostream>
#include <string.h>
#define PI 3.14159265
using namespace std;
using namespace cv;
class trainner2 {
public:
	trainner2();
	virtual ~trainner2();
	//variables
	IplImage* img;
	IplImage* gray_img;
	IplImage * gray_im; // used with ycrcb
	CvSeq* contour; //pointer to a contour.
	CvMemStorage* space;
	CvSeq* largest_contour;
	CvSeq* hull;
	CvPoint pt0, pt;
	CvFont f;//for text on image
	IplImage * hand_gray;
	IplImage* binary_hand;
	CvBox2D hand_boundary;
	// Variables for features
	float  permiter;
	float mean, variance;
	CvMoments* moments;
	float xc,yc,angle_theta;
	Mat mag;
	static const int num_of_features=141; /*113-41*/
	static const int dataset_size = 73; /* for train 73 for test 109 */
	float features[num_of_features];
	float data[dataset_size][num_of_features];
	float labels[dataset_size];
	float kernal_data[5];
	Mat kernal;
	void Orientation_Histo(Mat gray_img,int row);
	void radial_signature(int row);
	void train();
	void assign_labels();
};

#endif /* trainner2_H_ */
