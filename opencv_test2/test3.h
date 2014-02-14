/*
 * test3.h
 *
 *  Created on: Feb 8, 2014
 *      Author: Mohamed
 */

#ifndef TEST3_H_
#define TEST3_H_

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;
class test3 {
public:
	test3();
	void detectAndDraw(IplImage *img);
	virtual ~test3();

	//variables

	 double scale ;

		//1.0 api version
		CvMemStorage* storage ;
		CvHaarClassifierCascade* cascade ;
		const char* cascade_name ;
		//define the path to cascade file
		string cascadeName;		/*ROBUST-fist detection haartraining file*/

};

#endif /* TEST3_H_ */
