/*
 * trainner.h
 *
 *  Created on: Mar 23, 2014
 *      Author: Mohamed
 */

#ifndef TRAINNER_H_
#define TRAINNER_H_

#include "FileReader.h"
#include "filewriter.h"
#include "string.h"
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"

#include "My_SVM.h"
using namespace std;
using namespace cv;

class trainner {
public:
	trainner();
	void extract_feature(int row);
	void train();
	void assign_lables();
	//vars
	IplImage*hand;
	static const int dataset_size = 49;
	float data[dataset_size][100];
	float labels[dataset_size];

};

#endif /* TRAINNER_H_ */
