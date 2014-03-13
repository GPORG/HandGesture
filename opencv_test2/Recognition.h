/*
 * Recognition.h
 *
 *  Created on: Mar 6, 2014
 *      Author: Mohamed
 */

#ifndef RECOGNITION_H_
#define RECOGNITION_H_
#include <string.h>
#include <math.h>
#include <iostream>
#include "opencv/cv.h"
#include <algorithm>
using namespace std;
using namespace cv;

class Recognition {
public:
	Recognition();
	virtual ~Recognition();
	string detect_gesture();
	void setData(CvPoint centero);
	static vector<CvPoint> fingers;
private:
	string no_fingers();
	string one_finger_gestures();
	string two_fingers_gestures();
	string three_fingers_gestures();
	string four_fingers_gestures();
	string five_fingers_gestures();
	string more_than_five();
	static  bool compare(const CvPoint &p1,const CvPoint& p2);
	//helper functions
	float get_angle(CvPoint o, CvPoint p, bool x_axes);
	float get_angle(CvPoint o,CvPoint p1,CvPoint p2);
	float distance(CvPoint p1, CvPoint p2);
	//variables
	CvPoint center;
};

#endif /* RECOGNITION_H_ */
